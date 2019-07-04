/**
* @par Copyright (C): 2010-2019, Shenzhen Yahboom Tech
* @file         CarRun.c
* @author       liusen
* @version      V1.0
* @date         2017.07.25
* @brief       小车前进后退左右综合实验
* @details
* @par History  见如下说明
*
*/
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
#define SERVOMIN 150 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX 600 // this is the 'maximum' pulse length count (out of 4096)

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
/*小车初始速度控制*/
int CarSpeedControl = 2000;
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

	//  for(int i = 2; i < 14; i++)
	//  {
	//    pinMode(i, OUTPUT);
	//  }

	pwm.begin();

	pwm.setPWMFreq(50); // Analog servos run at ~60 Hz updates
}

/**
* Function       run
* @author        liusen
* @date          2017.07.25
* @brief         小车前进
* @param[in]     time
* @param[out]    void
* @retval        void
* @par History   无
*/
void run(int Speed)
{

	pwm.setPWM(8, 0, Speed);
	pwm.setPWM(9, 0, 0);
	pwm.setPWM(11, 0, Speed);
	pwm.setPWM(10, 0, 0);

	pwm.setPWM(0, 0, Speed);
	pwm.setPWM(1, 0, 0);
	pwm.setPWM(3, 0, Speed);
	pwm.setPWM(2, 0, 0);

	pwm.setPWM(12, 0, Speed);
	pwm.setPWM(13, 0, 0);
	pwm.setPWM(15, 0, Speed);
	pwm.setPWM(14, 0, 0);
}

/**
* Function       back
* @author        liusen
* @date          2017.07.25
* @brief         小车后退
* @param[in]     time
* @param[out]    void
* @retval        void
* @par History   无
*/
void back(int Speed)
{
	pwm.setPWM(8, 0, 0);
	pwm.setPWM(9, 0, Speed);
	pwm.setPWM(11, 0, 0);
	pwm.setPWM(10, 0, Speed);

	pwm.setPWM(0, 0, 0);
	pwm.setPWM(1, 0, Speed);
	pwm.setPWM(3, 0, 0);
	pwm.setPWM(2, 0, Speed);

	pwm.setPWM(12, 0, 0);
	pwm.setPWM(13, 0, Speed);
	pwm.setPWM(15, 0, 0);
	pwm.setPWM(14, 0, Speed);
}

/**
* Function       brake
* @author        liusen
* @date          2017.07.25
* @brief         小车刹车
* @param[in]     time
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

	pwm.setPWM(0, 0, 0);
	pwm.setPWM(1, 0, 0);
	pwm.setPWM(3, 0, 0);
	pwm.setPWM(2, 0, 0);

	pwm.setPWM(12, 0, 0);
	pwm.setPWM(13, 0, 0);
	pwm.setPWM(15, 0, 0);
	pwm.setPWM(14, 0, 0);
}

/**
* Function       left
* @author        liusen
* @date          2017.07.25
* @brief         小车左转 左转(左轮不动,右轮前进)
* @param[in]     time
* @param[out]    void
* @retval        void
* @par History   无
*/
void left(int Speed)
{
	pwm.setPWM(8, 0, Speed);
	pwm.setPWM(9, 0, 0);
	pwm.setPWM(11, 0, 0);
	pwm.setPWM(10, 0, Speed);

	pwm.setPWM(0, 0, Speed);
	pwm.setPWM(1, 0, 0);
	pwm.setPWM(3, 0, 0);
	pwm.setPWM(2, 0, Speed);

	pwm.setPWM(12, 0, Speed);
	pwm.setPWM(13, 0, 0);
	pwm.setPWM(15, 0, 0);
	pwm.setPWM(14, 0, Speed);
}

/**
* Function       right
* @author        liusen
* @date          2017.07.25
* @brief         小车右转 右转(左轮前进,右轮不动)
* @param[in]     time
* @param[out]    void
* @retval        void
* @par History   无
*/
void right(int Speed)
{
	pwm.setPWM(8, 0, 0);
	pwm.setPWM(9, 0, Speed);
	pwm.setPWM(11, 0, Speed);
	pwm.setPWM(10, 0, 0);

	pwm.setPWM(0, 0, 0);
	pwm.setPWM(1, 0, Speed);
	pwm.setPWM(3, 0, Speed);
	pwm.setPWM(2, 0, 0);

	pwm.setPWM(12, 0, 0);
	pwm.setPWM(13, 0, Speed);
	pwm.setPWM(15, 0, Speed);
	pwm.setPWM(14, 0, 0);
}

