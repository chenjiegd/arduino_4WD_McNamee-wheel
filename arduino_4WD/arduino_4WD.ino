#include <Arduino.h>
#include <Adafruit_NeoPixel.h> //编程灯
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <IRremote.h> //红外

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
#define SERVOMIN 150 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX 600 // this is the 'maximum' pulse length count (out of 4096)

#define DIN 9

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

char enServo[] = {0, 1, 2, 3};

int key = 7;   //按键key
int Echo = 13; //Echo回声脚
int Trig = 12; //Trig触发脚

/*小车初始速度控制*/
int CarSpeed = 200;
int distance = 0;

/**
* Function       setup
* @author        wusicaijuan
* @date          2019.06.25
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

	pwm.begin();

	pwm.setPWMFreq(60); // Analog servos run at ~60 Hz updates

	pinMode(key, INPUT); //定义按键输入脚
	//初始化超声波引脚
	pinMode(Echo, INPUT);  // 定义超声波输入脚
	pinMode(Trig, OUTPUT); // 定义超声波输出脚

	keysacn();
}

void loop()
{
	distance_test();
	if (distance < 50)
	{
		while (distance < 50)
		{
			/* code */
			spin_right(100);
			delay(20);
			brake();
			distance_test(); //测量前方距离
		}
	}
	else
	{
		/* code */
		run(150); //无障碍物，前进
	}
	/* code */
	// int Fdistance = 30;
	// int Ldistance = 0, Rdistance = 0;
	// int distance = distance_test(); //测量前方距离
	// Servo180(1, 90);
	// if (distance < Fdistance)
	// {
	// 	brake();
	// 	while (distance < Fdistance)
	// 	{
	// 		/* code */
	// 		Servo180(1, 0);
	// 		delay(1000);
	// 		Ldistance = distance_test();
	// 		delay(50);

	// 		Servo180(1, 180);
	// 		delay(1000);
	// 		Rdistance = distance_test();
	// 		delay(50);

	// 		Servo180(1, 90);

	// 		if (Ldistance > Rdistance)
	// 		{
	// 			spin_left(100);
	// 			delay(200);
	// 			brake();
	// 		}
	// 		else
	// 		{
	// 			/* code */
	// 			spin_right(100);
	// 			delay(200);
	// 			brake();
	// 		}
	// 		distance = distance_test(); //测量前方距离
	// 	}
	// }
	// else
	// {
	// 	/* code */
	// 	run(150); //无障碍物，前进
	// }
	
	// Ultrasonic_avoid(50);

	// Ultrasonic_avoid_servo(50);

	// breathing_light(255, 10, 1);

	// set_breath_RGB(255);

	// setRGB(255, 0, 0);
	// delay(1000);
	// setRGB(0, 255, 0);
	// delay(1000);
	// setRGB(0, 0, 255);
	// delay(1000);

	// brake();
	// run(100);
	// delay(1000);
	// back(100);
	// delay(1000);
	// left(200);
	// delay(1000);
	// right(200);
	// delay(1000);
	// spin_left(200);
	// delay(1000);
	// spin_right(200);
	// delay(1000);
	// brake();
	// delay(1000);

	// Servo180(1,90);
	// Servo180(2,90);
	// Servo180(3,90);
	// Servo180(4,90);
	// delay(1000);
	// Servo180(1,180);
	// Servo180(2,180);
	// Servo180(3,180);
	// Servo180(4,180);
	// delay(1000);
	// Servo180(1,0);
	// Servo180(2,0);
	// Servo180(3,0);
	// Servo180(4,0);
	// delay(1000);
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
	pwm.setPWM(8, 0, Speed);
	pwm.setPWM(9, 0, 0);

	pwm.setPWM(13, 0, 0);
	pwm.setPWM(12, 0, 0);
	pwm.setPWM(15, 0, 0);
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
	pwm.setPWM(11, 0, 0);
	pwm.setPWM(8, 0, 0);
	pwm.setPWM(9, 0, 0);

	pwm.setPWM(13, 0, Speed);
	pwm.setPWM(12, 0, 0);
	pwm.setPWM(15, 0, Speed);
	pwm.setPWM(14, 0, 0);
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

/*
* Function      Servo270(num, degree)
* @author       wusicaijuan
* @date         2019.06.25
* @bried        270°舵机转动角度
                    270 Degree Steering Engine Rotation Angle
* @param[in1]   index
                    1: s1
                    2: s2
                    3: s3
                    4: s4
* @param[in2]   degree (0 <= degree <= 270)
* @retval       void
*/
void Servo270(int num, int degree)
{
	degree = map(degree, 0, 255, 0, 4095);
	long us = (degree * 1800 / 180 + 600); // 0.6 ~ 2.4
	long pwmvalue = us * 4096 / 20000;	 // 50hz: 20,000 us
	pwm.setPWM(enServo[num - 1], 0, pwmvalue);
}

