/*******************
YahBoom_OMIBOX.cpp
Auth：liusen
Date: 2019.02.11
*******************/

#include"./YahBoom_OMIBOX.h"
#include "avr/pgmspace.h"


/* #define INPUT 0x0 #define OUTPUT 0x1 #define INPUT_PULLUP 0x2 */ 
/*****************************************************************/ /*LED控制*/ 


int Num_ofword=0;
char flag=0;
//int receive_number;

//爱心
const PROGMEM unsigned char Heart[16] = {
 0xc,0x60,0x1e,0xf0,0x1f,0xf0,0x1f,0xf0
 ,0xf,0xe0,0x7,0xc0,0x3,0x80,0x1,0x0};


//三角形
const PROGMEM unsigned char Triangle [16] = {
	0x4,0x0,0x6,0x0,0x5,0x0,0x4,0x80
	,0x4,0x40,0x4,0x20,0x4,0x10,0x7,0xf8};
//房子
const PROGMEM unsigned char House [16] ={
	0x1,0x0,0x3,0x80,0x7,0xc0,0xf,0xe0
	,0x1f,0xf0,0x4,0x40,0x4,0x40,0x7,0xc0};
//船
const PROGMEM unsigned char Boat [16] = {
	0x0,0x10,0x0,0x30,0x0,0x70,0x0,0x10
	,0x0,0x10,0xf,0xfc,0x7,0xf8,0x3,0xf0};
//音符
const PROGMEM unsigned char Music [16] ={
	0x1,0x80,0x1,0xc0,0x1,0xe0,0x1,0xf0
	,0x1,0x80,0x1,0x80,0xf,0x80,0xf,0x80};
//雨伞
const PROGMEM unsigned char Umbrella [16] = {
	0x1,0x0,0x3,0x80,0x7,0xc0,0xf,0xe0
	,0x1,0x0,0x1,0x0,0x7,0x0,0x7,0x0};
//对
const PROGMEM unsigned char Right [16]  = {
	0x0,0xc,0x0,0x18,0x0,0x30,0x0,0x60
	,0x18,0xc0,0xd,0x80,0x7,0x0,0x2,0x0};
//错
const PROGMEM unsigned char Error [16] {
	0x8,0x10,0x4,0x20,0x2,0x40,0x1,0x80
	,0x1,0x80,0x2,0x40,0x4,0x20,0x8,0x10};
//长方形
const PROGMEM unsigned char Rectangle [16] = {
	0x0,0x0,0x1f,0xf8,0x10,0x8,0x10,0x8
	,0x10,0x8,0x10,0x8,0x1f,0xf8,0x0,0x0};
	

//眨眼睛
const PROGMEM uint8_t nictation0[16]={0x38,0x1c,0x7c,0x3e,0xfe,0x7f,0xfe,0x7f,0xfe,0x7f,0xfe,0x7f,0x44,0x22,0x38,0x1c};
const PROGMEM uint8_t nictation1[16]={0x38,0x1c,0x7c,0x3e,0xfe,0x7f,0xba,0x5d,0x82,0x41,0x82,0x41,0x44,0x22,0x38,0x1c};
//冒桃心
const PROGMEM uint8_t heart0[16]={0x0,0x0,0x6c,0x36,0xfe,0x7f,0xfe,0x7f,0xfe,0x7f,0x7c,0x3e,0x38,0x1c,0x10,0x8};
const PROGMEM uint8_t heart1[16]={0x0,0x0,0x0,0x0,0x28,0x14,0x7c,0x3e,0x7c,0x3e,0x38,0x1c,0x10,0x8,0x0,0x0};
//调皮鬼
const PROGMEM uint8_t mischievous_ghost1[16]={0x4,0x2,0x7c,0x3e,0x0,0x0,0x7c,0x3e,0x8e,0x47,0x8e,0x47,0x7c,0x3e,0x0,0x0};
const PROGMEM uint8_t mischievous_ghost2[16]={0x40,0x20,0x7c,0x3e,0x0,0x0,0x7c,0x3e,0xe2,0x71,0xe2,0x71,0x7c,0x3e,0x0,0x0};
const PROGMEM uint8_t mischievous_ghost3[16]={0x4,0x20,0x7c,0x3e,0x0,0x0,0x7c,0x3e,0x8e,0x71,0x8e,0x71,0x7c,0x3e,0x0,0x0};
const PROGMEM uint8_t mischievous_ghost4[16]={0x40,0x2,0x7c,0x3e,0x0,0x0,0x7c,0x3e,0xe2,0x47,0xe2,0x47,0x7c,0x3e,0x0,0x0};

