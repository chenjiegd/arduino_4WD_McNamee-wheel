/**
* @par Copyright (C): 2010-2019, Shenzhen Yahboom Tech
* @file         bluetooth_control.c
* @author       Danny
* @version      V1.0
* @date         2017.07.25
* @brief        蓝牙控制智能小车实验(无循迹)
* @details
* @par History  见如下说明
*
*/
#include <Adafruit_NeoPixel.h> //库文件
#define PIN 6				   //定义RGB灯的引脚
#define MAX_LED 1			   //小车一共有1个RGB灯

Adafruit_NeoPixel strip = Adafruit_NeoPixel(MAX_LED, PIN, NEO_RGB + NEO_KHZ800);

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <IRremote.h> //红外

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
#define SERVOMIN 150 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX 600 // this is the 'maximum' pulse length count (out of 4096)

#define run_car     '1'//按键前
#define back_car    '2'//按键后
#define left_car    '3'//按键左
#define right_car   '4'//按键右
#define stop_car    '0'//按键停

#define ON 1           //使能LED
#define OFF 0          //禁止LED

/*小车运行状态枚举*/
enum {
	enSTOP = 0,
	enRUN,
	enBACK,
	enLEFT,
	enRIGHT,
	enTLEFT,
	enTRIGHT
} enCarState;

char enServo[] = {0, 1, 2, 3};

int RECV_PIN = A0; //定义红外接收器的引脚为2
IRrecv irrecv(RECV_PIN);
decode_results results;
int on = 0; //标志位
unsigned long last = millis();
long flag_switch = 0xFF00FF;	 //开关
long flag_run = 0xFF807F;		 //前进
long flag_led = 0xFF40BF;		 //灯
long flag_left = 0xFF20DF;		 //左转
long flag_buzzer = 0xFFA05F;	 //喇叭
long flag_right = 0xFF609F;		 //右转
long flag_spin_left = 0xFF10EF;  //左旋
long flag_back = 0xFF906F;		 //后退
long flag_spin_right = 0xFF50AF; //右旋
long flag_plus = 0xFF30CF;		 //加号
long flag_zero = 0xFFB04F;		 //0
long flag_minus = 0xFF708F;		 //减号
long flag_one = 0xFF08F7;		 //1
long flag_two = 0xFF8877;		 //2
long flag_three = 0xFF48B7;		 //3
long flag_four = 0xFF28D7;		 //4
long flag_five = 0xFFA857;		 //5
long flag_six = 0xFF6897;		 //6
long flag_seven = 0xFF18E7;		 //7
long flag_eight = 0xFF9867;		 //8
long flag_nine = 0xFF58A7;		 //9

//循迹红外引脚定义
//TrackSensorLeftPin1 TrackSensorLeftPin2 TrackSensorRightPin1 TrackSensorRightPin2
//      A2                  A1                  A3                   A4
const int TrackSensorLeftPin1  =  4;  //定义左边第一个循迹红外传感器引脚为A2
const int TrackSensorLeftPin2  =  5;  //定义左边第二个循迹红外传感器引脚为A1
const int TrackSensorRightPin1 =  3;  //定义右边第一个循迹红外传感器引脚为A3
const int TrackSensorRightPin2 =  2;  //定义右边第二个循迹红外传感器引脚为A4

//定义各个循迹红外引脚采集的数据的变量
bool TrackSensorLeftValue1;
bool TrackSensorLeftValue2;
bool TrackSensorRightValue1;
bool TrackSensorRightValue2;

String infrared_track_value = "0000";

/*避障红外传感器引脚及变量设置*/
const int AvoidSensorLeft =  9;   //定义左边避障的红外传感器引脚为A3
const int AvoidSensorRight = 8;   //定义右边避障的红外传感器引脚为A1
const int FollowSensorLeft =  9;   //定义左边跟随的红外传感器引脚为A3
const int FollowSensorRight = 8;   //定义右边跟随的红外传感器引脚为A1

int LeftSensorValue ;              //定义变量来保存红外传感器采集的数据大小
int RightSensorValue ;
String infrared_avoid_value = "00";

/*定义光敏电阻引脚及变量设置*/
const int LdrSensorLeft =  A1;   //定义左边光敏电阻引脚为A4
const int LdrSensorRight = A3;   //定义右边光敏电阻引脚为A2

int LdrSersorLeftValue ;         //定义变量来保存光敏电阻采集的数据大小
int LdrSersorRightValue ;
String LDR_value = "00";

/*蜂鸣器引脚设置*/
int buzzer = A0;                //设置控制蜂鸣器引脚为A0

/*颜色识别引脚及变量设置*/
int LDR_pin = A5;
double position = 0; //七彩探照

/*电压引脚及其变量设置*/
int VoltagePin = A0;
int VoltageValue = 0;

/*小车初始速度控制*/
int CarSpeedControl = 150;

