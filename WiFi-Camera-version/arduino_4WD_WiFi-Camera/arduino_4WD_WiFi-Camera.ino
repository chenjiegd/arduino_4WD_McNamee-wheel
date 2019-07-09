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

#define run_car '1'		   //按键前
#define back_car '2'	   //按键后
#define left_car '3'	   //按键左
#define right_car '4'	  //按键右
#define spin_left_car '5'  //按键左旋
#define spin_right_car '6' //按键右旋
#define stop_car '7'	   //按键停
#define servoL 'L'		   //舵机左转
#define servoR 'R'		   //舵机右转
#define servoS 'S'		   //舵机停止

#define ON 1  //使能LED
#define OFF 0 //禁止LED

/*小车运行状态枚举*/
const typedef enum {
	enRUN = 1,
	enBACK,
	enLEFT,
	enRIGHT,
	enSPINLEFT,
	enSPINRIGHT,
	enSTOP
} enCarState;

/*小车摄像头左右舵机状态枚举*/
const typedef enum {
	enServoL = 1,
	enServoR,
	enServoS
} enServoState;

const char enServo[] = {0, 1, 2, 3};

const int key = 7; //按键key

int flag = 0;//开机欢迎界面标志位
int Servo_LR = 90;

/*电压引脚及其变量设置*/
const int VoltagePin = A2;
double VoltageValue = 0;//电压值

const int flag_time = 2000;//时间标记点间隔2s
int newtime = 0;//记录系统当前时间
int lasttime = 0;//记录上一次系统时间标记点

/*小车初始速度控制*/
static int CarSpeedControl = 150;

/*串口数据设置*/
int IncomingByte = 0;			 //接收到的 data byte
String InputString = "";		 //用来储存接收到的内容
boolean NewLineReceived = false; //前一次数据结束标志
boolean StartBit = false;		 //协议开始标志
/*电机舵机状态*/
static int g_CarState = enSTOP;		//1前2后3左4右5左旋6右旋7停止
static int g_ServoState = enServoS; //1左转2右转3停止

/**
* Function       setup
* @author        wusicaijuan
* @date          2019.07.04
* @brief         初始化配置
* @param[in]     void
* @retval        void
* @par History   无
*/
void setup()
{
	//串口波特率设置
	Serial.begin(9600);

	strip.begin();
	strip.show();
	PCB_RGB_OFF();

	pwm.begin();
	pwm.setPWMFreq(60); // Analog servos run at ~60 Hz updates
	Clear_All_PWM();
	//舵机归位
	Servo180(1, 90);
	Servo180(2, Servo_LR);

	// PCB_LED();
	breathing_light(255, 40, 5);

	pinMode(VoltagePin, INPUT);

	pinMode(key, INPUT); //定义按键输入脚

	// by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
	display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // initialize with the I2C addr 0x3C (for the 128x32)
	// init done
	display.display(); // show splashscreen
	delay(2000);
	// display.clearDisplay(); // clears the screen and buffer

	welcome();
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
* @brief         小车左移
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
* @brief         小车右移
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
* Function       PCB_RGB_OFF()
* @author        wusicaijuan
* @date          2019.06.26
* @brief         关闭板载RGB灯
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
* @brief         开机呼吸灯
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
* @author        wusicaijuan
* @date          2019.07.04
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
}

