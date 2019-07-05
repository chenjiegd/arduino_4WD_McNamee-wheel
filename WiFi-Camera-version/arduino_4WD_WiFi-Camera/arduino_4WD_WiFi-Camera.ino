/**
* @par Copyright (C): 2010-2019, Shenzhen Yahboom Tech
* @file         arduino_4WD_McNamee-wheel_WiFi-Camera
* @author       wusicaijuan
* @version      V1.0
* @date         2019.07.04
* @brief       	wifi摄像头控制arduino4WD麦克纳姆轮小车
* @details
* @par History  见如下说明
*
*/
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <Adafruit_NeoPixel.h> //库文件
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
#define SERVOMIN 150 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX 600 // this is the 'maximum' pulse length count (out of 4096)

#define PIN 6	 //定义RGB灯的引脚
#define MAX_LED 1 //小车一共有1个RGB灯
Adafruit_NeoPixel strip = Adafruit_NeoPixel(MAX_LED, PIN, NEO_RGB + NEO_KHZ800);

#define OLED_RESET 11
Adafruit_SSD1306 display(OLED_RESET);

#define LOGO16_GLCD_HEIGHT 32
#define LOGO16_GLCD_WIDTH 128

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

#define run_car '1'   //按键前
#define back_car '2'  //按键后
#define left_car '3'  //按键左
#define right_car '4' //按键右
#define stop_car '0'  //按键停

#define ON 1  //使能LED
#define OFF 0 //禁止LED

/*小车运行状态枚举*/
enum
{
	enSTOP = 0,
	enRUN,
	enBACK,
	enLEFT,
	enRIGHT,
	enTLEFT,
	enTRIGHT
} enCarState;

const char enServo[] = {0, 1, 2, 3};

int flag = 0;

/*电压引脚及其变量设置*/
const int VoltagePin = A2;
double VoltageValue = 0;

/*小车初始速度控制*/
int CarSpeedControl = 150;

/*串口数据设置*/
int IncomingByte = 0;			 //接收到的 data byte
String InputString = "";		 //用来储存接收到的内容
boolean NewLineReceived = false; //前一次数据结束标志
boolean StartBit = false;		 //协议开始标志
String ReturnTemp = "";			 //存储返回值

/*状态机状态*/
int g_CarState = enSTOP; //1前2后3左4右0停止
int g_modeSelect = 0;	//0是默认状态;  1:红外遥控 2:巡线模式 3:超声波避障 4: 七彩探照 5: 寻光模式 6: 红外跟踪
boolean g_motor = false;

/**
* Function       setup
* @author        liusen
* @date          2017.07.25
* @brief         初始化配置
* @param[in]     void
* @retval        void
* @par History   无
*/
void setup()
{
	//初始化电机驱动IO口为输出方式
	//串口波特率设置
	Serial.begin(9600);

	strip.begin();
	strip.show();
	PCB_RGB_OFF();

	pwm.begin();
	pwm.setPWMFreq(60); // Analog servos run at ~60 Hz updates
	//电机驱动
	pwm.setPWM(8, 0, 0);
	pwm.setPWM(9, 0, 0);
	pwm.setPWM(11, 0, 0);
	pwm.setPWM(10, 0, 0);

	pwm.setPWM(12, 0, 0);
	pwm.setPWM(13, 0, 0);
	pwm.setPWM(14, 0, 0);
	pwm.setPWM(15, 0, 0);
	//外接RGB
	pwm.setPWM(6, 0, 0);
	pwm.setPWM(5, 0, 0);
	pwm.setPWM(4, 0, 0);

	// PCB_LED();
	breathing_light(255, 40, 5);

	pinMode(VoltagePin, INPUT);

	// by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
	display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // initialize with the I2C addr 0x3C (for the 128x32)
	// init done
	display.display(); // show splashscreen
	delay(2000);
	// display.clearDisplay(); // clears the screen and buffer

	welcome();
	delay(3000);
}

