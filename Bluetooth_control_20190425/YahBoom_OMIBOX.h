/*******************
YahBoom_OMIBOX.h
Auth：liusen
Date: 2019.02.11
*******************/

#ifndef _YAHBOOM_OMIBOX_H__
#define _YAHBOOM_OMIBOX_H__

//导入Arduino核心头文件
#include <Arduino.h>  
#include <Adafruit_NeoPixel.h>         //编程灯
#include <Wire.h>                     //IIC
#include <Adafruit_PWMServoDriver.h>  //PCA9685
#include <LedControl.h>               //点阵库
#include <IRremote.h>                 //红外
//#include "./IRremote.h"
#define L_Motor_PIN1 1
#define L_Motor_PIN2 2
#define R_Motor_PIN1 15
#define R_Motor_PIN2 14

/*定义点阵的引脚 */
#define DIN 9
#define CS  10
#define CLK 11
#define PIN 4 
//int RECV_PIN = 2; // 红外一体化接收头连接到Arduino 2号引脚
typedef struct 
{
  uint8_t Character[1];
  uint8_t data[6];
}LED_Matrix_Font_6x8_TypeDef;

const LED_Matrix_Font_6x8_TypeDef Character_font_6x8[] PROGMEM =
{
    ' ',  0x00,0x00,0x00,0x00,0x00,0x00,

    '0',  0x00,0x7C,0x82,0x82,0x7C,0x00,
    '1',  0x00,0x42,0xFE,0x02,0x00,0x00,
    '2',  0x00,0x46,0x8A,0x92,0x62,0x00,
    '3',  0x00,0x44,0x92,0x92,0x6C,0x00,
    '4',  0x00,0x1C,0x64,0xFE,0x04,0x00,
    '5',  0x00,0xF2,0x92,0x92,0x8C,0x00,
    '6',  0x00,0x7C,0x92,0x92,0x4C,0x00,
    '7',  0x00,0xC0,0x8E,0x90,0xE0,0x00,
    '8',  0x00,0x6C,0x92,0x92,0x6C,0x00,
    '9',  0x00,0x64,0x92,0x92,0x7C,0x00,

    'a',  0x00,0x04,0x2A,0x2A,0x1E,0x00,
    'b',  0x00,0xFE,0x12,0x12,0x0C,0x00,
    'c',  0x00,0x0C,0x12,0x12,0x12,0x00,
    'd',  0x00,0x0C,0x12,0x12,0xFE,0x00,
    'e',  0x00,0x1C,0x2A,0x2A,0x18,0x00,
    'f',  0x00,0x10,0x3E,0x50,0x50,0x00,
    'g',  0x00,0x08,0x15,0x15,0x1E,0x00,
    'h',  0x00,0xFE,0x10,0x1E,0x00,0x00,
    'i',  0x00,0x00,0x2E,0x00,0x00,0x00,
    'j',  0x00,0x02,0x01,0x2E,0x00,0x00,
    'k',  0x00,0xFE,0x08,0x14,0x12,0x00,
    'l',  0x00,0x00,0xFE,0x02,0x00,0x00,
    'm',  0x00,0x1E,0x10,0x0E,0x10,0x0E,
    'n',  0x00,0x1E,0x10,0x10,0x0E,0x00,
    'o',  0x00,0x0C,0x12,0x12,0x0C,0x00,
    'p',  0x00,0x1F,0x12,0x12,0x0C,0x00,
    'q',  0x00,0x0C,0x12,0x12,0x1F,0x00,
    'r',  0x00,0x1E,0x08,0x10,0x10,0x00,
    's',  0x00,0x12,0x29,0x25,0x12,0x00,
    't',  0x00,0x10,0x3E,0x12,0x00,0x00,
    'u',  0x00,0x1C,0x02,0x02,0x1E,0x00,
    'v',  0x18,0x04,0x02,0x04,0x18,0x00,
    'w',  0x18,0x06,0x1C,0x06,0x18,0x00,
    'x',  0x00,0x12,0x0C,0x0C,0x12,0x00,
    'y',  0x00,0x18,0x05,0x05,0x1E,0x00,
    'z',  0x00,0x12,0x16,0x1A,0x12,0x00,

    'A',  0x00,0x7E,0x88,0x88,0x7E,0x00,
    'B',  0x00,0xFE,0x92,0x92,0x6C,0x00,
    'C',  0x00,0x7C,0x82,0x82,0x44,0x00,
    'D',  0x00,0xFE,0x82,0x82,0x7C,0x00,
    'E',  0x00,0xFE,0x92,0x92,0x82,0x00,
    'F',  0x00,0xFE,0x90,0x90,0x80,0x00,
    'G',  0x00,0x7C,0x82,0x92,0x5C,0x00,
    'H',  0x00,0xFE,0x10,0x10,0xFE,0x00,
    'I',  0x00,0x82,0xFE,0x82,0x00,0x00,
    'J',  0x00,0x0C,0x02,0x02,0xFC,0x00,
    'K',  0x00,0xFE,0x10,0x28,0xC6,0x00,
    'L',  0x00,0xFE,0x02,0x02,0x02,0x00,
    'M',  0x00,0xFE,0x40,0x30,0x40,0xFE,
    'N',  0x00,0xFE,0x40,0x30,0x08,0xFE,//0x08,0xFE,
    'O',  0x00,0x7C,0x82,0x82,0x82,0x7C,//0x82,0x7C,
    'P',  0x00,0xFE,0x90,0x90,0x60,0x00,
    'Q',  0x00,0x7C,0x82,0x8A,0x84,0x7A,
    'R',  0x00,0xFE,0x98,0x94,0x62,0x00,
    'S',  0x00,0x64,0x92,0x92,0x4C,0x00,
    'T',  0x00,0x80,0xFE,0x80,0x80,0x00,
    'U',  0x00,0xFC,0x02,0x02,0xFC,0x00,
    'V',  0x00,0xF0,0x0C,0x02,0x0C,0xF0,
    'W',  0x00,0xFE,0x04,0x38,0x04,0xFE,
    'X',  0x00,0xC6,0x38,0x38,0xC6,0x00,
    'Y',  0xC0,0x20,0x1E,0x20,0xC0,0x00,
    'Z',  0x00,0x86,0x9A,0xB2,0xC2,0x00,
    ',',  0x00,0x01,0x0e,0x0c,0x00,0x00,
    '.',  0x00,0x00,0x06,0x06,0x00,0x00,
    '%',  0x72,0x54,0x78,0x1e,0x2a,0x4e,
    '!',  0x00,0x00,0x7a,0x00,0x00,0x00,
    '?',  0x00,0x20,0x4a,0x30,0x00,0x00,
    '-',  0x00,0x10,0x10,0x10,0x10,0x00,
    '+',  0x08,0x08,0x3e,0x08,0x08,0x00,
    '/',  0x00,0x02,0x0c,0x30,0x40,0x00,
    '*',  0x22,0x14,0x08,0x14,0x22,0x00,
    ':',  0x00,0x00,0x14,0x00,0x00,0x00,
    '"',  0x00,0xC0,0x00,0xC0,0x00,0x00,
    '#',  0x28,0xFE,0x28,0xFE,0x28,0x00,
    '(',  0x00,0x00,0x7C,0x82,0x00,0x00,
    ')',  0x00,0x00,0x82,0x7C,0x00,0x00,
    ';',  0x00,0x02,0x24,0x00,0x00,0x00,
    '~',  0x00,0x40,0x80,0x40,0x80,0x00,
    ';',  0x00,0x02,0x24,0x00,0x00,0x00,
    '=',  0x00,0x28,0x28,0x28,0x28,0x00,
    '|',  0x00,0x00,0xFE,0x00,0x00,0x00,
    '>',  0x00,0x82,0x44,0x28,0x10,0x00,
    '<',  0x00,0x10,0x28,0x44,0x82,0x00,  
    '@',  0x00,0x00,0x00,0x00,0x00,0x00,  // End mark of Character_font_6x8
};

