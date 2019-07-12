/**
* @par Copyright (C): 2010-2019, Shenzhen Yahboom Tech
* @file         arduino_4WD_IR_control
* @author       wusicaijuan
* @version      V1.0
* @date         2019.07.01
* @brief        红外控制arduino4WD小车
* @details
* @par History  见如下说明
*
*/
#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>
#include <Adafruit_NeoPixel.h> //库文件
#include <IRremote.h>       //红外
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
#define SERVOMIN 150 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX 600 // this is the 'maximum' pulse length count (out of 4096)

#define PIN 6  //定义RGB灯的引脚
#define MAX_LED 1 //小车一共有1个RGB灯
Adafruit_NeoPixel strip = Adafruit_NeoPixel(MAX_LED, PIN, NEO_RGB + NEO_KHZ800);

#define OLED_RESET 11
Adafruit_SSD1306 display(OLED_RESET);

#define LOGO16_GLCD_HEIGHT 32
#define LOGO16_GLCD_WIDTH 128

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

#define run_car '1'      //按键前
#define back_car '2'     //按键后
#define left_car '3'     //按键左
#define right_car '4'   //按键右
#define spin_left_car '5'  //按键左旋
#define spin_right_car '6' //按键右旋
#define stop_car '7'     //按键停
#define servoL 'L'       //舵机左转
#define servoR 'R'       //舵机右转
#define servoS 'S'       //舵机停止

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

const typedef enum {
  enServoL = 1,
  enServoR,
  enServoS
} enServoState;

const char enServo[] = {0, 1, 2, 3};

const int RECV_PIN = A0; //定义红外接收器的引脚为A0
IRrecv irrecv(RECV_PIN);
decode_results results;
// int on = 0; //标志位
unsigned long last = millis();

int flag = 0; //开机欢迎界面标志位
int Servo_LR = 90;

/*电压引脚及其变量设置*/
const int VoltagePin = A2;
double VoltageValue = 0; //电压值

const int flag_time = 2000; //时间标记点间隔2s
int newtime = 0;      //记录系统当前时间
int lasttime = 0;     //记录上一次系统时间标记点

/*小车初始速度控制*/
static int CarSpeedControl = 150;
static int g_carstate = enSTOP;   //1前2后3左4右5左旋6右旋7停止
static int g_ServoState = enServoS; //1左转2右转3停止

const int buzzer = 10; //设置控制蜂鸣器的引脚