/**
* Function       spin_left
* @author        liusen
* @date          2017.07.25
* @brief         小车原地左转 原地左转(左轮后退，右轮前进)
* @param[in]     time
* @param[out]    void
* @retval        void
* @par History   无
*/
void spin_left(int Speed)
{
	pwm.setPWM(8, 0, Speed);
	pwm.setPWM(9, 0, 0);
	pwm.setPWM(11, 0, 0);
	pwm.setPWM(10, 0, Speed);

	pwm.setPWM(0, 0, Speed);
	pwm.setPWM(1, 0, 0);
	pwm.setPWM(3, 0, 0);
	pwm.setPWM(2, 0, Speed);

	pwm.setPWM(12, 0, Speed);
	pwm.setPWM(13, 0, 0);
	pwm.setPWM(15, 0, 0);
	pwm.setPWM(14, 0, Speed);
}

/**
* Function       spin_right
* @author        liusen
* @date          2017.07.25
* @brief         小车原地右转 原地右转(右轮后退，左轮前进)
* @param[in]     time
* @param[out]    void
* @retval        void
* @par History   无
*/
void spin_right(int Speed)
{
	pwm.setPWM(8, 0, 0);
	pwm.setPWM(9, 0, Speed);
	pwm.setPWM(11, 0, Speed);
	pwm.setPWM(10, 0, 0);

	pwm.setPWM(0, 0, 0);
	pwm.setPWM(1, 0, Speed);
	pwm.setPWM(3, 0, Speed);
	pwm.setPWM(2, 0, 0);

	pwm.setPWM(12, 0, 0);
	pwm.setPWM(13, 0, Speed);
	pwm.setPWM(15, 0, Speed);
	pwm.setPWM(14, 0, 0);
}

void SetServoPos(int num, int value)
{
	// 50hz: 20,000 us
	long us = (value * 1800 / 180 + 600); // 0.6 ~ 2.4
	long pwmvalue = us * 4096 / 20000;
	pwm.setPWM(num, 0, pwmvalue);
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
	if (InputString.indexOf("4WD") > 0)
	{
		//解析上位机发来的舵机云台的控制指令并执行舵机旋转
		//如:$4WD,PTZ180# 舵机转动到180度
		if (InputString.indexOf("PTZ") > 0)
		{
			int m_kp;
			int i = InputString.indexOf("PTZ"); //寻找以PTZ开头,#结束中间的字符
			int ii = InputString.indexOf("#", i);
			if (ii > i)
			{
				String m_skp = InputString.substring(i + 3, ii);
				int m_kp = m_skp.toInt(); //将找到的字符串变成整型
				//Serial.print("PTZ:");
				//Serial.println(m_kp);

				SetServoPos(7, 180 - m_kp); //转动到指定角度m_kp
				//150 600   0-180
				//m_kp = map(180 - m_kp, 0, 180, SERVOMIN, SERVOMAX);
				// Serial.println(m_kp);
				//setServoPulse(7, SERVOMIN);

				InputString = ""; //清空串口数据
				NewLineReceived = false;
				return;
			}
		}
	}
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
			SetServoPos(7, 180); //转动到指定角度
		}

		if (InputString[9] == '2') //舵机旋转到0度
		{
			SetServoPos(7, 0); //转动到指定角度
		}

		//舵机归为判断
		if (InputString[17] == '1') //舵机旋转到90度
		{
			SetServoPos(7, 90); //转动到指定角度
		}

		//小车加减速判断
		if (InputString[7] == '1') //加速，每次加50
		{
			CarSpeedControl += 1000;
			if (CarSpeedControl > 4095)
			{
				CarSpeedControl = 4095;
			}
		}
		if (InputString[7] == '2') //减速，每次减50
		{
			CarSpeedControl -= 1000;
			if (CarSpeedControl < 0)
			{
				CarSpeedControl = 500;
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
		serial_data_parse(); //调用串口解析函数
	}
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