/**
* Function       run
* @author        wusicaijuan
* @date          2019.06.25
* @brief         小车前进
* @param[in]     Speed
* @param[out]    void
* @retval        void
* @par History   无
*/
void run(int Speed)
{
	Speed = map(Speed, 0, 255, 0, 4095);
	pwm.setPWM(10, 0, Speed); //右前
	pwm.setPWM(11, 0, 0);
	pwm.setPWM(8, 0, Speed); //右后
	pwm.setPWM(9, 0, 0);

	pwm.setPWM(13, 0, Speed); //左前
	pwm.setPWM(12, 0, 0);
	pwm.setPWM(15, 0, Speed); //左后
	pwm.setPWM(14, 0, 0);
}

/**
* Function       back
* @author        wusicaijuan
* @date          2019.06.25
* @brief         小车后退
* @param[in]     Speed
* @param[out]    void
* @retval        void
* @par History   无
*/
void back(int Speed)
{
	Speed = map(Speed, 0, 255, 0, 4095);
	pwm.setPWM(10, 0, 0);
	pwm.setPWM(11, 0, Speed);
	pwm.setPWM(8, 0, 0);
	pwm.setPWM(9, 0, Speed);

	pwm.setPWM(13, 0, 0);
	pwm.setPWM(12, 0, Speed);
	pwm.setPWM(15, 0, 0);
	pwm.setPWM(14, 0, Speed);
}

/**
* Function       brake
* @author        wusicaijuan
* @date          2019.06.25
* @brief         小车刹车
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void brake()
{
	pwm.setPWM(8, 0, 0);
	pwm.setPWM(9, 0, 0);
	pwm.setPWM(11, 0, 0);
	pwm.setPWM(10, 0, 0);

	pwm.setPWM(12, 0, 0);
	pwm.setPWM(13, 0, 0);
	pwm.setPWM(14, 0, 0);
	pwm.setPWM(15, 0, 0);
}

/**
* Function       left
* @author        wusicaijuan
* @date          2019.06.26
* @brief         小车左转(左轮不动，右轮前进)
* @param[in]     Speed
* @param[out]    void
* @retval        void
* @par History   无
*/
void left(int Speed)
{
	Speed = map(Speed, 0, 255, 0, 4095);
	pwm.setPWM(10, 0, Speed);
	pwm.setPWM(11, 0, 0);
	pwm.setPWM(8, 0, 0);
	pwm.setPWM(9, 0, Speed);

	pwm.setPWM(13, 0, 0);
	pwm.setPWM(12, 0, Speed);
	pwm.setPWM(15, 0, Speed);
	pwm.setPWM(14, 0, 0);
}

/**
* Function       right
* @author        wusicaijuan
* @date          2019.06.26
* @brief         小车右转(右轮不动，左轮前进)
* @param[in]     Speed
* @param[out]    void
* @retval        void
* @par History   无
*/
void right(int Speed)
{
	Speed = map(Speed, 0, 255, 0, 4095);
	pwm.setPWM(10, 0, 0);
	pwm.setPWM(11, 0, Speed);
	pwm.setPWM(8, 0, Speed);
	pwm.setPWM(9, 0, 0);

	pwm.setPWM(13, 0, Speed);
	pwm.setPWM(12, 0, 0);
	pwm.setPWM(15, 0, 0);
	pwm.setPWM(14, 0, Speed);
}

/**
* Function       spin_left
* @author        wusicaijuan
* @date          2019.06.25
* @brief         小车原地左转(左轮后退，右轮前进)
* @param[in]     Speed
* @param[out]    void
* @retval        void
* @par History   无
*/
void spin_left(int Speed)
{
	Speed = map(Speed, 0, 255, 0, 4095);
	pwm.setPWM(10, 0, Speed);
	pwm.setPWM(11, 0, 0);
	pwm.setPWM(8, 0, Speed);
	pwm.setPWM(9, 0, 0);

	pwm.setPWM(13, 0, 0);
	pwm.setPWM(12, 0, Speed);
	pwm.setPWM(15, 0, 0);
	pwm.setPWM(14, 0, Speed);
}