/*超声波引脚及变量设置*/
int EchoPin = 13;         //Echo回声脚
int TrigPin = 12;         //Trig触发脚
float distance = 0;

/*灭火电机引脚设置*/
// int OutfirePin = A5;

/*颜色值*/
int red, green, blue;

/*计时变量用于延时*/
int time = 20000;
int count = 10;

/*串口数据设置*/
int IncomingByte = 0;            //接收到的 data byte
String InputString = "";         //用来储存接收到的内容
boolean NewLineReceived = false; //前一次数据结束标志
boolean StartBit  = false;       //协议开始标志
String ReturnTemp = "";          //存储返回值
/*状态机状态*/
int g_CarState = enSTOP;         //1前2后3左4右0停止
int g_modeSelect = 0;  //0是默认状态;  1:红外遥控 2:巡线模式 3:超声波避障 4: 七彩探照 5: 寻光模式 6: 红外跟踪
boolean g_motor = false;

/*电压检测查表法定义数组(电压值,A0端口读到的模拟值)*/
float voltage_table[21][2] =
{
	{6.46, 676}, {6.51, 678}, {6.61, 683}, {6.72, 687}, {6.82, 691}, {6.91, 695}, {7.01, 700}, {7.11, 703},
	{7.20, 707}, {7.31, 712}, {7.4, 715}, {7.5, 719}, {7.6, 723}, {7.7, 728}, {7.81, 733}, {7.91, 740},
	{8.02, 741}, {8.1, 745}, {8.22, 749}, {8.30, 753}, {8.4, 758}
};

/*printf格式化字符串初始化*/
int serial_putc( char c, struct __file * )
{
	Serial.write( c );
	return c;
}
void printf_begin(void)
{
	fdevopen( &serial_putc, 0 );
}

/**
* Function       setup
* @author        Danny
* @date          2017.07.25
* @brief         初始化配置
* @param[in]     void
* @retval        void
* @par History   无
*/
void setup()
{
	//串口波特率设置
	Serial.begin(9600);
	printf_begin();
	//初始化电机驱动IO为输出方式
	pwm.begin();
	pwm.setPWMFreq(60); // Analog servos run at ~60 Hz updates

	strip.begin();
	strip.show();

	irrecv.enableIRIn(); // 初始化红外接收器

	//定义四路循迹红外传感器为输入接口
	pinMode(TrackSensorLeftPin1, INPUT);
	pinMode(TrackSensorLeftPin2, INPUT);
	pinMode(TrackSensorRightPin1, INPUT);
	pinMode(TrackSensorRightPin2, INPUT);

	//定义红外寻光管脚为输入模式
	pinMode(AvoidSensorLeft, INPUT);
	pinMode(AvoidSensorRight, INPUT);
	pinMode(FollowSensorLeft, INPUT);
	pinMode(FollowSensorRight, INPUT);
	pinMode(LdrSensorLeft, INPUT);
	pinMode(LdrSensorRight, INPUT);

	//初始化蜂鸣器IO为输出方式
	pinMode(buzzer, OUTPUT);
	digitalWrite(buzzer, HIGH);

	//初始化超声波引脚模式
	pinMode(EchoPin, INPUT);   //定义超声波输入脚
	pinMode(TrigPin, OUTPUT);  //定义超声波输出脚

	//定义灭火IO口为输出模式并初始化
	// pinMode(OutfirePin, OUTPUT);
	// digitalWrite(OutfirePin, HIGH);

	//初始化舵机引脚为输出模式
	// pinMode(ServoPin, OUTPUT);
	
	randomSeed(analogRead(0));   //设置一个随机数产生源模拟口 0
}

/**
* Function       servo_pulse
* @author        Danny
* @date          2017.07.26
* @brief         定义一个脉冲函数，用来模拟方式产生PWM值
*                时基脉冲为20ms,该脉冲高电平部分在0.5-2.5ms
*                控制0-180度
* @param[in1]    ServPin:舵机控制引脚
* @param[in2]    myangle:舵机转动指定的角度
* @param[out]    void
* @retval        void
* @par History   无
*/
void servo_pulse(int ServoPin, int myangle)
{
	int PulseWidth;                    //定义脉宽变量
	PulseWidth = (myangle * 11) + 500; //将角度转化为500-2480 的脉宽值
	digitalWrite(ServoPin, HIGH);      //将舵机接口电平置高
	delayMicroseconds(PulseWidth);     //延时脉宽值的微秒数
	digitalWrite(ServoPin, LOW);       //将舵机接口电平置低
	delay(20 - PulseWidth / 1000);     //延时周期内剩余时间
	return;
}

