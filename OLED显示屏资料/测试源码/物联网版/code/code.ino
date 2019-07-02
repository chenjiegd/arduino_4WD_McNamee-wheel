
#include <Wire.h>//申明OLED 12864的函数库
#include <Adafruit_GFX.h>//申明OLED 12864的函数库
#include <Adafruit_SSD1306.h>//申明OLED 12864的函数库

#define OLED_DC 11 //定义OLED 12864的DC引脚连接arduino11引脚
#define OLED_CS 12//定义OLED 12864的CS引脚连接arduino12引脚
#define OLED_CLK 10//定义OLED 12864的D0(CLK)引脚连接arduino10引脚
#define OLED_MOSI 9//定义OLED 12864的D1(MOST)引脚连接arduino9引脚
#define OLED_RESET 13//定义OLED 12864的RES引脚连接arduino13引脚
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);


/*串口协议相关*/
int incomingByte = 0;                    // 接收到的 data byte
String inputString = "";                 // 用来储存接收到的内容
boolean newLineReceived = false;         // 前一次数据结束标志
boolean startBit  = false;               //协议开始标志

String returntemp = "";           //存储返回值 


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

void setup()  
{   
  Serial.begin(9600);	      //波特率9600 （WIFI通讯设定波特率）  
  printf_begin();
  display.begin(SSD1306_SWITCHCAPVCC);//初始化OLED 12864开始工作
  display.clearDisplay();   // 清屏
  
  display.setTextSize(2);   //字体尺寸大小2
  display.setTextColor(WHITE);//字体颜色为白色
  display.setCursor(0,0); //把光标定位在第0行，第0列
  display.print(" Yahboom");//显示字符
  display.setCursor(0,16); 
  display.print(1234567890);//显示数字
  display.setCursor(0,32); 
  display.print("Waitting!"); 
  display.setCursor(0,48); 
  display.print("Wellcom!"); 
  display.display();//显示
}

void loop()
{ 
  
  while (newLineReceived)
  {
       //$OLED,1,2,3,4#
      if(inputString.indexOf("OLED") == -1)
      {
           returntemp = "$OLED,2,#";
           Serial.print(returntemp); //返回协议数据包       
           inputString = "";   // clear the string
           newLineReceived = false;
           break;    
      }
      //1
      display.clearDisplay();   // 清屏
      int i = inputString.indexOf(",");   //从接收到的数据中检索字符串“L1”出现的位置
      int ii = inputString.indexOf(",", i+1); //从接收到的数据中以i为起始位置检索字符串“，”的位置
      if(ii > i && ii > 0 && i > 0 )   //如果ii和i的顺序对了并且检索到ii与i存在
      {
        String sLINE1 = inputString.substring(i + 1, ii);//提取字符串中介于指定下标i+1到ii之间的字符赋值给sLINE1
        display.setTextSize(2);   //字体尺寸大小2
        display.setTextColor(WHITE);//字体颜色为白色
        display.setCursor(0,0); //把光标定位在第0行，第0列
        display.print(sLINE1);       
        display.display();//显示 
      }
      //2
      i = ii; 
      ii = inputString.indexOf(",", i+1); //从接收到的数据中以i为起始位置检索字符串“，”的位置
      if(ii > i && ii > 0 && i > 0 )   //如果ii和i的顺序对了并且检索到ii与i存在
      {
        String sLINE2 = inputString.substring(i + 1, ii);//提取字符串中介于指定下标i+1到ii之间的字符赋值给sLINE1
        display.setTextSize(2);   //字体尺寸大小2
        display.setTextColor(WHITE);//字体颜色为白色
        display.setCursor(0,16); //把光标定位在第0行，第0列
        display.print(sLINE2); 
        display.display();//显示
      }
      //3
      i = ii; 
      ii = inputString.indexOf(",", i+1); //从接收到的数据中以i为起始位置检索字符串“，”的位置
      if(ii > i && ii > 0 && i > 0 )   //如果ii和i的顺序对了并且检索到ii与i存在
      {
        String sLINE3 = inputString.substring(i + 1, ii);//提取字符串中介于指定下标i+1到ii之间的字符赋值给sLINE1
        display.setTextSize(2);   //字体尺寸大小2
        display.setTextColor(WHITE);//字体颜色为白色
        display.setCursor(0,32); //把光标定位在第0行，第0列
        display.print(sLINE3);
        display.display();//显示 

      }
      //4
      i = ii; 
      ii = inputString.indexOf("#", i+1); //从接收到的数据中以i为起始位置检索字符串“，”的位置
      if(ii > i && ii > 0 && i > 0 )   //如果ii和i的顺序对了并且检索到ii与i存在
      {
        String sLINE4 = inputString.substring(i + 1, ii);//提取字符串中介于指定下标i+1到ii之间的字符赋值给sLINE1
        display.setTextSize(2);   //字体尺寸大小2
        display.setTextColor(WHITE);//字体颜色为白色
        display.setCursor(0,48); //把光标定位在第0行，第0列
        display.print(sLINE4); 
        display.display();//显示
      } 
      //display.display();//显示
      returntemp = "$OLED,0,#";
      Serial.print(returntemp); //返回协议数据包       
      inputString = "";   // clear the string
      newLineReceived = false;  
   }
}

//serialEvent()是IDE1.0及以后版本新增的功能，不清楚为什么大部份人不愿意用，这个可是相当于中断功能一样的啊!  
void serialEvent()
{
  while (Serial.available()) 
  {    
    incomingByte = Serial.read();              //一个字节一个字节地读，下一句是读到的放入字符串数组中组成一个完成的数据包
    if(incomingByte == '$')
    {
      startBit= true;
    }
    if(startBit == true)
    {
       inputString += (char) incomingByte;     // 全双工串口可以不用在下面加延时，半双工则要加的//
    }  
    if (incomingByte == '#') 
    {
       newLineReceived = true; 
       startBit = false;
    }
  }
}
