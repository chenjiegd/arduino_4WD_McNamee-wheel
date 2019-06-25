
#include "./YahBoom_OMIBOX.h"

#define run_car     '1'//按键前
#define back_car    '2'//按键后
#define left_car    '3'//按键左
#define right_car   '4'//按键右
#define spin_left_car '5'//左旋
#define spin_right_car '6'//右旋
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

/*串口数据设置*/
int IncomingByte = 0;             //接收到的 data byte
String InputString = "";          //用来储存接收到的内容
boolean NewLineReceived = false;  //前一次数据结束标志
boolean StartBit  = false;        //协议开始标志
String ReturnTemp = "";           //存储返回值
/*状态机状态*/
int g_CarState = enSTOP;          //1前2后3左4右0停止
int g_oldcarState = 11;
unsigned long g_uptimes = 100000; //自动上报时间间隔
int g_Mode = 0;                   //1：流水灯  2：跑马灯  3：呼吸灯  4：巡线模式  5：避障模式  6：跟随模式  7：声控模式 8：关闭模式
int g_MusicMode = 0;              //1：播放 2：暂停 
int g_SoundsSelect = 1;           //默认播放第一首
const unsigned char music_max[5] = {20,79,47,33,31};   //所有歌最大长度
char matrixdata[16] = {0};
int g_ReportMode = 1;             //1：上报电池不上报颜色，2：上报颜色
/*运动*/


YahBoom_OMIBOX_MotorServo cMotor;
YahBoom_OMIBOX_Matrix cMatrix;
YahBoom_OMIBOX_RGBLED cRGB(8, 5);
YahBoom_OMIBOX_Buzzer cBuzzer(6);
YahBoom_OMIBOX_Voltage cVoltage(A2);
YahBoom_OMIBOX_Color cColor;
YahBoom_OMIBOX_TrackLine cTrack(7, 8);
YahBoom_OMIBOX_IRAvoid cIR(A0, A1);
YahBoom_OMIBOX_VoiceSensor cVoiceSensor(A7);