/**
* Function       Distance_test
* @author        Danny
* @date          2017.07.26
* @brief         超声波测距
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void Distance_test()
{
	digitalWrite(TrigPin, LOW);               //给触发脚低电平2μs
	delayMicroseconds(2);
	digitalWrite(TrigPin, HIGH);              //给触发脚高电平10μs，这里至少是10μs
	delayMicroseconds(10);
	digitalWrite(TrigPin, LOW);
	float Fdistance = pulseIn(EchoPin, HIGH); // 读取高电平时间(单位：微秒)
	Fdistance = Fdistance / 58;
	//  Serial.print("Distance:");            //输出距离（单位：厘米）
	//  Serial.print(Fdistance);              //显示距离
	//  Serial.println("cm");
	distance = Fdistance;
	return;
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
	pinMode(VoltagePin, INPUT);           //电压检测引脚和蜂鸣器引脚A5调整引脚模式来分时复用
	VoltageValue = analogRead(VoltagePin);//读取A0口值,换算为电压值

	//方法一:通过电路原理图和采集的A0口模拟值得到电压值
	//Serial.println(VoltageValue);
	//VoltageValue = (VoltageValue / 1023) * 5.02 * 1.75  ;
	//Voltage是端口A0采集到的ad值（0-1023），
	//1.75是（R14+R15）/R15的结果，其中R14=15K,R15=20K）。

/*查表记录打开*/  
//  float voltage = 0;
//  voltage = VoltageValue;
//  return voltage;


	//方法二:通过提前测量6.4-8.4v所对应的A0口模拟值,再通过查表法确定其值
	//       这种方法的误差小于0.1v
	int i = 0;
	float voltage = 0;
	if (VoltageValue > voltage_table[20][1])
	{
		voltage = 8.4;
		return voltage;
	}
	if (VoltageValue < voltage_table[0][1])
	{
		voltage = 6.4;
		return voltage;
	}
	for (i = 0; i < 20; i++)
	{
		if (VoltageValue >= voltage_table[i][1] && VoltageValue <= voltage_table[i + 1][1])
		{
			voltage =  voltage_table[i][0] + (VoltageValue - voltage_table[i][1]) * ((voltage_table[i + 1][0]
								 - voltage_table[i][0]) / (voltage_table[i + 1][1] - voltage_table[i][1]));
			return voltage;
		}
	}
	pinMode(VoltagePin, OUTPUT);
	digitalWrite(buzzer, HIGH);
	return 0;
}

/**
* Function       color_test
* @author        Danny
* @date          2017.07.26
* @brief         颜色识别测试
* @param[in]     void
* @param[out]    void
* @retval        iIntensity:灰度值
* @par History   无
*/
int color_test()
{
	int iIntensity;
	pinMode(LDR_pin, INPUT_PULLUP);          //灭火引脚和灰度传感器引脚复用A5口,需通过引脚设置进行分时复用
	iIntensity = analogRead(LDR_pin); //读取模拟口A5的值，存入Intensity变量
	//  Serial.print("Intensity = "); //串口输出"Intensity = "
	//  Serial.println(iIntensity);
	pinMode(LDR_pin, OUTPUT);
	return iIntensity;
}