//疑问Doubt
const PROGMEM uint8_t doubt1[16]={0x3c,0x3c,0x3e,0x3e,0x6,0x6,0xc,0xc,0x18,0x18,0x0,0x0,0x18,0x18,0x18,0x18};
const PROGMEM uint8_t doubt2[16]={0x38,0x1c,0x7c,0x3e,0xfe,0x7f,0xba,0x5d,0x82,0x41,0x82,0x41,0x44,0x22,0x38,0x1c};
const PROGMEM uint8_t doubt3[16]={0x38,0x1c,0x74,0x3a,0xf2,0x79,0xe2,0x71,0x82,0x41,0x82,0x41,0x44,0x22,0x38,0x1c};
const PROGMEM uint8_t doubt4[16]={0x38,0x1c,0x44,0x22,0xe2,0x71,0xf2,0x79,0xf2,0x79,0xe2,0x71,0x44,0x22,0x38,0x1c};
const PROGMEM uint8_t doubt5[16]={0x38,0x1c,0x44,0x22,0x82,0x41,0x82,0x41,0xe2,0x71,0xf2,0x79,0x74,0x3a,0x38,0x1c};
const PROGMEM uint8_t doubt6[16]={0x38,0x1c,0x44,0x22,0x82,0x41,0x82,0x41,0xba,0x5d,0xfe,0x7f,0x7c,0x3e,0x38,0x1c};
const PROGMEM uint8_t doubt7[16]={0x38,0x1c,0x44,0x22,0x82,0x41,0x82,0x41,0x8e,0x47,0x9e,0x4f,0x5c,0x2e,0x38,0x1c};
const PROGMEM uint8_t doubt8[16]={0x38,0x1c,0x44,0x22,0x8e,0x47,0x9e,0x4f,0x9e,0x4f,0x8e,0x47,0x44,0x22,0x38,0x1c};
const PROGMEM uint8_t doubt9[16]={0x38,0x1c,0x5c,0x2e,0x9e,0x4f,0x8e,0x47,0x82,0x41,0x82,0x41,0x44,0x22,0x38,0x1c};
const PROGMEM uint8_t doubt10[16]={0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
const PROGMEM uint8_t doubt11[16]={0x38,0x1c,0x44,0x22,0x44,0x22,0x44,0x22,0x28,0x14,0x10,0x8,0x38,0x1c,0x38,0x1c};
//滑稽
const PROGMEM uint8_t funny1[16]={0x0,0x0,0x30,0xc,0xcc,0x33,0x30,0xc,0x78,0x1e,0x48,0x12,0x48,0x12,0x30,0xc};
const PROGMEM uint8_t funny2[16]={0x30,0xc,0xcc,0x33,0x0,0x0,0x30,0xc,0x78,0x1e,0x78,0x1e,0x48,0x12,0x30,0xc};


//爱哭鬼
const PROGMEM uint8_t crying_wolf1[16]={0x1e,0x78,0x20,0x4,0xdc,0x3b,0x3a,0x5c,0x32,0x4c,0x22,0x44,0x22,0x44,0x1c,0x38};
const PROGMEM uint8_t crying_wolf2[16]={0x0,0x0,0x3c,0x7c,0xdc,0x3b,0x3a,0x5c,0x32,0x4c,0x32,0x4c,0x22,0x44,0x1c,0x38};





unsigned int val_red = 0;
unsigned int val_green = 0;
unsigned int val_blue = 0;
	
enum enMusic
 {
  enBirthday=1,
  Childhood=2,
  Fenshuajiang=3,
  Shishangzhiyoumamahao=4,
  Liangzhilaohu=5
  
 };

#define G5 392//小字组G
#define A6 440//小字组A
#define B7 494//小字组B
#define c1 525//小字一组c即中央c
#define d2 587//小字一组d
#define e3 659//小字一组e
#define f4 698//小字一组f
#define g5 784//小字一组g
#define a6 880//小字一组a
#define b7 988//小字一组b
#define C1 1047//小字二组C
#define D2 1175//小字二组D
#define E3 1319//小字二组E
#define F4 1397//小字二组F
#define GG5 1568//小字二组G
#define AA6 1769//小字二组A

const PROGMEM  int BIrthday[25][2]
{
// {392,2},{880,2},{784,2},{525,2},{494,4},
// {392,2},{440,2},{392,2},{587,2},{525,4},
// {392,2},{784,2},{659,2},{525,2},{494,2},{440,2},
// {698,2},{659,2},{525,2},{587,2},{525,2}
{G5,2},{A6,2},{G5,2},{c1,2},{B7,4},
 {G5,2},{A6,2},{G5,2},{d2,2},{c1,4},
 {G5,2},{g5,2},{e3,2},{c1,2},{B7,2},{A6,2},
 {f4,2},{e3,2},{c1,2},{d2,2},{c1,2}
};

const PROGMEM  int CHildhood[80][2]
{
	{e3,1},{g5,1},{g5,2},{e3,1},{a6,1},{a6,1},{b7,1},{a6,1},{a6,1},{a6,1},{g5,1},{C1,2},{C1,1},{C1,1},{C1,1},{a6,1},{C1,1},{a6,1},{g5,8},
	{0,2},{e3,1},{g5,1},{g5,2},{e3,1},{a6,1},{a6,1},{b7,1},{a6,1},{a6,1},{a6,1},{g5,1},{C1,2},{C1,1},{C1,1},{C1,1},{a6,1},{a6,1},{C1,1},{D2,8},
	{GG5,1},{GG5,2},{GG5,2},{GG5,1},{E3,1},{D2,1},{C1,1},{C1,2},{a6,2},{C1,1},{a6,1},{C1,1},{D2,1},{D2,1},{D2,1},{D2,1},{C1,1},{E3,1},{D2,1},{D2,8},	
	{E3,1},{E3,2},{E3,1},{E3,1},{D2,1},{D2,2},{C1,1},{C1,2},{C1,1},{D2,1},{C1,1},{a6,1},{g5,1},{g5,1},{g5,2},{g5,1},{a6,1},{g5,1},{D2,1},{E3,1},{C1,8}
};

const PROGMEM  int FEnshuajiang[60][2]
{
	{g5,1},{e3,1},{g5,1},{e3,1},{g5,1},{e3,1},{c1,2},
	{d2,1},{f4,1},{e3,1},{d2,1},{g5,4},
	{g5,1},{e3,1},{g5,1},{e3,1},{g5,1},{e3,1},{c1,2},
	{d2,1},{f4,1},{e3,1},{d2,1},{c1,4},
	{d2,1},{d2,1},{f4,1},{f4,1},{e3,1},{c1,1},{g5,2},
	{d2,1},{f4,1},{e3,1},{d2,1},{g5,4},
	{g5,1},{e3,1},{g5,1},{e3,1},{g5,1},{e3,1},{c1,2},
	{d2,1},{f4,1},{e3,1},{d2,1},{c1,2}
};


const PROGMEM  int SHishangzhiyoumamahao[34][2]
{
	{a6,6},{g5,2},{e3,4},{g5,4},{C1,4},{a6,2},{g5,2},{a6,8},
	{e3,4},{g5,2},{a6,2},{g5,4},{e3,2},{d2,2},{c1,2},{A6,2},{g5,2},{e3,2},{d2,8},	
	{d2,6},{e3,2},{g5,4},{g5,2},{a6,2},{e3,4},{d2,4},{c1,8},
	{g5,6},{e3,2},{d2,2},{c1,2},{A6,2},{c1,2},{G5,8}
};

const PROGMEM  int LIangzhilaohu[32][2]
{
	{c1,2},{d2,2},{e3,2},{c1,2},{c1,2},{d2,2},{e3,2},{c1,2},
	{e3,2},{f4,2},{g5,4},{e3,2},{f4,2},{g5,4},
	{g5,1},{a6,1},{g5,1},{f4,1},{e3,2},{c1,2},{g5,1},{a6,1},{g5,1},{f4,1},{e3,2},{c1,2},
	{d2,2},{G5,2},{c1,4},{d2,2},{G5,2},{c1,4}
};



YahBoom_OMIBOX_RGBLED::YahBoom_OMIBOX_RGBLED(byte max_led, byte pin)   //构造函数 
{ 
	strip = Adafruit_NeoPixel( max_led, pin, NEO_RGB + NEO_KHZ800 );
	
}

YahBoom_OMIBOX_RGBLED::~YahBoom_OMIBOX_RGBLED()
{
	
} 
void YahBoom_OMIBOX_RGBLED::RGBLED_Init()
{
	strip.begin();           
  strip.show(); 
}

void YahBoom_OMIBOX_RGBLED::RGBLED_Show_Color(byte Num, uint8_t v_Red, uint8_t v_Green, uint8_t v_Blue)
{
		//if()
		uint32_t color = strip.Color(v_Green, v_Red, v_Blue);
		strip.setPixelColor(Num, color);  //设置i的值控制点亮灯的序号，color控制灯的颜，这里是让0号灯为红色
    strip.show();
}

void YahBoom_OMIBOX_RGBLED::RGBLED_Show_All(uint8_t v_Red, uint8_t v_Green, uint8_t v_Blue)
{
		uint8_t i = 0;
		uint32_t color = strip.Color(v_Green, v_Red, v_Blue);
		for(i = 0; i < 8; i++)
		{
			strip.setPixelColor(i, color);  //设置i的值控制点亮灯的序号，color控制灯的颜，这里是让0号灯为红色
		}
    strip.show();
}

void YahBoom_OMIBOX_RGBLED::RGBLED_setBrightness(uint8_t b)
{
		strip.setBrightness(b);
    strip.show();
}

void YahBoom_OMIBOX_RGBLED::LED_OFF(byte PinNum)
{
		digitalWrite(PinNum, LOW);
}

void YahBoom_OMIBOX_RGBLED::LED_Breath(byte PinNum)
{
		for (int i = 0; i < 255; i++)
  	{
	    analogWrite(PinNum, i);
	    delay(7);
  	}
	  delay(10);
	  for (int i = 255; i > 0; i--)
	  {
	    analogWrite(PinNum, i);
	    delay(7);
	  }		
}

/*****************************************************************/


/*点阵面板 */
/******************************************************************/

YahBoom_OMIBOX_Matrix::YahBoom_OMIBOX_Matrix()
{
	memset(code, 0x00, 16);
}

YahBoom_OMIBOX_Matrix::~YahBoom_OMIBOX_Matrix()
{
	lc.clearDisplay(0);         //清除显示
	lc.clearDisplay(1);
}

void YahBoom_OMIBOX_Matrix::YahBoom_OMIBOX_Matrix_Init()
{
	   lc.shutdown(0,false);       //启动时，MAX72XX处于省电模式
     lc.setIntensity(0,1);       //将亮度设置为最大值
     lc.shutdown(1,false);       //启动时，MAX72XX处于省电模式
     lc.setIntensity(1,1);       //将亮度设置为最大值
     lc.clearDisplay(0);         //清除显示
     lc.clearDisplay(1);         //清除显示
}

void YahBoom_OMIBOX_Matrix::YahBoom_OMIBOX_Matrix_Show(byte character [])
{
   int i = 0;
   memset(code, 0x00, 16);
    for(i = 0; i < 8; i++)                
    {
        for(int j = 0; j < 2; j++)
        {
          lc.setRow(j==0?0:1, i, character[i*2 + j]);
        }
    } 
	  
}

void YahBoom_OMIBOX_Matrix::YahBoom_OMIBOX_Matrix_ShowIcon(byte *character)
{
   int i = 0;
   memset(code, 0x00, 16);
   for(i = 0; i < 8; i++)                
    {
        for(int j = 0; j < 2; j++)
        {
          lc.setRow(j==0?0:1, i, pgm_read_byte_near(character + i*2 + j));
        }
         
    } 
}

//显示数字
void YahBoom_OMIBOX_Matrix::YahBoom_OMIBOX_Matrix_ShowNum(int pos, float num)
{
	pos=3-pos;
    //uint8_t u8_Display_Buffer[16];
    uint8_t u8_Display_Buffer[16];
Posotion_1:
  uint8_t buf[4] = { 0x0c, 0x0c, 0x0c, 0x0c};
  uint8_t tempBuf[4];
  uint8_t b = 0, i =0;
  uint8_t bit_num = 0;
  uint8_t int_num = 0;
  uint8_t isNeg = 0;
  double number = num;
  if (number >= 9999.5)
  {
    buf[0] = 9;
    buf[1] = 9;
    buf[2] = 9;
    buf[3] = 9;
  }
  else if(number <= -999.5)
  {
    buf[0] = 0x0b;
    buf[1] = 9;
    buf[2] = 9;
    buf[3] = 9;  
  }
  else
  {
    // Handle negative numbers
    if (number < 0.0)
    {
      number = -number;
      isNeg = 1;
    }
    // Round correctly so that print(1.999, 2) prints as "2.00"
    double rounding = 0.5;
    for (uint8_t i = 0; i < pos; ++i)
    {
      rounding /= 10.0;
    }
    number += rounding;

    // Extract the integer part of the number and print it
    uint16_t int_part = (uint16_t)number;
    double remainder = number - (double)int_part;
    do
    {
      uint16_t m = int_part;
      int_part /= 10;
      int8_t c = m - 10 * int_part;
      tempBuf[int_num] = c;
      int_num++;
    }
    while (int_part);

    bit_num = isNeg + int_num + pos;

    if (bit_num > 4)
    {
      bit_num = 4;
      pos = 4 - (isNeg + int_num);
      goto Posotion_1;
    }
    b = 4 - bit_num;
    if (isNeg)
    {
      buf[b++] = 0x0b; // '-' display minus sign
    }
    for (uint8_t i = int_num; i > 0; i--)
    {
      buf[b++] = tempBuf[i - 1];
    }
    // Print the decimal point, but only if there are digits beyond
    if (pos > 0)
    {
      if((b == 3) && (int16_t(remainder*10) == 0))
      {
        buf[3] = 0x0c;
    	}
      else if((b == 2) && (int16_t(remainder*100) == 0))
      {
        buf[2] = 0x0c;
        buf[3] = 0x0c;
    }
    else if((b == 1) && (int16_t(remainder*1000) == 0))
      {
        buf[1] = 0x0c;
        buf[2] = 0x0c;
        buf[3] = 0x0c;
    }
    else
    {
        buf[b++] = 0x0a;  // display '.'
        // Extract digits from the remainder one at a time
        while (pos-- > 0)
        {
          remainder *= 10.0;
          int16_t toPrint = int16_t(remainder);
          buf[b++] = toPrint;
          remainder -= toPrint;
        }
    }
    }
  }

  u8_Display_Buffer[0]  = pgm_read_byte(&Clock_Number_font_3x8[buf[0]].data[0]);
  u8_Display_Buffer[1]  = pgm_read_byte(&Clock_Number_font_3x8[buf[0]].data[1]);
  u8_Display_Buffer[2]  = pgm_read_byte(&Clock_Number_font_3x8[buf[0]].data[2]);
  
  u8_Display_Buffer[3]  = 0x00;
  
  u8_Display_Buffer[4]  = pgm_read_byte(&Clock_Number_font_3x8[buf[1]].data[0]);
  u8_Display_Buffer[5]  = pgm_read_byte(&Clock_Number_font_3x8[buf[1]].data[1]);
  u8_Display_Buffer[6]  = pgm_read_byte(&Clock_Number_font_3x8[buf[1]].data[2]);
  
  u8_Display_Buffer[7]  = 0x00;
  
  u8_Display_Buffer[8]  = pgm_read_byte(&Clock_Number_font_3x8[buf[2]].data[0]);
  u8_Display_Buffer[9]  = pgm_read_byte(&Clock_Number_font_3x8[buf[2]].data[1]);
  u8_Display_Buffer[10]  = pgm_read_byte(&Clock_Number_font_3x8[buf[2]].data[2]);
 
  u8_Display_Buffer[11]  = 0x00;
  
  u8_Display_Buffer[12]  = pgm_read_byte(&Clock_Number_font_3x8[buf[3]].data[0]);
  u8_Display_Buffer[13]  = pgm_read_byte(&Clock_Number_font_3x8[buf[3]].data[1]);
  u8_Display_Buffer[14]  = pgm_read_byte(&Clock_Number_font_3x8[buf[3]].data[2]);

  u8_Display_Buffer[15]  = 0x00;

  for(i=0; i<8; i++)                
  {
       lc.setColumn(0, i, u8_Display_Buffer[i]); 
  }  
  
  for(i=0; i<8; i++)                
  {
       lc.setColumn(1, i, u8_Display_Buffer[8+i]); 
  } 
      
}
    
//显示字符串
void YahBoom_OMIBOX_Matrix::YahBoom_OMIBOX_Matrix_ShowStr(int pos, const char *str)
{
    int8_t Y_position = 7;
    for(i16_Number_of_Character_of_Str = 0; str[i16_Number_of_Character_of_Str] != '\0'; i16_Number_of_Character_of_Str++)
    {
        if(i16_Number_of_Character_of_Str < 20 - 1)
        {
            i8_Str_Display_Buffer[i16_Number_of_Character_of_Str] = str[i16_Number_of_Character_of_Str];
        }
        else
        {
            break;
        }
    }
    i8_Str_Display_Buffer[i16_Number_of_Character_of_Str] = '\0';


    if(pos < -(i16_Number_of_Character_of_Str * 6))
    {
        pos = -(i16_Number_of_Character_of_Str * 6);
    }
    else if(pos > 16)
    {
        pos = 16;
    }
    i16_Str_Display_pos = pos;
    if(Y_position < -1)
    {
        Y_position = -1;
    }
    else if(Y_position >15)
    {
        Y_position = 15;
    }
    i8_Str_Display_Y_Position = Y_position;
    showStr();
}
void YahBoom_OMIBOX_Matrix::showStr()
{
    uint8_t u8_Display_Buffer[16];
    uint8_t display_buffer_label = 0, i = 0;
    if(i16_Str_Display_pos > 0)
    {
        for(display_buffer_label = 0; display_buffer_label < i16_Str_Display_pos && display_buffer_label < 16; display_buffer_label++)
        {
            u8_Display_Buffer[display_buffer_label] = 0x00;
        }
        if(display_buffer_label < 16)
        {
            uint8_t num;

            for(uint8_t k=0;display_buffer_label < 16 && k < i16_Number_of_Character_of_Str;k++)
            {
                for(num=0; pgm_read_byte(&Character_font_6x8[num].Character[0]) != '@'; num++)
                {
                    if(i8_Str_Display_Buffer[k] == pgm_read_byte(&Character_font_6x8[num].Character[0]))
                    {
                        for(uint8_t j=0;j<6;j++)
                        {
                            u8_Display_Buffer[display_buffer_label] = pgm_read_byte(&Character_font_6x8[num].data[j]);
                            display_buffer_label++;

                            if(display_buffer_label >= 16)
                            {
                                break;
                            }
                        }
                        break;
                    }
                }
                if(pgm_read_byte(&Character_font_6x8[num].Character[0]) == '@')
                {
                    i8_Str_Display_Buffer[k] = ' ';
                    k--;
                }
            }

            if(display_buffer_label < 16)
            {
                for(display_buffer_label = display_buffer_label; display_buffer_label < 16; display_buffer_label++)
                {
                    u8_Display_Buffer[display_buffer_label] = 0x00;
                }
            }
        }
    }

    else if(i16_Str_Display_pos <= 0)
    {
        if(i16_Str_Display_pos == -(i16_Number_of_Character_of_Str * 6))
        {
            for(; display_buffer_label < 16; display_buffer_label++)
            {
                u8_Display_Buffer[display_buffer_label] = 0x00;
            }
        }
        else
        {
            int8_t j = (-i16_Str_Display_pos) % 6;
            uint8_t num;

            i16_Str_Display_pos = -i16_Str_Display_pos;

            for(int16_t k=i16_Str_Display_pos/6; display_buffer_label < 16 && k < i16_Number_of_Character_of_Str;k++)
            {
                for(num=0; pgm_read_byte(&Character_font_6x8[num].Character[0]) != '@'; num++)
                {
                    if(i8_Str_Display_Buffer[k] == pgm_read_byte(&Character_font_6x8[num].Character[0]))
                    {
                        for(;j<6;j++)
                        {
                            u8_Display_Buffer[display_buffer_label] = pgm_read_byte(&Character_font_6x8[num].data[j]);
                            display_buffer_label++;

                            if(display_buffer_label >= 16)
                            {
                                break;
                            }
                        }
                        j=0;
                        break;
                    }
                }

                if(pgm_read_byte(&Character_font_6x8[num].Character[0]) == '@')
                {
                    i8_Str_Display_Buffer[k] = ' ';
                    k--;
                }
            }
            if(display_buffer_label < 16)
            {
                for(; display_buffer_label < 16; display_buffer_label++)
                {
                    u8_Display_Buffer[display_buffer_label] = 0x00;
                }
            }
            i16_Str_Display_pos = -i16_Str_Display_pos;
        }
    }
    if(7 - i8_Str_Display_Y_Position >= 0)
    {
        for(uint8_t k = 0; k < 16; k++)
        {
            u8_Display_Buffer[k] = u8_Display_Buffer[k] << (7 - i8_Str_Display_Y_Position);
        }
    }
    else
    {
        for(uint8_t k = 0; k < 16; k++)
        {
            u8_Display_Buffer[k] = u8_Display_Buffer[k] >> (i8_Str_Display_Y_Position - 7);
        }
    }
    for(i=0; i<8; i++)                
    {
         lc.setColumn(0, i, u8_Display_Buffer[i]); 
    }  
    
    for(i=0; i<8; i++)                
    {
         lc.setColumn(1, i, u8_Display_Buffer[8+i]); 
    } 
}



//显示字符串可以滚动
void YahBoom_OMIBOX_Matrix::showStrroll(int time0,uint8_t a)
{
    uint8_t u8_Display_Buffer[Num_ofword],an[80],bn[80];
    uint8_t display_buffer_label = 0, i = 0;
    if(i16_Str_Display_pos > 0)
    {
        for(display_buffer_label = 0; display_buffer_label < i16_Str_Display_pos && display_buffer_label < Num_ofword; display_buffer_label++)
        {
            u8_Display_Buffer[display_buffer_label] = 0x00;
        }
        if(display_buffer_label < Num_ofword)
        {
            uint8_t num;

            for(uint8_t k=0;display_buffer_label < Num_ofword && k < i16_Number_of_Character_of_Str;k++)
            {
                for(num=0; pgm_read_byte(&Character_font_6x8[num].Character[0]) != '@'; num++)
                {
                    if(i8_Str_Display_Buffer[k] == pgm_read_byte(&Character_font_6x8[num].Character[0]))
                    {
                        for(uint8_t j=0;j<6;j++)
                        {
                            u8_Display_Buffer[display_buffer_label] = pgm_read_byte(&Character_font_6x8[num].data[j]);
                            display_buffer_label++;

                            if(display_buffer_label >= Num_ofword)
                            {
                                break;
                            }
                        }
                        break;
                    }
                }
                if(pgm_read_byte(&Character_font_6x8[num].Character[0]) == '@')
                {
                    i8_Str_Display_Buffer[k] = ' ';
                    k--;
                }
            }

            if(display_buffer_label < Num_ofword)
            {
                for(display_buffer_label = display_buffer_label; display_buffer_label < Num_ofword; display_buffer_label++)
                {
                    u8_Display_Buffer[display_buffer_label] = 0x00;
                }
            }
        }
    }

    else if(i16_Str_Display_pos <= 0)
    {
        if(i16_Str_Display_pos == -(i16_Number_of_Character_of_Str * 6))
        {
            for(; display_buffer_label < Num_ofword; display_buffer_label++)
            {
                u8_Display_Buffer[display_buffer_label] = 0x00;
            }
        }
        else
        {
            int8_t j = (-i16_Str_Display_pos) % 6;
            uint8_t num;

            i16_Str_Display_pos = -i16_Str_Display_pos;

            for(int16_t k=i16_Str_Display_pos/6; display_buffer_label < Num_ofword && k < i16_Number_of_Character_of_Str;k++)
            {
                for(num=0; pgm_read_byte(&Character_font_6x8[num].Character[0]) != '@'; num++)
                {
                    if(i8_Str_Display_Buffer[k] == pgm_read_byte(&Character_font_6x8[num].Character[0]))
                    {
                        for(;j<6;j++)
                        {
                            u8_Display_Buffer[display_buffer_label] = pgm_read_byte(&Character_font_6x8[num].data[j]);
                            display_buffer_label++;

                            if(display_buffer_label >= Num_ofword)
                            {
                                break;
                            }
                        }
                        j=0;
                        break;
                    }
                }

                if(pgm_read_byte(&Character_font_6x8[num].Character[0]) == '@')
                {
                    i8_Str_Display_Buffer[k] = ' ';
                    k--;
                }
            }
            if(display_buffer_label < Num_ofword)
            {
                for(; display_buffer_label < Num_ofword; display_buffer_label++)
                {
                    u8_Display_Buffer[display_buffer_label] = 0x00;
                }
            }
            i16_Str_Display_pos = -i16_Str_Display_pos;
        }
    }
    if(7 - i8_Str_Display_Y_Position >= 0)
    {
        for(uint8_t k = 0; k < Num_ofword; k++)
        {
            u8_Display_Buffer[k] = u8_Display_Buffer[k] << (7 - i8_Str_Display_Y_Position);
        }
    }
    else
    {
        for(uint8_t k = 0; k < Num_ofword; k++)
        {
            u8_Display_Buffer[k] = u8_Display_Buffer[k] >> (i8_Str_Display_Y_Position - 7);
        }
    }
    for(i=0; i<8; i++)                
    {
         an[i]= u8_Display_Buffer[i];
    }  
     for(i=0; i<8; i++)                
    {
         bn[i]= u8_Display_Buffer[i+8];
    } 
    roll(time0,a,u8_Display_Buffer);
}

void YahBoom_OMIBOX_Matrix::LED_ShowStrroll( char *str,uint8_t a,int time0)
{
    int pos=1,words,j;
    static char flag=0; 
    int8_t Y_position = 7;//
        while(str[i16_Number_of_Character_of_Str] != '\0')
        {
          i16_Number_of_Character_of_Str++;
         }   
         if(a==2&&flag==0)
         {
            flag=1;
            for(j=0;j<i16_Number_of_Character_of_Str/2;j++)
               {
                  words=str[j];
                   str[j]=str[i16_Number_of_Character_of_Str-j-1];
                   str[i16_Number_of_Character_of_Str-j-1]=words;
               }
          }
        Num_ofword=i16_Number_of_Character_of_Str*6+1;
        if(Num_ofword<8)
          Num_ofword=8;
       // Serial.println(Num_ofword,DEC);
      for(i16_Number_of_Character_of_Str = 0; str[i16_Number_of_Character_of_Str] != '\0'; i16_Number_of_Character_of_Str++)
        {
            if(i16_Number_of_Character_of_Str < Num_ofword+2 - 1)
            {
                i8_Str_Display_Buffer[i16_Number_of_Character_of_Str] = str[i16_Number_of_Character_of_Str];
           }
           else
            {
                break;
            }
        }
       
        i8_Str_Display_Buffer[i16_Number_of_Character_of_Str] = '\0';
    
    
        if(pos < -(i16_Number_of_Character_of_Str * 6))
        {
            pos = -(i16_Number_of_Character_of_Str * 6);
        }
        else if(pos > Num_ofword)
        {
            pos = Num_ofword;
        }
        i16_Str_Display_pos = pos;
        if(Y_position < -1)
        {
            Y_position = -1;
        }
        else if(Y_position >Num_ofword-1)
        {
            Y_position = Num_ofword-1;
        }
        i8_Str_Display_Y_Position = Y_position;
        showStrroll(time0,a);

}


 void YahBoom_OMIBOX_Matrix::roll(int time0,uint8_t t, uint8_t *pr1)
{
  int i,j,m;
  int a[Num_ofword],c[16];
  for(j=0;j<Num_ofword;j++)
  {
    a[j]=*(pr1+j);
   // Serial.println(a[j],DEC);
  }
  if(Num_ofword<16)
  {
    for(i=0;i<Num_ofword;i++)
    {
     c[i]=a[i]; 
     }
     for(i=Num_ofword;i<16;i++)
     c[i]=0;
   }
     if(Num_ofword>15)
     {
        for(i=0;i<Num_ofword+1;i++)
        {
         if(t==1)
         {
            for(j=0;j<8;j++)
             {
                lc.setColumn(0, j, a[j]); 
                lc.setColumn(1, j, a[j+8]); 
             }
             m=a[0];
             for(j=0;j<Num_ofword;j++)
             {
              a[j]=a[j+1]; 
             }
             a[Num_ofword-1]=0;//a[15]赋值为m的时候字体一直循环，尾接头，当a[15]赋值为0的时候知道所有的字体都显示完了再循环
             delay(time0);
         }
         else if(t==2)
         {
            for(j=0;j<8;j++)
             {
                lc.setColumn(0, j, a[Num_ofword-17+j]); 
                lc.setColumn(1, j, a[Num_ofword-9+j]); 
             }
              m=a[Num_ofword-1];
             for(j=Num_ofword;j>0;j--)
             {
              a[j]=a[j-1]; 
             }
             a[0]=0;//a[15]赋值为m的时候字体一直循环，尾接头，当a[15]赋值为0的时候知道所有的字体都显示完了再循环
             delay(time0);
         }
         else if(t==0)
             {
                   for(j=0;j<8;j++)
                     {
                        lc.setColumn(0, j, a[j]); 
                        lc.setColumn(1, j, a[j+8]); 
                     }
                     delay(time0);         
             }
           
        }
     }
    else
     {
             
                for(i=0;i<16;i++)
                {
                   if(t==1)
                   {
                     for(j=0;j<8;j++)
                       {
                          lc.setColumn(0, j, c[j]); 
                          lc.setColumn(1, j, c[j+8]); 
                       }
                     m=a[0];
                     for(j=0;j<15;j++)
                     {
                      c[j]=c[j+1]; 
                     }
                     c[15]=0;//a[15]赋值为m的时候字体一直循环，尾接头，当a[15]赋值为0的时候知道所有的字体都显示完了再循环
                     delay(time0);
                   }
                   if(t==2)
                   {
                      for(j=0;j<8;j++)
                         {
                            lc.setColumn(0, j, c[j]); 
                            lc.setColumn(1, j, c[j+8]); 
                         }
                       m=a[15];
                       for(j=15;j>0;j--)
                       {
                        c[j]=c[j-1]; 
                       }
                       c[0]=0;//a[15]赋值为m的时候字体一直循环，尾接头，当a[15]赋值为0的时候知道所有的字体都显示完了再循环
                       delay(time0);
                   }  
                  if(t==0)
                   {
                         for(j=0;j<8;j++)
                           {
                              lc.setColumn(0, j, c[j]); 
                              lc.setColumn(1, j, c[j+8]); 
                           }
                           delay(time0);         
                   }
                 }
                
       
     }
    
} 
void YahBoom_OMIBOX_Matrix::picture( uint8_t *pr1,uint8_t t,int time0)
{
    int i,j,m;
    int a[16],c[16];
    Num_ofword=16;
    for(j=0;j<Num_ofword;j++)
    {
      a[j]=*(pr1+j);
    }
    if(Num_ofword<16)
    {
        for(i=0;i<Num_ofword;i++)
        {
           c[i]=a[i]; 
        }
       for(i=Num_ofword;i<16;i++)
       c[i]=0;
     }
     else if(Num_ofword>15)
         {
            for(i=0;i<Num_ofword-1;i++)
            {
                 if(t==0)
                 {
                       for(j=0;j<8;j++)
                         {
                            lc.setColumn(0, j, a[j]); 
                            lc.setColumn(1, j, a[j+8]); 
                         }
                         delay(time0); 
                 }
                 else if(t==1)
                 {
                    for(j=0;j<8;j++)
                     {
                        lc.setColumn(0, j, a[j]); 
                        lc.setColumn(1, j, a[j+8]); 
                     }
                     m=a[0];
                     for(j=0;j<Num_ofword;j++)
                     {
                      a[j]=a[j+1]; 
                     }
                     a[Num_ofword-1]=0;//a[15]赋值为m的时候字体一直循环，尾接头，当a[15]赋值为0的时候知道所有的字体都显示完了再循环
                     delay(time0);
                 }
                 else if(t==2)
                 {
                    for(j=1;j<9;j++)
                     {
                        lc.setColumn(0, j, a[Num_ofword-17+j]); 
                     }
                    for(j=0;j<8;j++)
                     {
                        lc.setColumn(1, j, a[Num_ofword-9+j]); 
                     }
                     
                     m=a[Num_ofword-1];
                     for(j=Num_ofword-1;j>0;j--)
                     {
                      a[j]=a[j-1]; 
                     }
                     a[0]=0;//a[15]赋值为m的时候字体一直循环，尾接头，当a[15]赋值为0的时候知道所有的字体都显示完了再循环
                     delay(time0);
                 }
                 else if(t==4)
                 {
                       for(j=0;j<8;j++)
                         {
                            lc.setColumn(0, j, a[j]);                  
                            lc.setColumn(1, j, a[j+8]); 
                         }
          
                         for(j=0;j<16;j++)
                         {
                            a[j]=a[j]>>1; 
                           // Serial.println(a[j],DEC);
                         }
                         delay(time0); 
                     
                 }
                 else if(t==3)
                 {
                       for(j=0;j<8;j++)
                         {
                            lc.setColumn(0, j, a[j]);       
                            lc.setColumn(1, j, a[j+8]); 
                         }
                         for(j=0;j<16;j++)
                         {
                            a[j]=a[j]<<1; 
                           // Serial.println(a[j],DEC);
                         }
                         delay(time0); 
                 }
            }
         }
       else
          {
             
                for(i=0;i<16;i++)
                {
                   if(t==1)
                   {
                     for(j=0;j<8;j++)
                       {
                          lc.setColumn(0, j, c[j]); 
                       }
                
                     for(j=0;j<8;j++)
                       {
                          lc.setColumn(1, j, c[j+8]); 
                       }
                     m=a[0];
                     for(j=0;j<16;j++)
                     {
                      c[j]=c[j+1]; 
                     }
                     c[15]=0;//a[15]赋值为m的时候字体一直循环，尾接头，当a[15]赋值为0的时候知道所有的字体都显示完了再循环
                     delay(time0);
                   }
                   if(t==2)
                   {
                      for(j=0;j<8;j++)
                         {
                            lc.setColumn(0, j, c[j]); 
                         }
                  
                       for(j=0;j<8;j++)
                         {
                            lc.setColumn(1, j, c[j+8]); 
                         }
                       m=a[15];
                       for(j=15;j>0;j--)
                       {
                        c[j]=c[j-1]; 
                       }
                       c[0]=0;//a[15]赋值为m的时候字体一直循环，尾接头，当a[15]赋值为0的时候知道所有的字体都显示完了再循环
                       delay(time0);
                   }  
                }     
          }   
} 

//显示和关闭任意点
void YahBoom_OMIBOX_Matrix::LED_Display(uint8_t x,uint8_t y,uint8_t a)
{
    
    char j;
    if(x<0)
    x=0;
    if(x>15)
    x=15;
    if(y<0)
    y=0;
    if(y>7)
    y=7;
    if(a==1)
    {
       code[x]=code[x]|(0x01<<y);
    }
    if(a==0)
    {
       code[x]=code[x]&(~(0x01<<y));
     }
    for(j=0;j<8;j++)
    {
        lc.setColumn(0, j, code[j]); 
    }
    for(j=0;j<8;j++)
    {
        lc.setColumn(1, j, code[j+8]); 
     }
 }
 /*动态图的控制*/
void YahBoom_OMIBOX_Matrix::Move_picture(uint8_t x,uint16_t time)
{
	if(x==1)
	{
		YahBoom_OMIBOX_Matrix_ShowIcon(nictation0);	
		delay(time);
		YahBoom_OMIBOX_Matrix_ShowIcon(nictation1);	
		delay(time);
	}
	if(x==2)
	{
		YahBoom_OMIBOX_Matrix_ShowIcon(heart0);	
		delay(time);
		YahBoom_OMIBOX_Matrix_ShowIcon(heart1);	
		delay(time);
	}
	if(x==3)
	{
		YahBoom_OMIBOX_Matrix_ShowIcon(funny1);	
		delay(time);
		YahBoom_OMIBOX_Matrix_ShowIcon(funny2);	
		delay(time);
	}
	if(x==4)
	{
		YahBoom_OMIBOX_Matrix_ShowIcon(doubt1);	
		delay(time*2);
		YahBoom_OMIBOX_Matrix_ShowIcon(doubt2);	
		delay(time);
		YahBoom_OMIBOX_Matrix_ShowIcon(doubt3);	
		delay(time);
		YahBoom_OMIBOX_Matrix_ShowIcon(doubt4);	
		delay(time);
		YahBoom_OMIBOX_Matrix_ShowIcon(doubt5);	
		delay(time);
		YahBoom_OMIBOX_Matrix_ShowIcon(doubt6);	
		delay(time);
		YahBoom_OMIBOX_Matrix_ShowIcon(doubt7);	
		delay(time);
		YahBoom_OMIBOX_Matrix_ShowIcon(doubt8);	
		delay(time);
		YahBoom_OMIBOX_Matrix_ShowIcon(doubt9);	
		delay(time);
		YahBoom_OMIBOX_Matrix_ShowIcon(doubt10);	
		delay(time);
		YahBoom_OMIBOX_Matrix_ShowIcon(doubt11);	
		delay(time*2);
	}
	if(x==5)
	{
		YahBoom_OMIBOX_Matrix_ShowIcon(mischievous_ghost1);	
		delay(time);
		YahBoom_OMIBOX_Matrix_ShowIcon(mischievous_ghost2);	
		delay(time);
		YahBoom_OMIBOX_Matrix_ShowIcon(mischievous_ghost3);	
		delay(time);
		YahBoom_OMIBOX_Matrix_ShowIcon(mischievous_ghost4);	
		delay(time);
	}
	if(x==6)
	{
		YahBoom_OMIBOX_Matrix_ShowIcon(crying_wolf1);	
		delay(time);
		YahBoom_OMIBOX_Matrix_ShowIcon(crying_wolf2);	
		delay(time);
	}
	
}
	
	/*静态图*/
void YahBoom_OMIBOX_Matrix::Static_picture(uint8_t x)
{
	if(x==1)
	{
		YahBoom_OMIBOX_Matrix_ShowIcon(Heart);	
	}
	if(x==2)
	{
		YahBoom_OMIBOX_Matrix_ShowIcon(Triangle );	
	}
	if(x==3)
	{
		YahBoom_OMIBOX_Matrix_ShowIcon(House );	
	}
	if(x==4)
	{
		YahBoom_OMIBOX_Matrix_ShowIcon(Boat );	
	}	
	if(x==5)
	{
		YahBoom_OMIBOX_Matrix_ShowIcon(Music );	
	}
	if(x==6)
	{
		YahBoom_OMIBOX_Matrix_ShowIcon(Umbrella );	
	}	
	if(x==7)
	{
		YahBoom_OMIBOX_Matrix_ShowIcon(Right );	
	}	
	if(x==8)
	{
		YahBoom_OMIBOX_Matrix_ShowIcon(Error );	
	}
	if(x==9)
	{
		YahBoom_OMIBOX_Matrix_ShowIcon(Rectangle );	
	}	
}
 	
 	
 	
/*****************************************************************/



/******************************************************************/

/*蜂鸣器和音乐控制*/


YahBoom_OMIBOX_Buzzer::YahBoom_OMIBOX_Buzzer(byte pinname)
{
		pin = pinname;
}
YahBoom_OMIBOX_Buzzer::~YahBoom_OMIBOX_Buzzer()
{

		pinMode(pin, INPUT);
} 
void YahBoom_OMIBOX_Buzzer::setBuzzer_Tone(uint16_t frequency, uint32_t duration)
{
		int period = 1000000L / frequency;//1000000L
  	int pulse = period / 2;
		pinMode(pin, OUTPUT);
  	for (long i = 0; i < duration * 200000L; i += period) 
  	{
			digitalWrite(pin, 1);
    	delayMicroseconds(pulse);
    	digitalWrite(pin, 0);
    	delayMicroseconds(pulse);
  	}
  	if(frequency==0)
  	delay(230*duration);	
		delay(20);
}

void YahBoom_OMIBOX_Buzzer::birthday(int j)
{
	//int j;
	//for(j=0;j<21;j++)
	//{
		setBuzzer_Tone(pgm_read_word_near(&BIrthday[j][0]), pgm_read_word_near(&BIrthday[j][1]));	
	//}

   
}

void YahBoom_OMIBOX_Buzzer::childhood(int j)
{
	//int j;
	//for(j=0;j<80;j++)
	//{
		setBuzzer_Tone(pgm_read_word_near(&CHildhood[j][0]), pgm_read_word_near(&CHildhood[j][1]));	
	//}


}

void YahBoom_OMIBOX_Buzzer::fenshuajiang(int j)
{
	//int j;
	//for(j=0;j<48;j++)
	//{
		setBuzzer_Tone(pgm_read_word_near(&FEnshuajiang[j][0]), pgm_read_word_near(&FEnshuajiang[j][1]));	
	//}
}

void YahBoom_OMIBOX_Buzzer::shishangzhiyoumamahao(int j)
{
	//int j;
	//for(j=0;j<34;j++)
	//{
		setBuzzer_Tone(pgm_read_word_near(&SHishangzhiyoumamahao[j][0]), pgm_read_word_near(&SHishangzhiyoumamahao[j][1]));	
	//}
}


void YahBoom_OMIBOX_Buzzer::liangzhilaohu(int j)
{
	//int j;
	//for(j=0;j<32;j++)
	//{
		setBuzzer_Tone(pgm_read_word_near(&LIangzhilaohu[j][0]), pgm_read_word_near(&LIangzhilaohu[j][1]));	
	//}
}

void YahBoom_OMIBOX_Buzzer::music_Play(uint8_t v_song, uint8_t index)
{
		switch(v_song){ 
			case enBirthday:
		  {
			    birthday(index);
					break;
			}
			case Childhood:
			{
					childhood(index);		
					break;
			}
			case Fenshuajiang:
			{
					fenshuajiang(index);		
					break;
			}
			case Shishangzhiyoumamahao:
		  {
					shishangzhiyoumamahao(index);		
					break;
			}
		  case Liangzhilaohu:
		  {
					liangzhilaohu(index);			
					break;
			}
				
		}
		
}


/*********************************************************************/

/******************************************************************/

/*按键*/
YahBoom_OMIBOX_Button::YahBoom_OMIBOX_Button()
{
}

YahBoom_OMIBOX_Button::~YahBoom_OMIBOX_Button()
{
} 

bool YahBoom_OMIBOX_Button::Get_Button_State(byte PinNum)
{
		int temp = 1;
	  pinMode(PinNum, INPUT);
	  temp = digitalRead(PinNum);
	  if(temp == 0)
	  	return true;
	  else
	  	return false;
		
}


/*********************************************************************/

/******************************************************************/

/*巡线*/
YahBoom_OMIBOX_TrackLine::YahBoom_OMIBOX_TrackLine(byte pin1, byte pin2)
{
    pinName1 = pin1;
    pinName2 = pin2;
   	
}

YahBoom_OMIBOX_TrackLine::~YahBoom_OMIBOX_TrackLine()
{
} 

void YahBoom_OMIBOX_TrackLine::YahBoom_OMIBOX_TrackLine_Init()
{
	pinMode(pinName1, INPUT);
	pinMode(pinName2, INPUT);
} 

bool YahBoom_OMIBOX_TrackLine::Senser_State_L(uint8_t v_state)
{
	uint8_t state;
	state = digitalRead(pinName1);
	if(state == v_state)
		return false;
	else 
		return true;	
	
}

bool YahBoom_OMIBOX_TrackLine::Senser_State_R(uint8_t v_state)
{

	uint8_t state;
	state = digitalRead(pinName2);
	if(state == v_state)
		return false;
	else 
		return true;	
}

/*********************************************************************/

/******************************************************************/

/*红外避障*/
YahBoom_OMIBOX_IRAvoid::YahBoom_OMIBOX_IRAvoid(byte pin1, byte pin2)
{
    pinName1 = pin1;
   	pinName2 = pin2;
}

YahBoom_OMIBOX_IRAvoid::~YahBoom_OMIBOX_IRAvoid()
{
} 
void YahBoom_OMIBOX_IRAvoid::YahBoom_OMIBOX_IRAvoid_Init()
{
	pinMode(pinName1, INPUT);
	pinMode(pinName2, INPUT);
}

bool YahBoom_OMIBOX_IRAvoid::Senser_State_L()
{

	uint8_t state;
	state = digitalRead(pinName1);
	if(state == 1)
		return true;
	else 
		return false;	
}

bool YahBoom_OMIBOX_IRAvoid::Senser_State_R()
{
	uint8_t state;
	state = digitalRead(pinName2);
	if(state == 1)
		return true;
	else 
		return false;	
}

/*********************************************************************/

/******************************************************************/
/****电压检测****/
YahBoom_OMIBOX_Voltage::YahBoom_OMIBOX_Voltage(byte pin)//电压检测为A2引脚
{
    pinName = pin;
}

YahBoom_OMIBOX_Voltage::~YahBoom_OMIBOX_Voltage()
{
} 

void YahBoom_OMIBOX_Voltage::YahBoom_OMIBOX_Voltage_Init()
{
	pinMode(pinName, INPUT);
}

float YahBoom_OMIBOX_Voltage::get_Voltage()
{
	float state = 0;
	state = analogRead(pinName);
	state = state/1023*5.0;
	return state;	
}
/*光敏*/
YahBoom_OMIBOX_LightSensor::YahBoom_OMIBOX_LightSensor(byte pin)
{
    pinName = pin;
}

YahBoom_OMIBOX_LightSensor::~YahBoom_OMIBOX_LightSensor()
{
} 

void YahBoom_OMIBOX_LightSensor::YahBoom_OMIBOX_LightSensor_Init()
{
	pinMode(pinName, INPUT);
}

int YahBoom_OMIBOX_LightSensor::get_LightSensor()
{
	int state = 0;
	state = analogRead(pinName);
	return state;	
}

/*声音*/
YahBoom_OMIBOX_VoiceSensor::YahBoom_OMIBOX_VoiceSensor(byte pin)
{
    pinName = pin;
}

YahBoom_OMIBOX_VoiceSensor::~YahBoom_OMIBOX_VoiceSensor()
{
	 
} 

void YahBoom_OMIBOX_VoiceSensor::YahBoom_OMIBOX_VoiceSensor_Init()
{
	
	pinMode(pinName, INPUT);

}

int YahBoom_OMIBOX_VoiceSensor::get_VoiceSensor()
{
	int state = 0;
	state = analogRead(pinName);
	return state;	
}

/*********************************************************************/

/******************************************************************/

/*小车电机和舵机*/
YahBoom_OMIBOX_MotorServo::YahBoom_OMIBOX_MotorServo()
{
	pwm = Adafruit_PWMServoDriver(0x41); 
}

YahBoom_OMIBOX_MotorServo::~YahBoom_OMIBOX_MotorServo()
{

} 

void YahBoom_OMIBOX_MotorServo::YahBoom_OMIBOX_MotorServo_Init()
{
 		pwm.begin();
		pwm.setPWMFreq(50);  // Analog servos run at ~60 Hz updates
}
/*舵机部分*/
/**
* Function       SetServoPos
* @author        liusen
* @date          2018.05.07
* @brief         舵机控制
* @param[in]     num: 舵机的编号； value：舵机角度：0-180
* @retval        void
* @par History   无
*/
void YahBoom_OMIBOX_MotorServo::SetServoPos(int num, int value)
{
    // 50hz: 20,000 us
    long us = (value * 1800.0 / 270 + 600); // 0.6 ~ 2.4 
    long pwmvalue = us * 4096 / 20000;
    pwm.setPWM(num, 0, pwmvalue);
}

/*
//外部调用
void YahBoom_OMIBOX_MotorServo::SetServoAngle(int num, int value)
{
   if(num == 1)
   		SetServoPos(3, value);  //S1
   else if(num == 2)
   		SetServoPos(13, value); //S2
}
*/

//森哥让换的外部调用
void YahBoom_OMIBOX_MotorServo::SetServoAngle(int num, int value)
{
   switch(num)
   {
      case 1: SetServoPos(3, value);break;    //S1
      case 2: SetServoPos(13, value);break;   //S2
      case 3: SetServoPos(4, value);break;    //S3
      case 4: SetServoPos(12, value);break;   //S4
   }
}

void YahBoom_OMIBOX_MotorServo::Run(int speed, int delay_ms)
{
		speed = map(speed, 0, 255, 0, 4095);
		pwm.setPWM(L_Motor_PIN1, 0, speed);
    pwm.setPWM(L_Motor_PIN2, 0, 0);
    
    pwm.setPWM(R_Motor_PIN1, 0, speed);
    pwm.setPWM(R_Motor_PIN2, 0, 0);
		delay(delay_ms);	
}

void YahBoom_OMIBOX_MotorServo::Back(int speed, int delay_ms)
{
	 speed = map(speed, 0, 255, 0, 4095);
    pwm.setPWM(L_Motor_PIN1, 0, 0);
    pwm.setPWM(L_Motor_PIN2, 0, speed);
    
    pwm.setPWM(R_Motor_PIN1, 0, 0);
    pwm.setPWM(R_Motor_PIN2, 0, speed);
		delay(delay_ms);	

}

void YahBoom_OMIBOX_MotorServo::Left(int speed, int delay_ms)
{
		speed = map(speed, 0, 255, 0, 4095);
 		pwm.setPWM(L_Motor_PIN1, 0, 0);
    pwm.setPWM(L_Motor_PIN2, 0, 0);
    
    pwm.setPWM(R_Motor_PIN1, 0, speed);
    pwm.setPWM(R_Motor_PIN2, 0, 0);
		delay(delay_ms);	

}

void YahBoom_OMIBOX_MotorServo::Right(int speed, int delay_ms)
{
		speed = map(speed, 0, 255, 0, 4095);
 		pwm.setPWM(L_Motor_PIN1, 0, speed);
    pwm.setPWM(L_Motor_PIN2, 0, 0);
    
    pwm.setPWM(R_Motor_PIN1, 0, 0);
    pwm.setPWM(R_Motor_PIN2, 0, 0);

		delay(delay_ms);	
}

void YahBoom_OMIBOX_MotorServo::Stop(int speed, int delay_ms)
{
		
		pwm.setPWM(L_Motor_PIN1, 0, 0);
	  pwm.setPWM(L_Motor_PIN2, 0, 0);
	    
	  pwm.setPWM(R_Motor_PIN1, 0, 0);
	  pwm.setPWM(R_Motor_PIN2, 0, 0);
		delay(delay_ms);	

}

void YahBoom_OMIBOX_MotorServo::SpinRight(int speed, int delay_ms)
{
		speed = map(speed, 0, 255, 0, 4095);
		pwm.setPWM(L_Motor_PIN1, 0, speed);
   	pwm.setPWM(L_Motor_PIN2, 0, 0);
    
    pwm.setPWM(R_Motor_PIN1, 0, 0);
    pwm.setPWM(R_Motor_PIN2, 0, speed);

		delay(delay_ms);
}

void YahBoom_OMIBOX_MotorServo::SpinLeft(int speed, int delay_ms)
{
		speed = map(speed, 0, 255, 0, 4095);
   	pwm.setPWM(L_Motor_PIN1, 0, 0);
   	pwm.setPWM(L_Motor_PIN2, 0, speed);
    
		pwm.setPWM(R_Motor_PIN1, 0, speed);
    pwm.setPWM(R_Motor_PIN2, 0, 0);
	
		delay(delay_ms);
}

void YahBoom_OMIBOX_MotorServo::MotorSpeed(int left, int right, int delay_ms)
{
		int Lspeed = 0, Rspeed = 0;
		if(left < 0)
		{
			Lspeed = abs(left);
			Lspeed = map(Lspeed, 0, 255, 0, 4095);
			pwm.setPWM(L_Motor_PIN1, 0, 0);
    	pwm.setPWM(L_Motor_PIN2, 0, Lspeed);	
		}
		else
		{
			Lspeed = map(left, 0, 255, 0, 4095);
			pwm.setPWM(L_Motor_PIN1, 0, Lspeed);
    	pwm.setPWM(L_Motor_PIN2, 0, 0);		
		}
		if(right < 0)
		{
			Rspeed = abs(right);
			Rspeed = map(Rspeed, 0, 255, 0, 4095);
			pwm.setPWM(R_Motor_PIN1, 0, 0);
    	pwm.setPWM(R_Motor_PIN2, 0, Rspeed);	
		}
		else
		{
			Rspeed = map(right, 0, 255, 0, 4095);
			pwm.setPWM(R_Motor_PIN1, 0, Rspeed);
    	pwm.setPWM(R_Motor_PIN2, 0, 0);		
		}	
		
		delay(delay_ms);	
}

void YahBoom_OMIBOX_MotorServo::LeftMotor(int direction, int speed)
{
	
	speed = map(speed, 0, 255, 0, 4095);
	if(direction == 1) //正转
	{
		pwm.setPWM(L_Motor_PIN1, 0, speed);
    pwm.setPWM(L_Motor_PIN2, 0, 0);		
	}
	else if (direction == 2) //反转
	{
		pwm.setPWM(L_Motor_PIN1, 0, 0);
    pwm.setPWM(L_Motor_PIN2, 0, speed);	
	}
	else //停止
	{
		pwm.setPWM(L_Motor_PIN1, 0, 0);
    pwm.setPWM(L_Motor_PIN2, 0, 0);	
	}
}

void YahBoom_OMIBOX_MotorServo::RightMotor(int direction, int speed)
{

  speed = map(speed, 0, 255, 0, 4095);
	if(direction == 1) //正转
	{
		pwm.setPWM(R_Motor_PIN1, 0, speed);
    pwm.setPWM(R_Motor_PIN2, 0, 0);		
	}
	else if (direction == 2) //反转
	{
		pwm.setPWM(R_Motor_PIN1, 0, 0);
    pwm.setPWM(R_Motor_PIN2, 0, speed);	
	}
	else //停止
	{
		pwm.setPWM(R_Motor_PIN1, 0, 0);
    pwm.setPWM(R_Motor_PIN2, 0, 0);	
	}
	
}
  	
YahBoom_OMIBOX_Color::YahBoom_OMIBOX_Color()
{
		
}
YahBoom_OMIBOX_Color::~YahBoom_OMIBOX_Color()
{
		
}
void YahBoom_OMIBOX_Color::PowerSwith(int onoff)
{
  if(onoff == 1)
  {
    digitalWrite(4,1);
  } 
  else
  {
    digitalWrite(4,0);
  } 
}
void YahBoom_OMIBOX_Color::RGB_Config()
{
		uint8_t W_ADDR;
 
	  //复位寄存器，200ms积分时间，CRGB通道全部开启
	  Wire.beginTransmission(ADDR);   //发送Device地址
	  Wire.write(ControlReg_Addr);   
	  Wire.write(0x06); 
	  Wire.endTransmission();         //发送结速信号
	
	  Wire.beginTransmission(ADDR);  //发送Device地址
	  Wire.write(0x04);   
	  Wire.write(0x41);              //16bit 50ms
	  Wire.endTransmission();        //发送结速信号
	
	  Wire.beginTransmission(ADDR);  //发送Device地址
	  Wire.write(0x05);   
	  Wire.write(0x01); 
	  Wire.endTransmission();        //发送结速信号
}

void YahBoom_OMIBOX_Color::YahBoom_OMIBOX_Color_Init()
{

	  Wire.begin();
  	Wire.setClock(100000);
  	RGB_Config();
}

 /* 原来的写法 */
bool YahBoom_OMIBOX_Color::YahBoom_OMIBOX_Identify_Color(byte color)
{
	int nowcolor = 0;
  int index = 0;
  char ColorData[6] = {0};
  
  //红色

  Wire.beginTransmission(ADDR);  //发送Device地址
  Wire.write(0x10);  
  Wire.endTransmission();     //发送结束信号
  Wire.beginTransmission(ADDR);  //发送Device地址
  Wire.requestFrom(ADDR, 2);    // request 1 bytes from slave device #2
  while(Wire.available())    // slave may send less than requested
  { 
    char ff = Wire.read();    // receive a byte as character
    ColorData[index] = ff;
    index++;

  }
  Wire.endTransmission();     //发送结束信号

  //绿色
  Wire.beginTransmission(ADDR);  //发送Device地址
  Wire.write(0x0D);  
  Wire.endTransmission();     //发送结束信号
  Wire.beginTransmission(ADDR);  //发送Device地址
  Wire.requestFrom(ADDR, 2);    // request 1 bytes from slave device #2
  while(Wire.available())    // slave may send less than requested
  { 
    char ff = Wire.read();    // receive a byte as character
    ColorData[index] = ff;
    index++;
  //   printf("Green:%x\n", ff);  
  }
  Wire.endTransmission();     //发送结束信号

  //蓝色
  Wire.beginTransmission(ADDR);  //发送Device地址
  Wire.write(0x13);  
  Wire.endTransmission();     //发送结束信号
  Wire.beginTransmission(ADDR);  //发送Device地址
  Wire.requestFrom(ADDR, 2);    // request 1 bytes from slave device #2
  while(Wire.available())    // slave may send less than requested
  { 
    char ff = Wire.read();    // receive a byte as character
    ColorData[index] = ff;
    index++;
  //printf("Blue:%x\n", ff);  
  }
  Wire.endTransmission();     //发送结束信号
  //delay(500);  
  // printf("R:%x,%x, G:%x,%x, B:%x, %x,\n",ColorData[0], ColorData[1], ColorData[2], ColorData[3], ColorData[4], ColorData[5]);

  Red = ((int)(ColorData[1]&0xff) << 8 | (ColorData[0]&0xff));//2.06
  Green = ((int)(ColorData[3]&0xff) << 8 | (ColorData[2]&0xff));
  Blue = ((int)(ColorData[5]&0xff) << 8 | (ColorData[4]&0xff));//1.19

  //printf("R:%d, G:%d, B:%d\n", Red, Green, Blue);
  //Serial.println("R:%d, G:%d, B:%d\n", Red, Green, Blue);
  //Serial.println(Red,DEC);
  //Serial.println(Green,DEC);
  //Serial.println(Blue,DEC);
  //Serial.println(0);

 if(Red >= 1100 && Red <= 2500 && Green >= 900 && Green <= 2100 && Blue >= 100 && Blue <= 1000)//红色
  {
    Red = 255;
    Green = 0;
    Blue = 0;
    nowcolor = 1;
  }

  else if(Red >= 450 && Red <= 1200 && Green >= 800 && Green <= 2000 && Blue >= 500 && Blue <= 1100) //绿色

  {
    Red = 0;
    Green = 255;
    Blue = 0;
    nowcolor = 2;
  }

  else if(Red >= 400 && Red <= 1200 && Green >= 1500 && Green <= 3000 && Blue >= 1100 && Blue <= 3000)//蓝色
  {
    Red = 0;
    Green = 0;
    Blue = 255;
    nowcolor = 3;
  }

  else if(Red >= 2500 && Green >= 5000 && Blue >= 2600 ) //白色
  {
    Red = 0;
    Green = 0;
    Blue = 0;
    nowcolor = 4;
  }

  if(nowcolor == color)
    return true;
  else
    return false;
  	
}


//输出颜色识别传感器检测到的值
void YahBoom_OMIBOX_Color::get_ColorSensor()
{
	int index = 0;
  char ColorData[6] = {0};
  
  //红色
  Wire.beginTransmission(ADDR);   //发送Device地址
  Wire.write(0x10);  
  Wire.endTransmission();         //发送结束信号
  Wire.beginTransmission(ADDR);   //发送Device地址
  Wire.requestFrom(ADDR, 2);      // request 1 bytes from slave device #2
  while(Wire.available())         // slave may send less than requested
  { 
    char ff = Wire.read();      // receive a byte as character
    ColorData[index] = ff;
    index++;
  }
  Wire.endTransmission();     //发送结束信号
  
  //绿色
  Wire.beginTransmission(ADDR);   //发送Device地址
  Wire.write(0x0D);  
  Wire.endTransmission();         //发送结束信号
  Wire.beginTransmission(ADDR);   //发送Device地址
  Wire.requestFrom(ADDR, 2);      // request 1 bytes from slave device #2
  while(Wire.available())         // slave may send less than requested
  { 
    char ff = Wire.read();      // receive a byte as character
    ColorData[index] = ff; 
    index++;
    //printf("Green:%x\n", ff);  
  }
  Wire.endTransmission();         //发送结束信号
  
  //蓝色
  Wire.beginTransmission(ADDR);     //发送Device地址
  Wire.write(0x13);  
  Wire.endTransmission();          //发送结束信号
  Wire.beginTransmission(ADDR);    //发送Device地址
  Wire.requestFrom(ADDR, 2);       // request 1 bytes from slave device #2
  while(Wire.available())          // slave may send less than requested
  { 
    char ff = Wire.read();          // receive a byte as character
    ColorData[index] = ff;
    index++;
    //printf("Blue:%x\n", ff);  
  }
  Wire.endTransmission();     //发送结束信号
  //delay(500);  

  Red = ((unsigned int)(ColorData[1]&0xff) << 8 | (ColorData[0]&0xff));//2.06
  Green = ((unsigned int)(ColorData[3]&0xff) << 8 | (ColorData[2]&0xff));
  Blue = ((unsigned int)(ColorData[5]&0xff) << 8 | (ColorData[4]&0xff));

  if(Red > 4500) Red = 4500;
  if(Green > 7600) Green = 7600;
  if(Blue > 4600) Blue = 4600;
  val_red = map(Red, 0, 4500, 0, 255);
  val_blue = map(Blue, 0, 4600, 0, 255);
  val_green = map(Green, 0, 7600, 0, 255);
    
    
  if (val_red > val_green && val_red > val_blue)  
  {     
    val_red = 255;
    val_green /= 2;
    val_blue /= 2;
    
  } 
  else if (val_green > val_red && val_green > val_blue)     
  {

    val_green = 255;
    val_red /= 2;
    val_blue /= 2;
  } 
  else if (val_blue > val_red && val_blue >  val_green)       
  {
    val_blue = 255;
    val_red /=2;
    val_green /= 2;

    }
   
}  
	
//输出颜色识别传感器检测到的Red的值
int YahBoom_OMIBOX_Color::ReturnRedValue()
{
	get_ColorSensor();
	return val_red;
}

//输出颜色识别传感器检测到的Green的值
int YahBoom_OMIBOX_Color::ReturnGreenValue()
{
	get_ColorSensor();
	return val_green;
}

//输出颜色识别传感器检测到的Blue的值
int YahBoom_OMIBOX_Color::ReturnBlueValue()
{
	get_ColorSensor();
	return val_blue;
}


/*红外遥控 */
/******************************************************************/

YahBoom_OMIBOX_IRrecv::YahBoom_OMIBOX_IRrecv()
{
	last = millis();
	receive_number=0;
}

YahBoom_OMIBOX_IRrecv::~YahBoom_OMIBOX_IRrecv()
{
	
}

void YahBoom_OMIBOX_IRrecv::YahBoom_OMIBOX_IRrecv_Init()
{

	pinMode(2, INPUT_PULLUP);     //将2号管脚设置为输入并且内部上拉模式2
	irrecv.enableIRIn(); // 初始化红外解码
	
}
 int YahBoom_OMIBOX_IRrecv::IR_Deal()
{
		//int receive_number=0;
  if (irrecv.decode(&results))
  {
  	//Serial.println(results.value,HEX);
    switch (results.value)
    {
      case 0X00FF00FF: receive_number = 1;  break;
      case 0X00FF807F: receive_number = 4;  break;//
      case 0X00FF40BF: receive_number = 2;  break;
      case 0X00FF20DF: receive_number = 6;  break;//
      case 0X00FFA05F: receive_number = 3;  break;
      case 0X00FF609F: receive_number = 7;  break;//
      case 0X00FF10EF: receive_number = 8;  break;
      case 0X00FF906F: receive_number = 5;  break;//
      case 0X00FF50AF: receive_number = 9;  break;
      case 0X00FF30CF: receive_number = 10;  break;
      case 0X00FFB04F: receive_number = 12;  break;
      case 0X00FF708F: receive_number = 11;  break;
      case 0X00FF08F7: receive_number = 13;  break;
      case 0X00FF8877: receive_number = 14;  break;
      case 0X00FF48B7: receive_number = 15;  break;
      case 0X00FF28D7: receive_number = 16;  break;
      case 0X00FFA857: receive_number = 17;  break;
      case 0X00FF6897: receive_number = 18;  break;
      case 0X00FF18E7: receive_number = 19;  break;
      case 0X00FF9867: receive_number = 20;  break;
      case 0X00FF58A7: receive_number = 21;  break;
      //case 0xFFFFFFFF: receive_number = last_receive_number;	 break;
      default: break;
    }
   // 
   //last_receive_number = receive_number;
   last = millis();
    irrecv.resume(); // 接收下一个编码
    //Serial.println(receive_number,DEC);
   // return receive_number; 
   //last_receive_number = receive_number;
  }
  
  	/*if(receive_number==num)
    	{
    		last_receive_number = receive_number;
    		//receive_number = 0;
    		return 1;
    	}
    	
    else
    {
    		last_receive_number = 0;
    		return 0; 
    }*/
  	else if (millis() - last > 120)
  		{
  		  receive_number = 0;
   		 last = millis();
   	//	 return 0;
		 }
		 return receive_number; 
   
}
/***硬件初始化***/
YahBoom_OMIBOX::YahBoom_OMIBOX()
{

}

YahBoom_OMIBOX::~YahBoom_OMIBOX()
{
	
}

void YahBoom_OMIBOX::Init()
{
		
    
}