/*硬件初始化*/
void Init_All()
{
    unsigned char picBuffer[16];
    unsigned char *picTemp;
    pinMode(4,OUTPUT);
    digitalWrite(4,0);
    cMatrix.YahBoom_OMIBOX_Matrix_Init();
    cRGB.RGBLED_Init();
    cRGB.RGBLED_Show_All(0,0,0);  
    picTemp = new unsigned char[16]{
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0
    ,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
    memcpy(picBuffer,picTemp,16);
    free(picTemp);
    cMatrix.YahBoom_OMIBOX_Matrix_Show(picBuffer);  
    cMotor.YahBoom_OMIBOX_MotorServo_Init();
    cColor.YahBoom_OMIBOX_Color_Init(); 
    cTrack.YahBoom_OMIBOX_TrackLine_Init();
    cIR.YahBoom_OMIBOX_IRAvoid_Init();
    cVoiceSensor.YahBoom_OMIBOX_VoiceSensor_Init();
}

/*初始化程序*/
void setup()
{

    Serial.begin(115200);
    //Serial.println("Hello World\n");
    //for(int i= 0; i < 5; i++)
    //  cMatrix.Move_picture(1, 500);
    Init_All(); 
    cMatrix.LED_ShowStrroll("OMIBOX", 1, 0);
    cMatrix.Move_picture(1, 200);
    cMatrix.Move_picture(1, 200);
    cMatrix.Move_picture(1, 200);
    cBuzzer.setBuzzer_Tone(262, 1);
}

/*自动上报数据*/
void SendAutoUp()
{
  g_uptimes --;
  if (g_uptimes == 0)
  {
    char temp[5]={0};
    //电压
    float voltage = cVoltage.get_Voltage();
    memset(temp, 0x00, sizeof(temp));
    dtostrf(voltage, 1, 2, temp);  // 相當於 %2.1f
    String VT = temp;
    //颜色
    String returntemp;
    if(g_ReportMode == 2)
    {
      cColor.PowerSwith(1);
      if(cColor.YahBoom_OMIBOX_Identify_Color(1) == true)
      {
        returntemp = "$09" + VT +",1" + "#";
      }
      else if(cColor.YahBoom_OMIBOX_Identify_Color(2) == true)
      {
        returntemp = "$09" + VT +",2" + "#";
      }
      else if(cColor.YahBoom_OMIBOX_Identify_Color(3) == true)
      {
        returntemp = "$09" + VT +",3" + "#";
      }
      else if(cColor.YahBoom_OMIBOX_Identify_Color(4) == true)
      {
        returntemp = "$09" + VT +",4" + "#";
      }
      else
      {
        returntemp = "$09" + VT +",0" + "#";
      }
    }
    else
    {
      cColor.PowerSwith(0);
      returntemp = "$09" + VT +",0" + "#";      
    } 
    Serial.print(returntemp); //返回协议数据包
  }
  
  if (g_uptimes == 0)
      g_uptimes = 100000;
}

void Start_Music()
{
    cBuzzer.setBuzzer_Tone(349, 2);
    cBuzzer.setBuzzer_Tone(440, 2);
    cBuzzer.setBuzzer_Tone(523, 2);
}

int g_num = 0;
/*主循环程序*/
void loop()
{

    if (NewLineReceived)
    {
      serial_data_parse();  //调用串口解析函数
    }
    //根据小车状态做相应的动作
    if(g_CarState != g_oldcarState)
    {
      g_oldcarState = g_CarState;
      if(g_Mode == 0)
      {
        switch (g_CarState)
        {
          case enSTOP: cMotor.Stop(255, 0); break;
          case enRUN: cMotor.Run(255, 0); break;
          case enLEFT: cMotor.Left(255, 0); break;
          case enRIGHT: cMotor.Right(255, 0); break;
          case enBACK: cMotor.Back(255, 0); break;
          case enTLEFT: cMotor.SpinLeft(255, 0); break;
          case enTRIGHT: cMotor.SpinRight(255, 0); break;
          default: cMotor.Stop(255, 0); break;
        }
      }
    }
    else //if(g_Mode != 0 )
    {
      switch(g_Mode)
      {
        case 0:  SendAutoUp(); break;  //空闲模式下上报数据
        case 1:  Mode_WaterLED_Func(); break;
        case 2:  Mode_HorseLED_Func(); break;
        case 3:  Mode_chameleon_Func();cColor.PowerSwith(1); break;
        case 4:  Mode_TrackLine_Func(); break;
        case 5:  Mode_Avoid_Func(); break;
        case 6:  Mode_Fllow_Func(); break;
        case 7:  Mode_VoiceCtrl_Func();break;
        case 8:  Mode_Close_Func();cColor.PowerSwith(0); break;   
      }
    }

    //音乐模式控制
    if(g_MusicMode == 1) //播放
    {
        if(g_SoundsSelect != 0 )
        {
            switch(g_SoundsSelect)
            {
              case 1: cBuzzer.music_Play(1, g_num);break;
              case 2: cBuzzer.music_Play(2, g_num);break;
              case 3: cBuzzer.music_Play(3, g_num);break;
              case 4: cBuzzer.music_Play(4, g_num);break;
              case 5: cBuzzer.music_Play(5, g_num);break;  
            }
            g_num++;
            if(g_num >= music_max[g_SoundsSelect - 1])
            {
              g_num = 0;
              g_MusicMode = 2; //播放完毕暂停
            }
        }
        
    }
    else   //暂停
    {        
        SendAutoUp();
    }
}
/**
* Function       CMD_Sport_Fun
* @author        liusen
* @date          2019.02.27
* @brief         运动解析
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void CMD_Sport_Func(unsigned char str)
{
  
  switch (str)
  {
    case run_car:   g_CarState = enRUN;  break;
    case back_car:  g_CarState = enBACK;  break;
    case left_car:  g_CarState = enLEFT;  break;
    case right_car: g_CarState = enRIGHT;  break;
    case spin_left_car: g_CarState = enTLEFT;  break;
    case spin_right_car: g_CarState = enTRIGHT;  break;
    case stop_car:  g_CarState = enSTOP;  break;
    default: g_CarState = enSTOP; break;
  }
}

//点阵单独点亮模式
void CMD_Matrix_Show_Func()
{
   int X, Y, state;
   X = (InputString[3] - 0x30)*10 + (InputString[4] - 0x30);
   Y = (InputString[5] - 0x30)*10 + (InputString[6] - 0x30);
   state = (InputString[7] - 0x30);
   cMatrix.LED_Display(X, Y, state);
}

//点阵表情控制
void CMD_Matrix_Picture_Func(uint8_t static_or_move, uint8_t index)
{
  if(static_or_move == 1 && index < 7)
  {
    cMatrix.Move_picture(index, 500);
    
  }
  else if(static_or_move == 2 && index < 10)
  {
    cMatrix.Static_picture(index);
  }
  else
  {
    unsigned char picBuffer[16];
    unsigned char *picTemp;  
    picTemp = new unsigned char[16]{
    0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0
    ,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
    memcpy(picBuffer,picTemp,16);
    free(picTemp);
    cMatrix.YahBoom_OMIBOX_Matrix_Show(picBuffer); 
  }
}

//舵机控制模式
void CMD_Servo_Func()
{
  uint8_t index, pos;
  index = InputString[3] - 0x30;
  pos = (InputString[4] - 0x30)*100 + (InputString[5] - 0x30)*10 + (InputString[6]-0x30);
  if(index < 5 && pos <= 180)
    cMotor.SetServoAngle(index,pos);
}

//七彩灯控制模式
void CMD_RGB_Func()
{
  
    uint8_t num=0, Red=0, Green=0, Blue=0;
    String temp = "";
    temp = InputString.substring(3, 13);
    //Serial.println(temp);
   
    num = temp[0] - 0x30;
    Red = (temp[1] - 0x30)*100 + (temp[2] - 0x30)*10 + (temp[3]-0x30);
    Green = (temp[4] - 0x30)*100 + (temp[5] - 0x30)*10 + (temp[6]-0x30);
    Blue = (temp[7] - 0x30)*100 + (temp[8] - 0x30)*10 + (temp[9]-0x30);
    //  //Serial.println("Hello:" + R);
    if(num == 8)
    {
      cRGB.RGBLED_Show_All(Red,Green,Blue);
    }
    else
    {
      cRGB.RGBLED_Show_All(0,0,0);
      cRGB.RGBLED_Show_Color(num,Red,Green,Blue);
    }
    
}

//鸣笛模式
void CMD_BEEP_Func(uint8_t state)
{
    if(state == 1)
      cBuzzer.setBuzzer_Tone(262, 1);
    else
      cBuzzer.setBuzzer_Tone(0, 8);
}

//弹奏音乐模式
void CMD_Music_Func()
{
    uint8_t tonenum, button;
    tonenum = InputString[3] - 0x30;
    button =  InputString[4];
    
    switch(tonenum)
    {
      case 1:
      {
        switch(button)
        {
          case '1': cBuzzer.setBuzzer_Tone(131, 1);break;
          case '2': cBuzzer.setBuzzer_Tone(147, 1);break;
          case '3': cBuzzer.setBuzzer_Tone(165, 1);break;
          case '4': cBuzzer.setBuzzer_Tone(175, 1);break;
          case '5': cBuzzer.setBuzzer_Tone(196, 1);break;
          case '6': cBuzzer.setBuzzer_Tone(220, 1);break;
          case '7': cBuzzer.setBuzzer_Tone(247, 1);break;
          case '8': cBuzzer.setBuzzer_Tone(262, 1);break;
          case 'a': cBuzzer.setBuzzer_Tone(139, 1);break;
          case 'b': cBuzzer.setBuzzer_Tone(156, 1);break;
          case 'c': cBuzzer.setBuzzer_Tone(185, 1);break;
          case 'd': cBuzzer.setBuzzer_Tone(208, 1);break;
          case 'e': cBuzzer.setBuzzer_Tone(233, 1);break; 
        }
      }break;
      case 2:
      {
        switch(button)
        {
          case '1': cBuzzer.setBuzzer_Tone(262, 1);break;
          case '2': cBuzzer.setBuzzer_Tone(294, 1);break;
          case '3': cBuzzer.setBuzzer_Tone(330, 1);break;
          case '4': cBuzzer.setBuzzer_Tone(349, 1);break;
          case '5': cBuzzer.setBuzzer_Tone(392, 1);break;
          case '6': cBuzzer.setBuzzer_Tone(440, 1);break;
          case '7': cBuzzer.setBuzzer_Tone(494, 1);break;
          case '8': cBuzzer.setBuzzer_Tone(523, 1);break;
          case 'a': cBuzzer.setBuzzer_Tone(277, 1);break;
          case 'b': cBuzzer.setBuzzer_Tone(311, 1);break;
          case 'c': cBuzzer.setBuzzer_Tone(370, 1);break;
          case 'd': cBuzzer.setBuzzer_Tone(415, 1);break;
          case 'e': cBuzzer.setBuzzer_Tone(466, 1);break; 
        }
      }break;
      case 3:
      {
        switch(button)
        {
          case '1': cBuzzer.setBuzzer_Tone(523, 1);break;
          case '2': cBuzzer.setBuzzer_Tone(587, 1);break;
          case '3': cBuzzer.setBuzzer_Tone(659, 1);break;
          case '4': cBuzzer.setBuzzer_Tone(698, 1);break;
          case '5': cBuzzer.setBuzzer_Tone(784, 1);break;
          case '6': cBuzzer.setBuzzer_Tone(880, 1);break;
          case '7': cBuzzer.setBuzzer_Tone(988, 1);break;
          case '8': cBuzzer.setBuzzer_Tone(1047, 1);break;
          case 'a': cBuzzer.setBuzzer_Tone(554, 1);break;
          case 'b': cBuzzer.setBuzzer_Tone(622, 1);break;
          case 'c': cBuzzer.setBuzzer_Tone(740, 1);break;
          case 'd': cBuzzer.setBuzzer_Tone(831, 1);break;
          case 'e': cBuzzer.setBuzzer_Tone(932, 1);break; 
        }
      }break;
    }  
}

//音乐选择模式
void CMD_Sound_Func()
{
    if(InputString[4] == 'R')
    {
         g_MusicMode = 1;
    }
    else if(InputString[4] == 'P')
    {
         g_MusicMode = 2;  
    }
    else
    {
        g_SoundsSelect = (InputString[3] - 0x30)*10 + (InputString[4] - 0x30);
        g_num = 0;
    }
     
}

//模式选择
void CMD_Mode_Func(int Mode)
{
    g_Mode = Mode;
    if(g_Mode != 8)
      Start_Music();
}
//上报模式
void CMD_ReportMode_Func(int Mode)
{
  g_ReportMode = Mode;
}
//流水灯
void Mode_WaterLED_Func()
{
    cRGB.RGBLED_Show_Color(0,255,0,0);
    delay(0.2*1000);
    cRGB.RGBLED_Show_Color(1,0,255,0);
    delay(0.2*1000);
    cRGB.RGBLED_Show_Color(2,0,0,255);
    delay(0.2*1000);
    cRGB.RGBLED_Show_Color(3,255,0,255);
    delay(0.2*1000);
    cRGB.RGBLED_Show_Color(4,0,255,255);
    delay(0.2*1000);
    cRGB.RGBLED_Show_Color(5,255,255,255);
    delay(0.2*1000);
    cRGB.RGBLED_Show_Color(6,255,255,0);
    delay(0.2*1000);
    cRGB.RGBLED_Show_Color(7,0,0,255);
    delay(0.2*1000);
    cRGB.RGBLED_Show_All(0, 0, 0);
    delay(0.2*1000);  
}

//跑马灯
void Mode_HorseLED_Func()
{
    cRGB.RGBLED_Show_Color(0,255,0,0);
    delay(0.2*1000);
    cRGB.RGBLED_Show_Color(0, 0, 0, 0);
    cRGB.RGBLED_Show_Color(1,0,255,0);
    delay(0.2*1000);
    cRGB.RGBLED_Show_Color(1, 0, 0, 0);
    cRGB.RGBLED_Show_Color(2,0,0,255);
    delay(0.2*1000);
    cRGB.RGBLED_Show_Color(2, 0, 0, 0);
    cRGB.RGBLED_Show_Color(3,255,0,255);
    delay(0.2*1000);
    cRGB.RGBLED_Show_Color(3, 0, 0, 0);
    cRGB.RGBLED_Show_Color(4,0,255,255);
    delay(0.2*1000);
    cRGB.RGBLED_Show_Color(4, 0, 0, 0);
    cRGB.RGBLED_Show_Color(5,255,255,255);
    delay(0.2*1000);
    cRGB.RGBLED_Show_Color(5, 0, 0, 0);
    cRGB.RGBLED_Show_Color(6,0,255,255);
    delay(0.2*1000);
    cRGB.RGBLED_Show_Color(6, 0, 0, 0);
    cRGB.RGBLED_Show_Color(7,255,0,0);
    delay(0.2*1000);
    cRGB.RGBLED_Show_Color(7, 0, 0, 0);
    delay(0.2*1000);
}

//变色龙
void Mode_chameleon_Func()
{
    cColor.PowerSwith(1);
    /*if(cColor.YahBoom_OMIBOX_Identify_Color(1) == true)
    {
      cRGB.RGBLED_Show_All(255,0,0);
    }
    if(cColor.YahBoom_OMIBOX_Identify_Color(2) == true)
    {
      cRGB.RGBLED_Show_All(0,255,0);
    }
    if(cColor.YahBoom_OMIBOX_Identify_Color(3) == true)
    {
      cRGB.RGBLED_Show_All(0,0,255);
    }
    if(cColor.YahBoom_OMIBOX_Identify_Color(4) == true)
    {
      cRGB.RGBLED_Show_All(255,255,255);
    }
    */
    cRGB.RGBLED_Show_All(cColor.ReturnRedValue(),cColor.ReturnGreenValue(),cColor.ReturnBlueValue());
}