/**
* Function       track_test
* @author        Danny
* @date          2017.07.26
* @brief         循迹模式引脚测试
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
//循迹和红外避障,寻光模块复用了A1,A2,A3,A4
//采集红外避障和寻光时要禁用循迹数据的采集
void track_test()
{
	//检测到黑线时循迹模块相应的指示灯亮，端口电平为LOW
	//未检测到黑线时循迹模块相应的指示灯灭，端口电平为HIGH
	TrackSensorLeftValue1 = digitalRead(TrackSensorLeftPin1);
	TrackSensorLeftValue2 = digitalRead(TrackSensorLeftPin2);
	TrackSensorRightValue1 = digitalRead(TrackSensorRightPin1);
	TrackSensorRightValue2 = digitalRead(TrackSensorRightPin2);

	(TrackSensorLeftValue1 == LOW) ? infrared_track_value[0] = '1' : infrared_track_value[0] = '0';
	(TrackSensorLeftValue2 == LOW) ? infrared_track_value[1] = '1' : infrared_track_value[1] = '0';
	(TrackSensorRightValue1 == LOW) ? infrared_track_value[2] = '1' : infrared_track_value[2] = '0';
	(TrackSensorRightValue2 == LOW) ? infrared_track_value[3] = '1' : infrared_track_value[3] = '0';
	//infrared_track_value = "0000";
	return;
}
/**
* Function       track_get_value
* @author        liusen
* @date          2017.07.26
* @brief         循迹模式引脚测试
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void track_get_value()
{
	TrackSensorLeftValue1  = digitalRead(TrackSensorLeftPin1);
	TrackSensorLeftValue2  = digitalRead(TrackSensorLeftPin2);
	TrackSensorRightValue1 = digitalRead(TrackSensorRightPin1);
	TrackSensorRightValue2 = digitalRead(TrackSensorRightPin2);
}

/**
* Function       infrared_avoid_test
* @author        Danny
* @date          2017.07.26
* @brief         避障红外引脚测试
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void infrared_avoid_test()
{
	//遇到障碍物,红外避障模块的指示灯亮,端口电平为LOW
	//未遇到障碍物,红外避障模块的指示灯灭,端口电平为HIGH
	LeftSensorValue  = digitalRead(AvoidSensorLeft);
	RightSensorValue = digitalRead(AvoidSensorRight);
	(LeftSensorValue == LOW) ? infrared_avoid_value[0] = '1' : infrared_avoid_value[0] = '0';
	(RightSensorValue == LOW) ? infrared_avoid_value[1] = '1' : infrared_avoid_value[1] = '0';
	return;
}

/**
* Function       follow_light_test
* @author        Danny
* @date          2017.07.26
* @brief         寻光引脚测试
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void follow_light_test()
{
	//遇到光线,寻光模块的指示灯灭,端口电平为HIGH
	//未遇光线,寻光模块的指示灯亮,端口电平为LOW
	LdrSersorRightValue = digitalRead(LdrSensorRight);
	LdrSersorLeftValue  = digitalRead(LdrSensorLeft);

	(LdrSersorLeftValue == LOW) ? LDR_value[0] = '0' : LDR_value[0] = '1';
	(LdrSersorRightValue == LOW) ? LDR_value[1] = '0' : LDR_value[1] = '1';
	return;
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
void setServoPulse(int num, double pulse)
{
	double pulselength;

	pulselength = 1000000; // 1,000,000 us per second
	pulselength /= 60;	 // 60 Hz
	pulselength /= 4096;   // 12 bits of resolution
	pulse *= 1000;
	pulse /= pulselength;
	pwm.setPWM(enServo[num - 1], 0, pulse);
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
/********************************************************************************************************/
/*模式2 巡线*/
/**
* Function       Tracking_Mode
* @author        Danny
* @date          2017.07.25
* @brief         巡线
* @param[in1]    void
* @param[out]    void
* @retval        void
* @par History   无
*/
void Tracking_Mode()
{
	//检测到黑线时循迹模块相应的指示灯亮，端口电平为LOW
	//未检测到黑线时循迹模块相应的指示灯灭，端口电平为HIGH
	TrackSensorLeftValue1  = digitalRead(TrackSensorLeftPin1);
	TrackSensorLeftValue2  = digitalRead(TrackSensorLeftPin2);
	TrackSensorRightValue1 = digitalRead(TrackSensorRightPin1);
	TrackSensorRightValue2 = digitalRead(TrackSensorRightPin2);
	
	//在巡线过程中发送巡线传感器效果
	time--;
	if (time == 0)
	{
		count--;
		time = 2000;
		if (count == 0)
		{
			printf("$4WD,CSB0,PV8,GS0,LF%d%d%d%d,HW00,GM00#", !TrackSensorLeftValue1, !TrackSensorLeftValue2, !TrackSensorRightValue1, !TrackSensorRightValue2);
			time = 2000;
			count = 1;
		}
	}


	//四路循迹引脚电平状态
	// 0 0 X 0
	// 1 0 X 0
	// 0 1 X 0
	//以上6种电平状态时小车原地右转，速度为250,延时80ms
	//处理右锐角和右直角的转动
	if ( (TrackSensorLeftValue1 == LOW || TrackSensorLeftValue2 == LOW) &&  TrackSensorRightValue2 == LOW)
	{
		CarSpeedControl = 150;
		spin_right(150);
		delay(80);
	}
	//四路循迹引脚电平状态
	// 0 X 0 0
	// 0 X 0 1
	// 0 X 1 0
	//处理左锐角和左直角的转动
	else if ( TrackSensorLeftValue1 == LOW && (TrackSensorRightValue1 == LOW ||  TrackSensorRightValue2 == LOW))
	{
		CarSpeedControl = 150;
		spin_left(150);
		delay(80);
	}
	// 0 X X X
	//最左边检测到
	else if ( TrackSensorLeftValue1 == LOW)
	{
		CarSpeedControl = 100;
		spin_left(150);
		//delay(10);
	}
	// X X X 0
	//最右边检测到
	else if ( TrackSensorRightValue2 == LOW )
	{
		CarSpeedControl = 100;
		spin_right(150);
		//delay(10);
	}
	//四路循迹引脚电平状态
	// X 0 1 X
	//处理左小弯
	else if ( TrackSensorLeftValue2 == LOW && TrackSensorRightValue1 == HIGH)
	{
		CarSpeedControl = 120;
		left(150);
	}
	//四路循迹引脚电平状态
	// X 1 0 X
	//处理右小弯
	else if (TrackSensorLeftValue2 == HIGH && TrackSensorRightValue1 == LOW)
	{
		CarSpeedControl = 120;
		right(150);
	}
	//四路循迹引脚电平状态
	// X 0 0 X
	//处理直线
	else if (TrackSensorLeftValue2 == LOW && TrackSensorRightValue1 == LOW)
	{
		CarSpeedControl = 150;
		run(150);
	}
}
/********************************************************************************************************/
/*模式3:超声波避障模式*/
/**
* Function       servo_color_carstate
* @author        Danny
* @date          2017.07.26
* @brief         舵机转向超声波测距避障行驶,led根据车的状态显示相应的颜色
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void servo_color_carstate()
{
	//定义舵机位置变量和小车前方,左侧,右侧距离
	int iServoPos = 0;
	int LeftDistance = 0;    //左方距离值变量LeftDistance
	int RightDistance = 0;   //右方距离值变量RightDistance
	int FrontDistance = 0;   //前方距离值变量FrontDistance
	setRGB(255, 0, 0);
	CarSpeedControl= 80;
	back(150);                //避免突然停止,刹不住车
	delay(80);
	brake();

	//舵机旋转到0度,即右侧,测距
	Servo180(1, 0);
	delay(500);
	Distance();         //测距
	RightDistance = distance;//所测的右侧距离赋给变量RightDistance

	//舵机旋转到180度,即左侧,测距
	Servo180(1, 180);
	delay(500);
	Distance();        //测距
	LeftDistance = distance;//所测的左侧距离赋给变量LeftDistance

	//舵机旋转到90度,即左侧,测距
	Servo180(1, 90);
	delay(500);
	Distance();
	FrontDistance = distance;//所测前侧距离付给变量FrontDistance

	if (LeftDistance < 10 && RightDistance < 10 && FrontDistance < 10  )
	{
		//亮品红色,掉头
		setRGB(255, 0, 0);
		CarSpeedControl= 120;
		spin_right(150);
		delay(560);
		brake();
	}
	else if ( LeftDistance >= RightDistance) //当发现左侧距离大于右侧，原地左转
	{
		//亮蓝色
		setRGB(0, 0, 255);
		CarSpeedControl= 120;
		spin_left(150);
		delay(280);
		brake();
	}
	else if (LeftDistance < RightDistance ) //当发现右侧距离大于左侧，原地右转
	{
		//亮品红色,向右转
		setRGB(255, 0, 0);
		CarSpeedControl= 120;
		spin_right(150);
		delay(280);
		brake();
	}
}
/**
* Function       bubble
* @author        Danny
* @date          2017.07.26
* @brief         超声波测五次的数据进行冒泡排序
* @param[in1]    a:超声波数组首地址
* @param[in2]    n:超声波数组大小
* @param[out]    void
* @retval        void
* @par History   无
*/
void bubble(unsigned long *a, int n)