typedef struct 
{
  uint8_t data[3];
}LED_Matrix_Clock_Number_Font_3x8_TypeDef;

const LED_Matrix_Clock_Number_Font_3x8_TypeDef Clock_Number_font_3x8[] PROGMEM =
{
  0x7C,0x44,0x7C,  //0
  0x24,0x7C,0x04,  //1
  0x5C,0x54,0x74,  //2
  0x54,0x54,0x7C,  //3
  0x70,0x10,0x7C,  //4
  0x74,0x54,0x5C,  //5
  0x7C,0x54,0x5C,  //6
  0x40,0x40,0x7C,  //7
  0x7C,0x54,0x7C,  //8
  0x74,0x54,0x7C,  //9
  0x00,0x04,0x00,  //.
  0x10,0x10,0x10,  //-
  0x00,0x00,0x00,  //
};

/*RGB_LED*/
class YahBoom_OMIBOX_RGBLED
{
     private:
          byte pinName;
          Adafruit_NeoPixel strip;

     public:
          
          YahBoom_OMIBOX_RGBLED(byte max_led, byte pin);   //构造函数         
          ~YahBoom_OMIBOX_RGBLED();  //析构函数

          /*LED控制*/
          void RGBLED_Init();

          void RGBLED_Show_Color(byte Num, uint8_t v_Red, uint8_t v_Green, uint8_t v_Blue);
          void RGBLED_Show_All(uint8_t v_Red, uint8_t v_Green, uint8_t v_Blue);
          void RGBLED_setBrightness(uint8_t b);
          void LED_OFF(byte PinNum);
          void LED_Breath(byte PinNum);


};

