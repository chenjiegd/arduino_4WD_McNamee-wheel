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


/*小车初始速度控制*/
int CarSpeed = 200;

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
}

void loop()
{
	brake();
	// run(100);
	// delay(1000);
	// back(100);
	// delay(1000);
	// left(200);
	// delay(1000);
	// brake();
	// delay(1000);
	Servo180(1,90);
	Servo180(2,90);
	Servo180(3,90);
	Servo180(4,90);
	delay(1000);
	Servo180(1,180);
	Servo180(2,180);
	Servo180(3,180);
	Servo180(4,180);
	delay(1000);
	Servo180(1,0);
	Servo180(2,0);
	Servo180(3,0);
	Servo180(4,0);
	delay(1000);
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
* @date          2019.06.25
* @brief         小车左移(左前后退，左后前进，右前前进，右后后退)
* @param[in]     Speed
* @param[out]    void
* @retval        void
* @par History   无
*/
void left(int Speed)
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
* Function       right
* @author        wusicaijuan
* @date          2019.06.25
* @brief         小车右移(左前前进，左后后退，右前后退，右后前进)
* @param[in]     Speed
* @param[out]    void
* @retval        void
* @par History   无
*/
void right(int Speed)
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
	
	long us = (degree * 1800 / 180 + 600); 	// 0.6 ~ 2.4
	long pwmvalue = us * 4096 / 20000;		// 50hz: 20,000 us
	pwm.setPWM(enServo[num-1], 0, pwmvalue);
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
	long us = (degree * 1800 / 180 + 600); 	// 0.6 ~ 2.4
	long pwmvalue = us * 4096 / 20000;		// 50hz: 20,000 us
	pwm.setPWM(enServo[num-1], 0, pwmvalue);
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