{
	int i, j, temp;
	for (i = 0; i < n - 1; i++)
	{
		for (j = i + 1; j < n; j++)
		{
			if (a[i] > a[j])
			{
				temp = a[i];
				a[i] = a[j];
				a[j] = temp;
			}
		}
	}
}
/**
* Function       Distance
* @author        Danny
* @date          2017.07.26
* @brief         超声波测五次，去掉最大值,最小值,
*                取平均值,提高测试准确性
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void Distance()
{
	unsigned long ultrasonic[5] = {0};
	int num = 0;
	while (num < 5)
	{
		Distance_test();
		//过滤掉测试距离中出现的错误数据大于500,或者distance==0
		while (distance >= 500 || distance == 0)
		{
			brake();
			Distance_test();
		}
		ultrasonic[num] = distance;
		//printf("L%d:%d\r\n", num, (int)distance);
		num++;
		delay(10);
	}
	num = 0;
	bubble(ultrasonic, 5);
	distance = (ultrasonic[1] + ultrasonic[2] + ultrasonic[3]) / 3;
	return;
}
/**
* Function       Ultrasonic_avoidMode
* @author        Danny
* @date          2017.07.26
* @brief         超声波避障模式
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void Ultrasonic_avoidMode()
{
	Distance();        //测量前方距离
	 //printf("D:%d\r\n", (int)distance);
	if (distance > 30  )    //障碍物距离大于50时，开启左右红外辅助避障
	{
		//遇到障碍物,红外避障模块的指示灯亮,端口电平为LOW
		//未遇到障碍物,红外避障模块的指示灯灭,端口电平为HIGH
		LeftSensorValue = digitalRead(AvoidSensorLeft);
		RightSensorValue = digitalRead(AvoidSensorRight);

		if (LeftSensorValue == HIGH && RightSensorValue == LOW)
		{
			CarSpeedControl= 120;
			spin_left(150); //右边探测到有障碍物，有信号返回，原地向左转
			delay(200);
		}
		else if (RightSensorValue == HIGH && LeftSensorValue == LOW)
		{
			 CarSpeedControl= 120;
			 spin_right(150);//左边探测到有障碍物，有信号返回，原地向右转
			 delay(200);
		}
		else if (RightSensorValue == LOW && LeftSensorValue == LOW)
		{
			 CarSpeedControl= 120;
			spin_right(150);//当两侧均检测到障碍物时调用固定方向的避障(原地右转)
			delay(200);
		}
		//距离大于50时前进,亮绿灯
		CarSpeedControl= 120;
		run(150);
		setRGB(0, 255, 0);
	}
	else if ((distance >= 20 && distance <= 30))
	{
		//遇到障碍物,红外避障模块的指示灯亮,端口电平为LOW
		//未遇到障碍物,红外避障模块的指示灯灭,端口电平为HIGH
		LeftSensorValue = digitalRead(AvoidSensorLeft);
		RightSensorValue = digitalRead(AvoidSensorRight);

		if (LeftSensorValue == HIGH && RightSensorValue == LOW)
		{
			 CarSpeedControl= 120;
			spin_left(150); //右边探测到有障碍物，有信号返回，原地向左转
			delay(200);
		}
		else if (RightSensorValue == HIGH && LeftSensorValue == LOW)
		{
			 CarSpeedControl= 120;
			spin_right(150);//左边探测到有障碍物，有信号返回，原地向右转
			delay(200);
		}
		else if (RightSensorValue == LOW && LeftSensorValue == LOW)
		{
			 CarSpeedControl= 120;
			spin_right(150);//当两侧均检测到障碍物时调用固定方向的避障(原地右转)
			delay(200);
		}
		//距离在30-50之间时慢速前进
		 CarSpeedControl= 60;
		 run(150);
		
	}
	else if (  distance < 20  )//当距离小于30时调用舵机颜色控制程序
	{
		servo_color_carstate();
	}
}
/********************************************************************************************************/
/*模式:4  七彩颜色*/