// you can use this function if you'd like to set the pulse length in seconds
// e.g. setServoPulse(0, 0.001) is a ~1 millisecond pulse width. its not precise!
void setServoPulse(uint8_t n, double pulse)
{
	double pulselength;

	pulselength = 1000000; // 1,000,000 us per second
	pulselength /= 60;	 // 60 Hz
	pulselength /= 4096;   // 12 bits of resolution
	pulse *= 1000;
	pulse /= pulselength;
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
		pwm.setPWM(6, 0, newb);
		delay(time);
	}
	for (int b = 255; b > 0; b -= increament)
	{
		int newb = map(b, 0, 255, 0, 4095);
		pwm.setPWM(6, 0, newb);
		delay(time);
	}
}

/**
* Function       keysacn
* @author        wusicaijuan
* @date          2019.06.04
* @brief         按键扫描
* @param[in1]    void
* @retval        void
* @par History   无
*/
void keysacn()
{
	int val;
	val = digitalRead(key); //读取数字7口电平值赋给val
	while (val == HIGH)		//当按键没被按下时，一直循环
	{
		val = digitalRead(key); //此句可省略，可让循环跑空
	}
	while (val == LOW) //当按键被按下时
	{
		delay(1);				//延时10ms
		val = digitalRead(key); //读取数字7口电平值赋给val
		while (val == HIGH)		//判断按键是否被松开
		{
			break;
		}
	}
}

/**
* Function       distance_test
* @author        wusicaijuan
* @date          2019.06.04
* @brief         测距
* @param[in1]    void
* @retval        distance
* @par History   无
*/
void distance_test() // 量出前方距离
{
	digitalWrite(Trig, LOW); // 给触发脚低电平2μs
	delayMicroseconds(2);
	digitalWrite(Trig, HIGH); // 给触发脚高电平10μs，这里至少是10μs
	delayMicroseconds(10);
	digitalWrite(Trig, LOW);			  // 持续给触发脚低电
	float Fdistance = pulseIn(Echo, HIGH); // 读取高电平时间(单位：微秒)
	distance = Fdistance / 58;			  //为什么除以58等于厘米，  Y米=（X秒*344）/2
	// X秒=（ 2*Y米）/344 ==》X秒=0.0058*Y米 ==》厘米=微秒/58
	Serial.print("Distance:"); //输出距离（单位：厘米）
	Serial.println(distance);  //显示距离
}

/**
* Function       Ultrasonic_avoid
* @author        wusicaijuan
* @date          2019.06.26
* @brief         超声波避障（无舵机）
* @param[in1]    Fdistance
* @retval        void
* @par History   无
*/
// void Ultrasonic_avoid(int Fdistance)
// {
// 	int distance = distance_test(); //测量前方距离
// 	if (distance < Fdistance)
// 	{
// 		while (distance < Fdistance)
// 		{
// 			/* code */
// 			spin_right(100);
// 			delay(20);
// 			brake();
// 			distance_test(); //测量前方距离
// 		}
// 	}
// 	else
// 	{
// 		/* code */
// 		run(250); //无障碍物，前进
// 	}
// }

/**
* Function       Ultrasonic_avoid
* @author        wusicaijuan
* @date          2019.06.26
* @brief         超声波避障（无舵机）
* @param[in1]    Fdistance
* @retval        void
* @par History   无
*/
// void Ultrasonic_avoid_servo(int Fdistance)
// {
// 	int Ldistance = 0, Rdistance = 0;
// 	int distance = distance_test(); //测量前方距离
// 	Servo180(1, 90);
// 	if (distance < Fdistance)
// 	{
// 		brake();
// 		while (distance < Fdistance)
// 		{
// 			/* code */
// 			Servo180(1, 0);
// 			delay(1000);
// 			Ldistance = distance_test();
// 			delay(50);

// 			Servo180(1, 180);
// 			delay(1000);
// 			Rdistance = distance_test();
// 			delay(50);

// 			Servo180(1, 90);

// 			if (Ldistance > Rdistance)
// 			{
// 				spin_left(100);
// 				delay(20);
// 			}
// 			else
// 			{
// 				/* code */
// 				spin_right(100);
// 				delay(20);
// 			}
// 			distance = distance_test(); //测量前方距离
// 		}
// 	}
// 	else
// 	{
// 		/* code */
// 		run(150); //无障碍物，前进
// 	}
// }