void Mode_TrackLine_Func()
{
   unsigned char picBuffer[16];
   unsigned char *picTemp;
   if((cTrack.Senser_State_L(0) && cTrack.Senser_State_R(0)))
   {
      picTemp = new unsigned char[16]{
      0x38,0x1c,0x7c,0x3e,0xfe,0x7f,0xba,0x5d
      ,0x82,0x41,0x82,0x41,0x44,0x22,0x38,0x1c};
      memcpy(picBuffer,picTemp,16);
      free(picTemp);
      cMatrix.YahBoom_OMIBOX_Matrix_Show(picBuffer);
      cMotor.LeftMotor(1, 60);
      cMotor.RightMotor(1, 60);
    }
    if((cTrack.Senser_State_L(1) && cTrack.Senser_State_R(0)))
    {
      picTemp = new unsigned char[16]{
      0x38,0x1c,0x44,0x22,0x8e,0x47,0x9e,0x4f
      ,0x9e,0x4f,0x8e,0x47,0x44,0x22,0x38,0x1c};
      memcpy(picBuffer,picTemp,16);
      free(picTemp);
      cMatrix.YahBoom_OMIBOX_Matrix_Show(picBuffer);
      cMotor.LeftMotor(2, 40);
      cMotor.RightMotor(1, 60);
    }
    if((cTrack.Senser_State_L(0) && cTrack.Senser_State_R(1)))
    {
      picTemp = new unsigned char[16]{
      0x38,0x1c,0x44,0x22,0xe2,0x71,0xf2,0x79
      ,0xf2,0x79,0xe2,0x71,0x44,0x22,0x38,0x1c};
      memcpy(picBuffer,picTemp,16);
      free(picTemp);
      cMatrix.YahBoom_OMIBOX_Matrix_Show(picBuffer);
      cMotor.LeftMotor(1, 60);
      cMotor.RightMotor(2, 40);
    }
    if((cTrack.Senser_State_L(1) && cTrack.Senser_State_R(1)))
    {
      picTemp = new unsigned char[16]{
      0x0,0x0,0x40,0x2,0x60,0x6,0x30,0xc
      ,0x18,0x18,0x7c,0x3e,0x7c,0x3e,0x0,0x0};
      memcpy(picBuffer,picTemp,16);
      free(picTemp);
      cMatrix.YahBoom_OMIBOX_Matrix_Show(picBuffer);
      cMotor.LeftMotor(1, 0);
      cMotor.RightMotor(1, 0);
    }
}