void FindColor_Mode()
{
	for(; position < 180; position += 10){
		Servo180(1, position);
		setRGB( random(0,255), random(0,255), random(0,255));
		delay(100);
	}
	for(position = 180; position > 0; position -= 10){
		Servo180(1, position);
		setRGB( random(0,255), random(0,255), random(0,255));
		delay(100);
	}

	// Servo180(1, position);
	// setRGB( random(0,255), random(0,255), random(0,255));
	// position += 10;
	// delay(100);
	// if(position > 180)
	// {
	// 	position = 0;
	// }
}



/********************************************************************************************************/
/*模式5:  寻光模式*/
void LightSeeking_Mode()
{
	//遇到光线,寻光模块的指示灯灭,端口电平为HIGH
	//未遇光线,寻光模块的指示灯亮,端口电平为LOW
	LdrSersorRightValue = digitalRead(LdrSensorRight);
	LdrSersorLeftValue  = digitalRead(LdrSensorLeft);
	CarSpeedControl = 120;
	time--;
	if (time == 0)
	{
		count--;
		time = 2000;
		if (count == 0)
		{
			printf("$4WD,CSB120,PV8.3,GS000,LF0000,HW00,GM%d%d#", LdrSersorLeftValue, LdrSersorRightValue);
			time = 2000;
			count = 1;
		}
	}
	
	if (LdrSersorLeftValue == HIGH && LdrSersorRightValue == HIGH)
	{
		run(150);   //两侧均有光时信号为HIGH，光敏电阻指示灯灭,小车前进
	}
	else if (LdrSersorLeftValue == HIGH && LdrSersorRightValue == LOW)
	{
		left(150); //左边探测到有光，有信号返回，向左转
	}
	else if (LdrSersorRightValue == HIGH && LdrSersorLeftValue == LOW)
	{
		right(150);//右边探测到有光，有信号返回，向右转
	}
	else
	{
		brake();//均无光，停止
	}
}
/********************************************************************************************************/
/*模式6: 红外跟随模式*/

void Ir_flow_Mode()
{
	//遇到跟随物,红外跟随模块的指示灯亮,端口电平为LOW
	//未遇到跟随物,红外跟随模块的指示灯灭,端口电平为HIGH
	LeftSensorValue  = digitalRead(FollowSensorLeft);
	RightSensorValue = digitalRead(FollowSensorRight);
	CarSpeedControl = 120;
	time--;
	if (time == 0)
	{
		count--;
		time = 2000;
		if (count == 0)
		{
			printf("$4WD,CSB120,PV8.3,GS000,LF0000,HW%d%d,GM00", LeftSensorValue, RightSensorValue);
			time = 2000;
			count = 1;
		}
	}

	
	if (LeftSensorValue == LOW && RightSensorValue == LOW)
	{
		run(150);        //当两侧均检测到跟随物时调用前进函数
	}
	else if (LeftSensorValue == LOW && RightSensorValue == HIGH)
	{
		spin_left(150); //左边探测到有跟随物，有信号返回，原地向左转
	}
	else if (RightSensorValue == LOW && LeftSensorValue == HIGH)
	{
		spin_right(150);//右边探测到有跟随物，有信号返回，原地向右转
	}
	else
	{
		brake();     //当两侧均未检测到跟随物时停止
	}
}

