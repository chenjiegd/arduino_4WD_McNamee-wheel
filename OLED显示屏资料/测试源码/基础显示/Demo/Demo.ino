
#include <Wire.h>//申明OLED 12864的函数库
#include <Adafruit_GFX.h>//申明OLED 12864的函数库
#include <Adafruit_SSD1306.h>//申明OLED 12864的函数库

#define OLED_DC 11 //定义OLED 12864的DC引脚连接arduino11引脚
#define OLED_CS 12//定义OLED 12864的CS引脚连接arduino12引脚
#define OLED_CLK 10//定义OLED 12864的D0(CLK)引脚连接arduino10引脚
#define OLED_MOSI 9//定义OLED 12864的D1(MOST)引脚连接arduino9引脚
#define OLED_RESET 13//定义OLED 12864的RES引脚连接arduino13引脚
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

void setup()  
{   
while(1)
  {    
      display.begin(SSD1306_SWITCHCAPVCC);//初始化OLED 12864开始工作
      display.clearDisplay();   // 清屏
      display.setTextSize(2);   //字体尺寸大小2
      display.setTextColor(WHITE);//字体颜色为白色
      display.setCursor(0,0); //把光标定位在第0行，第0列
      display.println(" yahboom");//显示字符
      display.setTextColor( WHITE); //字体颜色为白色 
      display.println(123456789);//显示数字
      display.setTextSize(2);//字体尺寸大小2
      display.setTextColor(WHITE);//字体颜色为白色
      display.print("0x"); display.println(0xDEADBEEF, HEX);//显示16进制字符
      display.display();//显示
      delay(2000);//延时两秒
  }
}
void loop()
{ }//初始化已完成显示，主循环无动作