void Mode_Avoid_Func()
{
  unsigned char picBuffer[16];
  unsigned char *picTemp;
  if((cIR.Senser_State_L() && cIR.Senser_State_R()))
  {
    picTemp = new unsigned char[16]{
    0x0,0x0,0xc0,0x3,0x60,0x6,0x30,0xc
    ,0x18,0x18,0xfc,0x3f,0xfc,0x3f,0x0,0x0};
    memcpy(picBuffer,picTemp,16);
    free(picTemp);
    cMatrix.YahBoom_OMIBOX_Matrix_Show(picBuffer);
    cMotor.LeftMotor(1, 70);
    cMotor.RightMotor(2, 70);
  }
  else
  {
    if(cIR.Senser_State_L())
    {
      picTemp = new unsigned char[16]{
      0x38,0x1c,0x44,0x22,0x8e,0x47,0x9e,0x4f
      ,0x9e,0x4f,0x8e,0x47,0x44,0x22,0x38,0x1c};
      memcpy(picBuffer,picTemp,16);
      free(picTemp);
      cMatrix.YahBoom_OMIBOX_Matrix_Show(picBuffer);
      cMotor.LeftMotor(1, 50);
      cMotor.RightMotor(2, 50);
    }
    else
    {
      if(cIR.Senser_State_R())
      {
        picTemp = new unsigned char[16]{
        0x38,0x1c,0x44,0x22,0xe2,0x71,0xf2,0x79
        ,0xf2,0x79,0xe2,0x71,0x44,0x22,0x38,0x1c};
        memcpy(picBuffer,picTemp,16);
        free(picTemp);
        cMatrix.YahBoom_OMIBOX_Matrix_Show(picBuffer);
        cMotor.LeftMotor(2, 50);
        cMotor.RightMotor(1, 50);
      }
      else
      {
        picTemp = new unsigned char[16]{
        0x38,0x1c,0x7c,0x3e,0xfe,0x7f,0xba,0x5d
        ,0x82,0x41,0x82,0x41,0x44,0x22,0x38,0x1c};
        memcpy(picBuffer,picTemp,16);
        free(picTemp);
        cMatrix.YahBoom_OMIBOX_Matrix_Show(picBuffer);
        cMotor.LeftMotor(1, 50);
        cMotor.RightMotor(1, 50);
      }
    }
  }
}