/**
* Function       ModeBEEP
* @author        Danny
* @date          2017.08.17
* @brief         模式显示函数
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void ModeBEEP(int mode)
{
	pinMode(buzzer, OUTPUT);
	for (int i = 0; i < mode + 1; i++)
	{
		digitalWrite(buzzer, LOW); //鸣
		delay(100);
		digitalWrite(buzzer, HIGH); //不鸣
		delay(100);
	}
	delay(100);
	digitalWrite(buzzer, HIGH); //不鸣
}
/**
* Function       BeepOnOffMode
* @author        Danny
* @date          2017.08.17
* @brief         模式切换长鸣
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void BeepOnOffMode()
{
	pinMode(buzzer, OUTPUT);
	digitalWrite(buzzer, LOW);   //发声音
	delay(1000);                  //延时100ms
	digitalWrite(buzzer, HIGH);  //不发声音
}
/**
* Function       serial_data_parse
* @author        Danny
* @date          2017.07.25
* @brief         串口数据解析并指定相应的动作
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void serial_data_parse()
{

	/*解析模式切换*/
	//先判断是否是模式选择
	if (InputString.indexOf("MODE") > 0 && InputString.indexOf("4WD") > 0)
	{
		if (InputString[10] == '0') //停止模式
		{
			brake();
			g_CarState = enSTOP;
			g_modeSelect = 0;
			//position = 0;
			BeepOnOffMode();
		}
		else
		{
			switch (InputString[9])
			{
				case '0': g_modeSelect = 0; ModeBEEP(0); break;
				case '1': g_modeSelect = 1; ModeBEEP(1); break;
				case '2': g_modeSelect = 2; ModeBEEP(2); break;
				case '3': g_modeSelect = 3; ModeBEEP(3); break;
				case '4': g_modeSelect = 4; ModeBEEP(4); break;
				case '5': g_modeSelect = 5; ModeBEEP(5); break;
				case '6': g_modeSelect = 6; ModeBEEP(6); break;
				default: g_modeSelect = 0; break;
			}
			delay(1000);
			BeepOnOffMode();
		}
		InputString = "";                     //清空串口数据
		NewLineReceived = false;
		return;
	}

	//非apk模式则退出
	if (g_modeSelect != 0) //
	{
		InputString = "";                     //清空串口数据
		NewLineReceived = false;
		return;
	}

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
			int m_kp = m_skp.toInt();        //将找到的字符串变成整型
			//      Serial.print("PTZ:");
			//      Serial.println(m_kp);
			Servo180(1, 180 - m_kp);//转动到指定角度m_kp
			InputString = "";                     //清空串口数据
			NewLineReceived = false;
			return;
		}
	}
	//解析上位机发来的七彩探照灯指令并点亮相应的颜色
	//如:$4WD,CLR255,CLG0,CLB0# 七彩灯亮红色
	else if (InputString.indexOf("CLR") > 0)
	{
		int m_kp;
		int i = InputString.indexOf("CLR");
		int ii = InputString.indexOf(",", i);
		if (ii > i)
		{
			String m_skp = InputString.substring(i + 3, ii);
			int m_kp = m_skp.toInt();
			//      Serial.print("CLR:");
			//      Serial.println(m_kp);
			red =   m_kp;
		}
		i = InputString.indexOf("CLG");
		ii = InputString.indexOf(",", i);
		if (ii > i)
		{
			String m_skp = InputString.substring(i + 3, ii);
			int m_kp = m_skp.toInt();
			//      Serial.print("CLG:");
			//      Serial.println(m_kp);
			green =   m_kp;
		}
		i = InputString.indexOf("CLB");
		ii = InputString.indexOf("#", i);
		if (ii > i)
		{
			String m_skp = InputString.substring(i + 3, ii);
			int m_kp = m_skp.toInt();
			//      Serial.print("CLB:");
			//      Serial.println(m_kp);
			blue =  m_kp;
			setRGB(red, green, blue);//点亮相应颜色的灯
			InputString = "";               //清空串口数据
			NewLineReceived = false;
			return;
		}
	}
	//解析上位机发来的通用协议指令,并执行相应的动作
	//如:$1,0,0,0,0,0,0,0,0,0#    小车前进
	if (InputString.indexOf("4WD") == -1)
	{
		//小车原地左旋右旋判断
		if (InputString[3] == '1')      //小车原地左旋
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

		//小车鸣笛判断
		if (InputString[5] == '1')     //鸣笛
		{
			
		}

		//小车加减速判断
		if (InputString[7] == '1')     //加速，每次加50
		{
			CarSpeedControl += 50;
			if (CarSpeedControl > 255)
			{
				CarSpeedControl = 255;
			}
		}
		if (InputString[7] == '2')//减速，每次减50
		{
			CarSpeedControl -= 50;
			if (CarSpeedControl < 50)
			{
				CarSpeedControl = 100;
			}
		}

		//舵机左旋右旋判断
		if (InputString[9] == '1') //舵机旋转到180度
		{
			Servo180(1, 180);
		}
		if (InputString[9] == '2') //舵机旋转到0度
		{
			Servo180(1, 0);
		}

		//点灯判断
		switch (InputString[13])
		{
			case '1': setRGB(255, 255, 255); break;
			case '2': setRGB(255, 0, 0); break;
			case '3': setRGB(0, 255, 0); break;
			case '4': setRGB(0, 0, 255); break;
			case '5': setRGB(0, 255, 255); break;
			case '6': setRGB(255, 0, 255); break;
			case '7': setRGB(255, 255, 0); break;
			case '8': setRGB(0, 0, 0); break;
		}

		//灭火判断
		// if (InputString[15] == '1')  //灭火
		// {
		// 	pinMode(OutfirePin, OUTPUT);
		// 	digitalWrite(OutfirePin, LOW );
		// 	g_motor = true;
		// }
		// else if (InputString[15] == '0')  //灭火
		// {
		// 	pinMode(OutfirePin, OUTPUT);
		// 	digitalWrite(OutfirePin, HIGH );
		// 	g_motor = false;
		// }

		//舵机归为判断
		if (InputString[17] == '1') //舵机旋转到90度
		{
			Servo180(1, 90);
		}

		//小车的前进,后退,左转,右转,停止动作
		if (g_CarState != enTLEFT && g_CarState != enTRIGHT)
		{
			switch (InputString[1])
			{
				case run_car:   g_CarState = enRUN;  break;
				case back_car:  g_CarState = enBACK;  break;
				case left_car:  g_CarState = enLEFT;  break;
				case right_car: g_CarState = enRIGHT;  break;
				case stop_car:  g_CarState = enSTOP;  break;
				default: g_CarState = enSTOP; break;
			}
		}

		InputString = "";         //清空串口数据
		NewLineReceived = false;

		//根据小车状态做相应的动作
		switch (g_CarState)
		{
			case enSTOP: brake(); break;
			case enRUN: run(150); break;
			case enLEFT: left(150); break;
			case enRIGHT: right(150); break;
			case enBACK: back(150); break;
			case enTLEFT: spin_left(150); break;
			case enTRIGHT: spin_right(150); break;
			default: brake(); break;
		}
	}
}