/**
* Function       spin_right
* @author        wusicaijuan
* @date          2019.06.25
* @brief         小车原地右转(右轮后退，左轮前进)
* @param[in]     Speed
* @param[out]    void
* @retval        void
* @par History   无
*/
void spin_right(int Speed)
{
	Speed = map(Speed, 0, 255, 0, 4095);
	pwm.setPWM(10, 0, 0);
	pwm.setPWM(11, 0, Speed);
	pwm.setPWM(8, 0, 0);
	pwm.setPWM(9, 0, Speed);

	pwm.setPWM(13, 0, Speed);
	pwm.setPWM(12, 0, 0);
	pwm.setPWM(15, 0, Speed);
	pwm.setPWM(14, 0, 0);
}

/*
* Function      Servo180(num, degree)
* @author       wusicaijuan
* @date         2019.06.25
* @bried        180°舵机转动角度
                    180 Degree Steering Engine Rotation Angle
* @param[in1]   index
                    1: s1
                    2: s2
                    3: s3
                    4: s4
* @param[in2]   degree (0 <= degree <= 180)
* @retval       void
*/
void Servo180(int num, int degree)
{

	long us = (degree * 1800 / 180 + 600); // 0.6 ~ 2.4
	long pwmvalue = us * 4096 / 20000;	 // 50hz: 20,000 us
	pwm.setPWM(enServo[num - 1], 0, pwmvalue);
}

void setServoPulse(uint8_t n, double pulse)
{
	double pulselength;

	pulselength = 1000000; // 1,000,000 us per second
	pulselength /= 60;	 // 60 Hz
	//Serial.print(pulselength); Serial.println(" us per period");
	pulselength /= 4096; // 12 bits of resolution
	//Serial.print(pulselength); Serial.println(" us per bit");
	pulse *= 1000;
	pulse /= pulselength;
	//Serial.println(pulse);
	pwm.setPWM(n, 0, pulse);
}

/**
* Function       setRGB(R,G,B)
* @author        wusicaijuan
* @date          2019.06.26
* @brief         设置外接RGB灯
* @param[in1]	 R
* @param[in2]    G
* @param[in3]    B
* @param[out]    void
* @retval        void
* @par History   无
*/
void setRGB(int R, int G, int B)
{
	R = map(R, 0, 255, 0, 4095);
	G = map(G, 0, 255, 0, 4095);
	B = map(B, 0, 255, 0, 4095);
	pwm.setPWM(6, 0, R);
	pwm.setPWM(5, 0, G);
	pwm.setPWM(4, 0, B);
}

/**
* Function       PCB_RGB(R,G,B)
* @author        wusicaijuan
* @date          2019.06.26
* @brief         设置板载RGB灯
* @param[in1]	 R
* @param[in2]    G
* @param[in3]    B
* @param[out]    void
* @retval        void
* @par History   无
*/
void PCB_RGB(int R, int G, int B)
{
	uint8_t i = 0;
	R = map(R, 0, 255, 0, 10);
	G = map(G, 0, 255, 0, 10);
	B = map(B, 0, 255, 0, 10);
	uint32_t color = strip.Color(G, R, B);
	strip.setPixelColor(i, color);
	strip.show();
}

/**
* Function       PCB_RGB(R,G,B)
* @author        wusicaijuan
* @date          2019.06.26
* @brief         设置板载RGB灯
* @param[in1]	 void
* @param[out]    void
* @retval        void
* @par History   无
*/
void PCB_RGB_OFF()
{
	uint8_t i = 0;
	uint32_t color = strip.Color(0, 0, 0);
	strip.setPixelColor(i, color);
	strip.show();
}

/**
* Function       PCB_LED()
* @author        wusicaijuan
* @date          2019.07.03
* @brief         设置板载LED灯
* @param[in1]	 void
* @param[out]    void
* @retval        void
* @par History   无
*/
void PCB_LED()
{
	pwm.setPWM(7, 0, 4095);
}

/**
* Function       breathing_light(brightness,time,increament)
* @author        wusicaijuan
* @date          2019.06.26
* @brief         呼吸灯
* @param[in1]	 brightness
* @param[in2]    time
* @param[in3]    increament
* @param[out]    void
* @retval        void
* @par History   无
*/
void breathing_light(int brightness, int time, int increament)
{
	if (brightness < 0)
	{
		brightness = 0;
	}
	if (brightness > 255)
	{
		brightness = 255;
	}
	for (int b = 0; b < brightness; b += increament)
	{
		int newb = map(b, 0, 255, 0, 4095);
		pwm.setPWM(7, 0, newb);
		delay(time);
	}
}