void Mode_Fllow_Func()
{
    unsigned char picBuffer[16];
    unsigned char *picTemp;
    if((cIR.Senser_State_L() && cIR.Senser_State_R()))
    {
      picTemp = new unsigned char[16]{
      0x0,0x0,0x0,0x0,0x6c,0x36,0xfe,0x7f
      ,0xfe,0x7f,0x7c,0x3e,0x38,0x1c,0x10,0x8};
      memcpy(picBuffer,picTemp,16);
      free(picTemp);
      cMatrix.YahBoom_OMIBOX_Matrix_Show(picBuffer);
      cMotor.LeftMotor(1, 50);
      cMotor.RightMotor(1, 50);
    }
    else
    {
      if(cIR.Senser_State_L())
      {
        picTemp = new unsigned char[16]{
        0x38,0x1c,0x44,0x22,0x8e,0x47,0x9e,0x4f
        ,0x9e,0x4f,0x8e,0x47,0x44,0x22,0x38,0x1c};
        memcpy(picBuffer,picTemp,16);
        free(picTemp);
        cMatrix.YahBoom_OMIBOX_Matrix_Show(picBuffer);
        cMotor.LeftMotor(2, 50);
        cMotor.RightMotor(1, 50);
      }
      else
      {
        if(cIR.Senser_State_R())
        {
          picTemp = new unsigned char[16]{
          0x38,0x1c,0x44,0x22,0xe2,0x71,0xf2,0x79
          ,0xf2,0x79,0xe2,0x71,0x44,0x22,0x38,0x1c};
          memcpy(picBuffer,picTemp,16);
          free(picTemp);
          cMatrix.YahBoom_OMIBOX_Matrix_Show(picBuffer);
          cMotor.LeftMotor(1, 50);
          cMotor.RightMotor(2, 50);
        }
        else
        {
          picTemp = new unsigned char[16]{
          0x38,0x1c,0x7c,0x3e,0xfe,0x7f,0xba,0x5d
          ,0x82,0x41,0x82,0x41,0x44,0x22,0x38,0x1c};
          memcpy(picBuffer,picTemp,16);
          free(picTemp);
          cMatrix.YahBoom_OMIBOX_Matrix_Show(picBuffer);
          cMotor.Stop(255, 0);
        }
      }
    }
}