/**
* Function       serial_data_postback
* @author        Danny
* @date          2017.07.25
* @brief         将采集的传感器数据通过串口传输给上位机显示
* @param[in]     void
* @retval        void
* @par History   无
*/
void serial_data_postback()
{
	//小车超声波传感器采集的信息发给上位机显示
	//打包格式如:
	//    超声波 电压  灰度  巡线  红外避障 寻光
	//$4WD,CSB120,PV8.3,GS214,LF1011,HW11,GM11#
	//超声波
	Distance_test();
	ReturnTemp = "$4WD,CSB" ;
	ReturnTemp.concat(distance);
	//电压
	ReturnTemp += ",PV";
	//voltage_test();
	ReturnTemp.concat( voltage_test());
	//灰度
	ReturnTemp += ",GS";
	ReturnTemp.concat(color_test());
	//巡线
	ReturnTemp += ",LF";
	track_test();
	ReturnTemp.concat(infrared_track_value);
	//红外避障
	ReturnTemp += ",HW";
	infrared_avoid_test();
	ReturnTemp.concat(infrared_avoid_value);
	//寻光
	ReturnTemp += ",GM";
	follow_light_test();
	ReturnTemp.concat(LDR_value);
	ReturnTemp += "#";
	Serial.print(ReturnTemp);
	return;
}

/**
* Function       serialEvent
* @author        Danny
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
			InputString += (char) IncomingByte;
		}
		if (IncomingByte == '#')
		{
			NewLineReceived = true;
			StartBit = false;
		}
	}
}

/**
* Function       loop
* @author        Danny
* @date          2017.07.25
* @brief         对串口发送过来的数据解析，并执行相应的指令
* @param[in]     void
* @retval        void
* @par History   无
*/
void loop()
{
	if (NewLineReceived)
	{
		serial_data_parse();  //调用串口解析函数
	}

	// 切换不同功能模式, 功能模式显示
	switch (g_modeSelect)
	{
		case 1: break; //暂时保留
		case 2: Tracking_Mode(); break; //巡线模式
		case 3: Ultrasonic_avoidMode();  break;  //超声波避障模式
		case 4: FindColor_Mode(); break;  //七彩颜色识别模式
		case 5: LightSeeking_Mode(); break;  //寻光模式
		case 6: Ir_flow_Mode(); break;  //跟随模式
	}

	//让小车串口平均每秒发送采集的数据给手机蓝牙apk
	//避免串口打印数据速度过快,造成apk无法正常运行
	if (g_modeSelect == 0 && g_motor == false)
	{
		time--;
		if (time == 0)
		{
			count--;
			time = 20000;
			if (count == 0)
			{
				
				serial_data_postback();
				time = 20000;
				count = 10;
			}
		}
	}
}