/*点阵显示*/
class YahBoom_OMIBOX_Matrix
{
     public:
          LedControl lc = LedControl(DIN,CLK,CS,PIN);
          int16_t i16_Str_Display_pos;
          int8_t i8_Str_Display_Y_Position;
          int16_t i16_Number_of_Character_of_Str;
          char i8_Str_Display_Buffer[20];
          char code[16];
         
     public:
          
          YahBoom_OMIBOX_Matrix();   //构造函数         
          ~YahBoom_OMIBOX_Matrix();  //析构函数
          
          void YahBoom_OMIBOX_Matrix_Init();

          void YahBoom_OMIBOX_Matrix_Show(byte character[]);
          void YahBoom_OMIBOX_Matrix_ShowIcon(byte *character);
          void YahBoom_OMIBOX_Matrix_ShowNum(int pos, float num);
          void YahBoom_OMIBOX_Matrix_ShowStr(int pos, const char *str);
          void showStr();
          
          void picture( uint8_t *pr1,uint8_t t,int time0);
          void roll(int time0,uint8_t t, uint8_t *pr1);
          void LED_ShowStrroll( char *str,uint8_t a,int time0);
          void showStrroll(int time0,uint8_t a);
          void LED_Display(uint8_t x,uint8_t y,uint8_t a);
          void Move_picture(uint8_t x,uint16_t time);
          void Static_picture(uint8_t x);
};

/*蜂鸣器*/
class YahBoom_OMIBOX_Buzzer
{
     private:
          byte pin;

     public:
          
          YahBoom_OMIBOX_Buzzer(byte pinname);   //构造函数         
          ~YahBoom_OMIBOX_Buzzer();  //析构函数

          /*蜂鸣器控制*/
          void setBuzzer_Tone(uint16_t frequency, uint32_t duration);//void setBuzzer_Tone(uint16_t frequency, uint32_t duration);
          void music_Play(uint8_t  v_song, uint8_t index);
          void birthday(int j);
          void childhood(int j);
          void fenshuajiang(int j);
          void shishangzhiyoumamahao(int j);
          void liangzhilaohu(int j);
};


/*按钮*/
class YahBoom_OMIBOX_Button
{
         
     public:
          
          YahBoom_OMIBOX_Button();   //构造函数         
          ~YahBoom_OMIBOX_Button();  //析构函数

          /*按键读取*/
          bool Get_Button_State(byte PinNum);

};

/*巡线*/
class YahBoom_OMIBOX_TrackLine
{
     private:
          byte pinName1;
          byte pinName2;
          
     public:
          
          YahBoom_OMIBOX_TrackLine(byte pin1, byte pin2);   //构造函数         
          ~YahBoom_OMIBOX_TrackLine();  //析构函数
          void YahBoom_OMIBOX_TrackLine_Init();
          
          /*左巡线读取*/
          bool Senser_State_L(uint8_t v_state);
          /*右巡线读取*/
          bool Senser_State_R(uint8_t v_state);

};

/*红外避障*/
class YahBoom_OMIBOX_IRAvoid
{
     private:
          byte pinName1;
          byte pinName2;
          
     public:
          
          YahBoom_OMIBOX_IRAvoid(byte pin1, byte pin2);   //构造函数         
          ~YahBoom_OMIBOX_IRAvoid();  //析构函数
          void YahBoom_OMIBOX_IRAvoid_Init();

          /*左避障读取*/
          bool Senser_State_L();       
          /*右避障读取*/
          bool Senser_State_R();

};
/*电压检测*/
class YahBoom_OMIBOX_Voltage
{
     private:
          byte pinName;
          
     public:
          
          YahBoom_OMIBOX_Voltage(byte pin);   //构造函数         
          ~YahBoom_OMIBOX_Voltage();  //析构函数
          void YahBoom_OMIBOX_Voltage_Init();
          float get_Voltage();  //获取电压0-5.0

};
/*光敏电阻*/
class YahBoom_OMIBOX_LightSensor
{
     private:
          byte pinName;
          
     public:
          