void Mode_VoiceCtrl_Func()
{
  cMatrix.Static_picture(1);
  if(cVoiceSensor.get_VoiceSensor() > 400)
  {
    cBuzzer.setBuzzer_Tone(523, 2);
    cMatrix.Static_picture(8);
    cMotor.Run(100, 1000);
    cMotor.Stop(255, 0);
  }  
}

void Mode_Close_Func()
{
  g_Mode = 0;
  cMotor.Stop(255, 0);
  cBuzzer.setBuzzer_Tone(392, 2);
  cBuzzer.setBuzzer_Tone(249, 2);
  cMatrix.Move_picture(1, 0);
}

//将十六进制的字符串转换成整数  
int htoi(char s[])  
{  
    int i;  
    int n = 0;  
    if (s[0] == '0' && (s[1]=='x' || s[1]=='X'))  
    {  
        i = 2;  
    }  
    else  
    {  
        i = 0;  
    }  
    for (; (s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'z') || (s[i] >='A' && s[i] <= 'Z');++i)  
    {  
        if (tolower(s[i]) > '9')  
        {  
            n = 16 * n + (10 + tolower(s[i]) - 'a');  
        }  
        else  
        {  
            n = 16 * n + (tolower(s[i]) - '0');  
        }  
    }  
    return n;  
}  