/**
* Function       serial_data_parse
* @author        wusicaijuan
* @date          2019.07.04
* @brief         串口数据解析并指定相应的动作
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void serial_data_parse()
{

	//解析上位机发来的通用协议指令,并执行相应的动作
	//$4WD,UD180#
	if (InputString.indexOf("4WD") > 0)
	{
		//解析上位机发来的舵机云台的控制指令并执行舵机旋转
		//$4WD,UD180# 舵机转动到180度
		if (InputString.indexOf("UD") > 0)
		{
			int i = InputString.indexOf("UD"); //寻找以PTZ开头,#结束中间的字符
			int ii = InputString.indexOf("#", i);
			if (ii > i)
			{
				String m_skp = InputString.substring(i + 2, ii);
				int m_kp = m_skp.toInt(); //将找到的字符串变成整型
				Servo180(3, 180 - m_kp);  //转动到指定角度m_kp
			}
			InputString = ""; //清空串口数据
			NewLineReceived = false;
			return; //直接跳出本次loop()
		}

		//解析上位机发来的舵机云台的控制指令并执行舵机旋转
		//$4WD,LRS# 舵机停止转动
		//$4WD,LRL# 舵机左转
		//$4WD,LRR# 舵机右转
		if (InputString.indexOf("LR") > 0)
		{
			switch (InputString[7])
			{
			case servoL:
				g_ServoState = enServoL;
				break;
			case servoR:
				g_ServoState = enServoR;
				break;
			case servoS:
				g_ServoState = enServoS;
				break;
			}
		}
		InputString = ""; //清空串口数据
		NewLineReceived = false;
		return;
	}
	//解析上位机发来的通用协议指令,并执行相应的动作
	//如:$1,0,0,0#    小车前进
	//检测长度以防误判
	if ((InputString.indexOf("4WD") == -1) && (InputString.length() == 9))
	{
		//小车加减速判断
		if (InputString[3] == '1') //加速，每次加50
		{
			CarSpeedControl += 50;
			if (CarSpeedControl > 150)
			{
				CarSpeedControl = 150;
			}
			InputString = ""; //清空串口数据
			NewLineReceived = false;
			return;
		}
		if (InputString[3] == '2') //减速，每次减50
		{
			CarSpeedControl -= 50;
			if (CarSpeedControl < 50)
			{
				CarSpeedControl = 50;
			}
			InputString = ""; //清空串口数据
			NewLineReceived = false;
			return;
		}

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
		case spin_left_car:
			g_CarState = enSPINLEFT;
			break;
		case spin_right_car:
			g_CarState = enSPINRIGHT;
			break;
		case stop_car:
			g_CarState = enSTOP;
			break;
		default:
			g_CarState = enSTOP;
			break;
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
		case enSPINLEFT:
			spin_left(CarSpeedControl);
			break;
		case enSPINRIGHT:
			spin_right(CarSpeedControl);
			break;
		default:
			brake();
			break;
		}
	}
	InputString = ""; //清空串口数据
	NewLineReceived = false;
	return;
}
/**
* Function       loop
* @author        wusicaijuan
* @date          2019.07.08
* @brief         对串口发送过来的数据解析，并执行相应的指令
* @param[in]     void
* @retval        void
* @par History   无
*/

void loop()
{
	serialEvent();
	if (NewLineReceived)
	{
		if (flag == 0)
		{
			flag = 1;
		}
		// 调试查看串口数据
		// Serial.println(InputString);
		serial_data_parse(); //调用串口解析函数
	}
	if (flag == 1)
	{
		voltage_test();
		newtime = millis();
		if (newtime - lasttime > flag_time)
		{
			Controling();
			Display_voltage();
			lasttime = newtime;
			InputString = ""; //清空串口数据
		}
	}
	Servo_State();
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
* Function       Display_voltage
* @author        wusicaijuan
* @date          2019.07.04
* @brief         OLED显示电压
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

/*
* Function       Clear_All_PWM
* @author        wusicaijuan
* @date          2019.07.04
* @brief         关闭PCA9685所有PWM
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void Clear_All_PWM()
{
	for (int i = 0; i < 16; i++)
	{
		pwm.setPWM(i, 0, 0);
	}
}

/*
* Function       Servo_State
* @author        wusicaijuan
* @date          2019.07.04
* @brief         舵机状态控制
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void Servo_State()
{
	if (g_ServoState != enServoS)
	{
		if (g_ServoState == enServoL)
		{
			Servo_LR++;
			if (Servo_LR > 180)
			{
				Servo_LR = 180;
			}
			Servo180(2, Servo_LR);
			delay(5);
		}
		if (g_ServoState == enServoR)
		{
			Servo_LR--;
			if (Servo_LR < 0)
			{
				Servo_LR = 0;
			}
			Servo180(2, Servo_LR);
			delay(5);
		}
		return;
	}
}