/**
* Function       voltage_test
* @author        Danny
* @date          2017.07.26
* @brief         电池电压引脚检测
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
float voltage_test()
{
	VoltageValue = analogRead(VoltagePin); //读取A0口值,换算为电压值

	//方法一:通过电路原理图和采集的A0口模拟值得到电压值
	// Serial.println(VoltageValue);
	VoltageValue = (VoltageValue / 1023) * 5.02 * 4.03;
	return VoltageValue;
	//Voltage是端口A0采集到的ad值（0-1023），
	//1.75是（R14+R15）/R15的结果，其中R14=15K,R15=20K）。

	/*查表记录打开*/
	// float voltage = 0;
	// voltage = VoltageValue;
	// return voltage;

	//方法二:通过提前测量6.4-8.4v所对应的A0口模拟值,再通过查表法确定其值
	//       这种方法的误差小于0.1v
	// int i = 0;
	// float voltage = 0;
	// if (VoltageValue > voltage_table[20][1])
	// {
	// 	voltage = 8.4;
	// 	return voltage;
	// }
	// if (VoltageValue < voltage_table[0][1])
	// {
	// 	voltage = 6.4;
	// 	return voltage;
	// }
	// for (i = 0; i < 20; i++)
	// {
	// 	if (VoltageValue >= voltage_table[i][1] && VoltageValue <= voltage_table[i + 1][1])
	// 	{
	// 		voltage = voltage_table[i][0] + (VoltageValue - voltage_table[i][1]) * ((voltage_table[i + 1][0] - voltage_table[i][0]) / (voltage_table[i + 1][1] - voltage_table[i][1]));
	// 		return voltage;
	// 	}
	// }
	return 0;
	// return;
}

/**
* Function       serial_data_parse
* @author        liusen
* @date          2017.07.25
* @brief         串口数据解析并指定相应的动作
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void serial_data_parse()
{

	//解析上位机发来的通用协议指令,并执行相应的动作
	//$4WD,PTZ180#
	// if (InputString.indexOf("4WD") > 0)
	// {
	// 	//解析上位机发来的舵机云台的控制指令并执行舵机旋转
	// 	//如:$4WD,PTZ180# 舵机转动到180度
	// 	if (InputString.indexOf("PTZ") > 0)
	// 	{
	// 		int m_kp;
	// 		int i = InputString.indexOf("PTZ"); //寻找以PTZ开头,#结束中间的字符
	// 		int ii = InputString.indexOf("#", i);
	// 		if (ii > i)
	// 		{
	// 			String m_skp = InputString.substring(i + 3, ii);
	// 			int m_kp = m_skp.toInt(); //将找到的字符串变成整型
	// 			//Serial.print("PTZ:");
	// 			//Serial.println(m_kp);

	// 			Servo180(7, 180 - m_kp); //转动到指定角度m_kp
	// 			//150 600   0-180
	// 			//m_kp = map(180 - m_kp, 0, 180, SERVOMIN, SERVOMAX);
	// 			// Serial.println(m_kp);
	// 			//setServoPulse(7, SERVOMIN);

	// 			InputString = ""; //清空串口数据
	// 			NewLineReceived = false;
	// 			return;
	// 		}
	// 	}
	// }
	//如:$1,0,0,0,0,0,0,0,0,0#    小车前进
	if (InputString.indexOf("4WD") == -1)
	{
		//小车原地左旋右旋判断
		if (InputString[3] == '1') //小车原地左旋
		{
			g_CarState = enTLEFT;
		}
		else if (InputString[3] == '2') //小车原地右旋
		{
			g_CarState = enTRIGHT;
		}
		else
		{
			g_CarState = enSTOP;
		}
		//舵机左旋右旋判断
		if (InputString[9] == '1') //舵机旋转到180度
		{
			Servo180(7, 180); //转动到指定角度
		}

		if (InputString[9] == '2') //舵机旋转到0度
		{
			Servo180(7, 0); //转动到指定角度
		}

		//舵机归为判断
		if (InputString[17] == '1') //舵机旋转到90度
		{
			Servo180(7, 90); //转动到指定角度
		}

		//小车加减速判断
		if (InputString[7] == '1') //加速，每次加50
		{
			CarSpeedControl += 50;
			if (CarSpeedControl > 255)
			{
				CarSpeedControl = 255;
			}
		}
		if (InputString[7] == '2') //减速，每次减50
		{
			CarSpeedControl -= 50;
			if (CarSpeedControl < 50)
			{
				CarSpeedControl = 50;
			}
		}
		//小车的前进,后退,左转,右转,停止动作
		if (g_CarState != enTLEFT && g_CarState != enTRIGHT)
		{
			switch (InputString[1])
			{
			case run_car:
				g_CarState = enRUN;
				break;
			case back_car:
				g_CarState = enBACK;
				break;
			case left_car:
				g_CarState = enLEFT;
				break;
			case right_car:
				g_CarState = enRIGHT;
				break;
			case stop_car:
				g_CarState = enSTOP;
				break;
			default:
				g_CarState = enSTOP;
				break;
			}
		}

		InputString = ""; //清空串口数据
		NewLineReceived = false;

		//根据小车状态做相应的动作
		switch (g_CarState)
		{
		case enSTOP:
			brake();
			break;
		case enRUN:
			run(CarSpeedControl);
			break;
		case enLEFT:
			left(CarSpeedControl);
			break;
		case enRIGHT:
			right(CarSpeedControl);
			break;
		case enBACK:
			back(CarSpeedControl);
			break;
		case enTLEFT:
			spin_left(CarSpeedControl);
			break;
		case enTRIGHT:
			spin_right(CarSpeedControl);
			break;
		default:
			brake();
			break;
		}
	}
}
/**
* Function       loop
* @author        liusen
* @date          2017.07.25
* @brief         先延时2，再前进1，后退1s,左转2s,右转2s,
*                原地左转3s,原地右转3s,停止0.5s
* @param[in]     void
* @retval        void
* @par History   无
*/