void CMD_Matrix_Refrush_Func()
{
    char line = InputString[3];
    char hexstr[2] = {0};
    int num = 0;
    //matrixdata[] = ;
    switch(line)
    {
      case '1': 
      {
        for(int i = 0; i < 4; i++)
        {
          hexstr[0] = InputString[4 + i*2];
          hexstr[1] = InputString[4 + i*2 + 1];
          sscanf(hexstr,"%x",&num);
          matrixdata[i] = num;
        }
      }break;
      case '2':
      {
        for(int i = 0; i < 4; i++)
        {
          hexstr[0] = InputString[4 + i*2];
          hexstr[1] = InputString[4 + i*2 + 1];
          sscanf(hexstr,"%x",&num);
          matrixdata[i+4] = num;
        }
      }break;
      case '3':
      {
        for(int i = 0; i < 4; i++)
        {
          hexstr[0] = InputString[4 + i*2];
          hexstr[1] = InputString[4 + i*2 + 1];
          sscanf(hexstr,"%x",&num);
          matrixdata[i+8] = num;
        }
      }break;
      case '4': 
      {
        for(int i = 0; i < 4; i++)
        {
          hexstr[0] = InputString[4 + i*2];
          hexstr[1] = InputString[4 + i*2 + 1];
          sscanf(hexstr,"%x",&num);
          matrixdata[i+12] = num;
        }
       
        cMatrix.YahBoom_OMIBOX_Matrix_Show(matrixdata);
        memset(matrixdata, 0x00, 16);
      }break;
      
    }  
}

/**
* Function       serial_data_parse
* @author        liusen
* @date          2019.02.27
* @brief         串口数据解析并指定相应的动作
* @param[in]     void
* @param[out]    void
* @retval        void
* @par History   无
*/
void serial_data_parse()
{
  //Serial.println(InputString);
  //判断命令字
  char cmd = InputString[2];
  char state = InputString[3];
  switch(cmd)
  {
    case '1': CMD_Sport_Func(state);break;                                             //运动类
    case '2': CMD_Matrix_Show_Func();break;                                            //点阵类单独点
    case '3': CMD_Matrix_Picture_Func(state - 0x30, InputString[4] - 0x30);break;      //显示动态表情
    case '4': CMD_Servo_Func();break;                                                  //舵机控制
    case '5': CMD_RGB_Func(); break;                                                   //幻彩车灯
    case '6': CMD_BEEP_Func(state - 0x30);break;                                       //鸣笛
    case '7': CMD_Music_Func();break;                                                  //音乐弹奏
    case '8': CMD_Sound_Func(); break;                                                 //音乐选择播放和暂停
    case '9': CMD_ReportMode_Func(state - 0x30); break;                                //上报模式
    case 'A': CMD_Mode_Func(state - 0x30);break;                                       //模式选择
    case 'B': CMD_Matrix_Refrush_Func();break;                                         //点阵动态刷新 
  } 
  InputString = "";                     //清空串口数据
  NewLineReceived = false;
  return;
  
}
/**
* Function       serialEvent
* @author        liusen
* @date          2019.02.27
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
    if ( StartBit == false && IncomingByte == '$')
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