/**
* Function       setup
* @author        wusicaijuan
* @date          2019.07.09
* @brief         初始化配置
* @param[in]     void
* @retval        void
* @par History   无
*/
void setup()
{
  // put your setup code here, to run once:
  strip.begin();
  strip.show();
  PCB_RGB_OFF();

  pwm.begin();
  pwm.setPWMFreq(60); // Analog servos run at ~60 Hz updates
  Clear_All_PWM();
  //舵机归位
  Servo180(1, 90);
  Servo180(2, Servo_LR);

  irrecv.enableIRIn();       // 初始化红外解码
  pinMode(RECV_PIN, INPUT_PULLUP); //将2号管脚设置为输入并且内部上拉模式

  // PCB_LED();
  breathing_light(255, 40, 5);

  pinMode(VoltagePin, INPUT);
  pinMode(buzzer, OUTPUT);  //设置数字IO脚模式，OUTPUT为输出
  digitalWrite(buzzer, HIGH); // 持续给触发脚低电

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // initialize with the I2C addr 0x3C (for the 128x32)
  // init done
  display.display(); // show splashscreen
  delay(2000);
  // display.clearDisplay(); // clears the screen and buffer

  welcome();
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (irrecv.decode(&results))
  {
    if (flag == 0)
    {
      flag = 1;
      return;
    }
    //Serial.println(results.value, HEX);

    //红外遥控按键码
    //FF00FF  开关
    //FF807F  前进
    //FF40BF  灯
    //FF20DF  左转
    //FFA05F  喇叭
    //FF609F  右转
    //FF10EF  左旋
    //FF906F  后退
    //FF50AF  右旋
    //FF30CF  加号
    //FFB04F  0
    //FF708F  减号
    //FF08F7  1
    //FF8877  2
    //FF48B7  3
    //FF28D7  4
    //FFA857  5
    //FF6897  6
    //FF18E7  7
    //FF9867  8
    //FF58A7  9

    switch (results.value)
    {
    //开关
    case 0X00FF00FF:
      RGB_OFF();
      break;
    //灯
    case 0x00FF40BF:
      setRGB(255, 255, 255);
      break;
    //1
    case 0x00FF08F7:

      break;
    //2
    case 0x00FF8877:

      break;
    //3
    case 0x00FF48B7:

      break;
    //4
    case 0x00FF28D7:

      break;
    //加号
    case 0x00FF30CF:
      CarSpeedControl += 50;
      if (CarSpeedControl > 150)
      {
        CarSpeedControl = 150;
      }
      break;
    //0
    case 0x00FFB04F:

      break;
    //减号
    case 0x00FF708F:
      CarSpeedControl -= 50;
      if (CarSpeedControl < 50)
      {
        CarSpeedControl = 50;
      }
      break;
    //5
    case 0x00FFA857:

      break;
    //喇叭
    case 0x00FFA05F:
      whistle();
      break; //鸣笛

    //7
    case 0x00ff18e7:
      g_ServoState = enServoL;
      break;
    //8
    case 0x00ff9867:

      break;
    //9
    case 0x00ff58a7:
      g_ServoState = enServoR;
      break;
    //6
    case 0x00FF6897:

      break;
    //前进
    case 0x00FF807F:
      g_carstate = enRUN;
      break; //前
    //左转(左移)
    case 0x00FF20DF:
      g_carstate = enLEFT;
      break; //左
    //右转(右移)
    case 0x00FF609F:
      g_carstate = enRIGHT;
      break; //右
    //后退
    case 0x00FF906F:
      g_carstate = enBACK;
      break; //后
    //左旋
    case 0x00FF10EF:
      g_carstate = enSPINLEFT;
      break; //左旋
    //右旋
    case 0x00FF50AF:
      g_carstate = enSPINRIGHT;
      break; //右旋
    default:
      break; //保持原来状态
    }
    last = millis();
    irrecv.resume(); // 接收下一个编码
  }
  else if (millis() - last > 120)
  {
    g_carstate = enSTOP;
    g_ServoState = enServoS;
    last = millis();
  }
  switch (g_carstate)
  {
  //case enSTOP: brake(); break;
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
  switch (g_ServoState)
  {
  case enServoL:
    Servo_LR++;
    if (Servo_LR > 180)
    {
      Servo_LR = 180;
    }
    Servo180(1, Servo_LR);
    delay(5);
    break;
  case enServoR:
    Servo_LR--;
    if (Servo_LR < 0)
    {
      Servo_LR = 0;
    }
    Servo180(1, Servo_LR);
    delay(5);
    break;
  default:
    break;
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
    }
  }
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
  long pwmvalue = us * 4096 / 20000;   // 50hz: 20,000 us
  pwm.setPWM(enServo[num - 1], 0, pwmvalue);
}

/**
* Function       setRGB(R,G,B)
* @author        wusicaijuan
* @date          2019.06.26
* @brief         设置外接RGB灯
* @param[in1]  R
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
* Function       RGB_OFF()
* @author        wusicaijuan
* @date          2019.06.26
* @brief         设置外接RGB灯
* @param[in1]  R
* @param[in2]    G
* @param[in3]    B
* @param[out]    void
* @retval        void
* @par History   无
*/
void RGB_OFF()
{
  pwm.setPWM(6, 0, 0);
  pwm.setPWM(5, 0, 0);
  pwm.setPWM(4, 0, 0);
}

/**
* Function       PCB_RGB(R,G,B)
* @author        wusicaijuan
* @date          2019.06.26
* @brief         设置板载RGB灯
* @param[in1]  R
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
* @param[in1]  void
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
* @param[in1]  void
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
* @param[in1]  brightness
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

/**
* Function       whistle
* @author        wusicaijuan
* @date          2019.07.09
* @brief         鸣笛
* @param[in1]    void
* @retval        void
* @par History   无
*/
void whistle() //鸣笛
{
  for (int i = 0; i < 50; i++)
  {
    digitalWrite(buzzer, HIGH); //发声音
    delay(3);         //延时
    digitalWrite(buzzer, LOW);  //不发声音
    delay(1);         //延时
  }
}