void loop()
{
	if (NewLineReceived)
	{
		if (flag == 0)
		{
			Controling();
			Display_voltage();
			flag = 1;
		}
		Serial.println(InputString);
		serial_data_parse(); //调用串口解析函数
	}
	voltage_test();
}

/**
* Function       serialEvent
* @author        liusen
* @date          2017.07.25
* @brief         串口解包
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/

void serialEvent()
{
	while (Serial.available())
	{
		//一个字节一个字节地读,下一句是读到的放入字符串数组中组成一个完成的数据包
		IncomingByte = Serial.read();
		if (IncomingByte == '$')
		{
			StartBit = true;
		}
		if (StartBit == true)
		{
			InputString += (char)IncomingByte;
		}
		if (IncomingByte == '#')
		{
			NewLineReceived = true;
			StartBit = false;
		}
	}
}

/*
* Function       Controling
* @author        wusicaijuan
* @date          2019.07.01
* @brief         控制模式
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void Controling()
{
	PCB_RGB(0, 255, 0);
	display.setTextSize(2);
	display.setTextColor(WHITE);
	display.setCursor(0, 0);
	display.clearDisplay();
	display.println("Remoting!");
	display.display();
}

/*
* Function       Controling
* @author        wusicaijuan
* @date          2019.07.04
* @brief         控制模式
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void Display_voltage()
{
	display.setTextSize(2);
	display.setTextColor(WHITE);
	display.setCursor(0, 16);
	display.print("DCV:");
	display.setCursor(46, 16);
	display.print(VoltageValue);
	display.setCursor(94, 16);
	display.print("V");
	display.display();
}

/*
* Function       welcome
* @author        wusicaijuan
* @date          2019.07.01
* @brief         欢迎词
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void welcome()
{
	PCB_RGB(255, 0, 0);
	display.setTextSize(2);
	display.setTextColor(WHITE);
	display.setCursor(0, 0);
	display.clearDisplay();
	display.println("Welcome!");
	display.println("Hola,Maker");
	display.display();
}