          YahBoom_OMIBOX_LightSensor(byte pin);   //构造函数         
          ~YahBoom_OMIBOX_LightSensor();  //析构函数
          void YahBoom_OMIBOX_LightSensor_Init();
          int get_LightSensor();  //获取光敏AD 0-1023

};

/*声音传感器*/
class YahBoom_OMIBOX_VoiceSensor
{
     private:
          byte pinName;
          
     public:
          
          YahBoom_OMIBOX_VoiceSensor(byte pin);   //构造函数         
          ~YahBoom_OMIBOX_VoiceSensor();  //析构函数
          void YahBoom_OMIBOX_VoiceSensor_Init();
          int get_VoiceSensor();  //获取光敏AD 0-1023

};

/*扩展舵机*/
class YahBoom_OMIBOX_Servo
{
     private:
          void SetServoPos(int num, int value);
          
     public:
          
          YahBoom_OMIBOX_Servo();   //构造函数         
          ~YahBoom_OMIBOX_Servo();  //析构函数
          void YahBoom_OMIBOX_Servo_Init();
       
};

/*小车运动舵机*/
class YahBoom_OMIBOX_MotorServo
{
     private:
          void SetServoPos(int num, int value);
          Adafruit_PWMServoDriver pwm;
     public:
 
          YahBoom_OMIBOX_MotorServo();   //构造函数         
          ~YahBoom_OMIBOX_MotorServo();  //析构函数
          void YahBoom_OMIBOX_MotorServo_Init(); //初始化函数
          
          void SetServoAngle(int num, int value); //外部调用

          /*小车运行状态*/
          void Run(int speed = 255, int delay_ms = 0);
          void Back(int speed = 255, int delay_ms = 0);
          void Left(int speed = 255, int delay_ms = 0);
          void Right(int speed = 255, int delay_ms = 0);
          void SpinLeft(int speed = 255, int delay_ms = 0);
          void SpinRight(int speed = 255, int delay_ms = 0);
          void Stop(int speed = 0, int delay_ms = 0); 
          
          void MotorSpeed(int left, int right, int delay_ms);
          void LeftMotor(int direction = 0, int speed = 255);
          void RightMotor(int direction = 0, int speed = 255);

};
/*颜色传感器*/
#define RGB_2           0x00
#define ADDR            0x53
#define W_ADDR_L        0x66
#define R_ADDR_L        0x67
//ADDR->High
#define RGB_1           0x01
#define W_ADDR_H        0x98
#define R_ADDR_H        0x99

//Register Address
#define ControlReg_Addr 0x00
#define RGBCON_Addr     0x04

#define ID_Addr         0x06

#define CDATAL_Addr     0x12
#define CDATAH_Addr     0x13

#define RDATAL_Addr     0x10
#define RDATAM_Addr     0x11
#define RDATAH_Addr     0x12

#define GDATAL_Addr     0x0D
#define GDATAM_Addr     0x0E
#define GDATAH_Addr     0x0F

#define BDATAL_Addr     0x13
#define BDATAM_Addr     0x14
#define BDATAH_Addr     0x15
/*  Confi Arg  */
//Control Register 
#define RST             0x00

/*小车颜色传感器*/
class YahBoom_OMIBOX_Color
{
     private:
          
          unsigned int Red;
          unsigned int Green;
          unsigned int Blue;
          void RGB_Config(void);
     public:
          
          YahBoom_OMIBOX_Color();   //构造函数         
          ~YahBoom_OMIBOX_Color();  //析构函数
          void YahBoom_OMIBOX_Color_Init(); //初始化函数
          bool YahBoom_OMIBOX_Identify_Color(byte color);
          void get_ColorSensor();
          int ReturnRedValue();
          int ReturnGreenValue();
          int ReturnBlueValue();
          void PowerSwith(int onoff);

};
/*红外接收*/
class YahBoom_OMIBOX_IRrecv
{
    
     public:            
          unsigned long last;
          decode_results results; // 用于存储编码结果的对象
          int last_receive_number;
          int receive_number;
          IRrecv irrecv = IRrecv(2);
          
     public:
          
          YahBoom_OMIBOX_IRrecv();   //构造函数         
          ~YahBoom_OMIBOX_IRrecv();  //析构函数
          
          void YahBoom_OMIBOX_IRrecv_Init();
          int IR_Deal();
          void init();
};
/*硬件初始化*/
class YahBoom_OMIBOX
{     
     public:
          
          YahBoom_OMIBOX();   //构造函数         
          ~YahBoom_OMIBOX();  //析构函数

          void Init();
};
#endif
