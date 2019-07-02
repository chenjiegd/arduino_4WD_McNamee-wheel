

#include <Wire.h>//申明OLED 12864的函数库
#include <Adafruit_GFX.h>//申明OLED 12864的函数库
#include <Adafruit_SSD1306.h>//申明OLED 12864的函数库

#define OLED_DC 11//定义OLED 12864的DC引脚连接arduino11引脚
#define OLED_CS 12//定义OLED 12864的CS引脚连接arduino12引脚
#define OLED_CLK 10//定义OLED 12864的D0(CLK)引脚连接arduino10引脚
#define OLED_MOSI 9//定义OLED 12864的D1(MOST)引脚连接arduino9引脚
#define OLED_RESET 13//定义OLED 12864的RES引脚连接arduino13引脚
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16 //定义长度为16
#define LOGO16_GLCD_WIDTH  16 //定义宽度为16
const static unsigned char PROGMEM logo16_glcd_bmp[]=//取模后得到的微型图标数组
{
  B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 
};

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

void setup()   
{                
  display.begin(SSD1306_SWITCHCAPVCC);//初始化OLED 12864开始工作
  display.display(); // 闪烁屏幕
  delay(2000);//延时两秒
  display.clearDisplay();   //清屏
  display.drawPixel(10, 10, WHITE);//显示像素是10*10大小，白光
  display.display();// 闪烁屏幕
  delay(2000);//延时两秒
  display.clearDisplay();//清屏
  
  testdrawline();//调用直线的动画函数
  display.display();//显示线的动画
  delay(2000);//延时两秒
  display.clearDisplay();  //清屏
  
  testdrawrect();//调用矩形的函数
  display.display();//显示矩形
  delay(2000);//延时两秒
  display.clearDisplay();//清屏

  testfillrect();//调用填充矩矩形动画函数
  display.display();//显示填充矩矩形
  delay(2000);//延时两秒
  display.clearDisplay();//清屏

  testdrawcircle();//调用圆形的函数
  display.display();//显示圆形
  delay(2000);//延时两秒
  display.clearDisplay();//清屏

  display.fillCircle(display.width()/2, display.height()/2, 10, WHITE);//调用填充圆形动画函数
  display.display();//显示填充圆形动画
  delay(2000);//延时两秒
  display.clearDisplay();//清屏

  testdrawroundrect();//调用圆矩形的函数
  delay(2000);//延时两秒
  display.clearDisplay();//清屏

  testfillroundrect();//调用圆矩形的函数
  delay(2000);//延时两秒
  display.clearDisplay();//清屏

  testdrawtriangle();//调用圆矩形的函数
  delay(2000);//延时两秒
  display.clearDisplay();//清屏
   
  testfilltriangle();//调用圆矩形的函数
  delay(2000);//延时两秒
  display.clearDisplay();//清屏

  testdrawchar();//调用字符的函数
  display.display();//显示字符
  delay(2000);//延时两秒
  display.clearDisplay();//清屏

  testscrolltext();//调用卷动的函数
  delay(2000);//延时两秒
  display.clearDisplay();//清屏
  
  display.setTextSize(1);//字体尺寸大1
  display.setTextColor(WHITE);//字体颜色为白色
  display.setCursor(0,0); //把光标定位在第0行，第0列
  display.println("Hello, world!"); //显示字符
  display.setTextColor(BLACK, WHITE); // 背光
  display.println(3.141592);//显示数字
  display.setTextSize(2);//字体尺寸大小2
  display.setTextColor(WHITE);//字体颜色为白色
  display.print("0x"); display.println(0xDEADBEEF, HEX);//显示16进制字符
  display.display();//显示
  delay(2000);//延时两秒

  display.clearDisplay();//清屏
  display.drawBitmap(30, 16,  logo16_glcd_bmp, 16, 16, 1);//微型图位印象显示，logo16_glcd_bmp表示前文定义的数组，30表示数组个数，长16，宽16，尺寸1
  display.display();//显示

  display.invertDisplay(true);
  delay(1000); 
  display.invertDisplay(false);//反向显示
  delay(1000); 

  testdrawbitmap(logo16_glcd_bmp, LOGO16_GLCD_HEIGHT, LOGO16_GLCD_WIDTH);//显示一个微型图标并‘有生命’的移动
}


void loop() {}//初始化已完成显示，主循环无动作


void testdrawbitmap(const uint8_t *bitmap, uint8_t w, uint8_t h) //微型图标函数
{
  uint8_t icons[NUMFLAKES][3];
  srandom(666);     // whatever seed
 
  // initialize
  for (uint8_t f=0; f< NUMFLAKES; f++)
  {
    icons[f][XPOS] = random() % display.width();
    icons[f][YPOS] = 0;
    icons[f][DELTAY] = random() % 5 + 1;
    
    Serial.print("x: ");
    Serial.print(icons[f][XPOS], DEC);
    Serial.print(" y: ");
    Serial.print(icons[f][YPOS], DEC);
    Serial.print(" dy: ");
    Serial.println(icons[f][DELTAY], DEC);
  }

  while (1)
  {
    // draw each icon
    for (uint8_t f=0; f< NUMFLAKES; f++) 
    {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], logo16_glcd_bmp, w, h, WHITE);
    }
    display.display();
    delay(200);
    
    // then erase it + move it
    for (uint8_t f=0; f< NUMFLAKES; f++)
    {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS],  logo16_glcd_bmp, w, h, BLACK);
      // move it
      icons[f][YPOS] += icons[f][DELTAY];
      // if its gone, reinit
      if (icons[f][YPOS] > display.height())
      {
	icons[f][XPOS] = random() % display.width();
	icons[f][YPOS] = 0;
	icons[f][DELTAY] = random() % 5 + 1;
      }
    }
   }
}


void testdrawchar(void)//字符函数
{
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);

  for (uint8_t i=0; i < 168; i++) 
  {
    if (i == '\n') continue;
    display.write(i);
    if ((i > 0) && (i % 21 == 0))
      display.println();
  }    
  display.display();
}

void testdrawcircle(void) //圆函数
{
  for (int16_t i=0; i<display.height(); i+=2) 
  {
    display.drawCircle(display.width()/2, display.height()/2, i, WHITE);
    display.display();
  }
}

void testfillrect(void)//填充矩形函数
{
  uint8_t color = 1;
  for (int16_t i=0; i<display.height()/2; i+=3) 
  {
    // alternate colors
    display.fillRect(i, i, display.width()-i*2, display.height()-i*2, color%2);
    display.display();
    color++;
  }
}

void testdrawtriangle(void) //三角形函数
{
  for (int16_t i=0; i<min(display.width(),display.height())/2; i+=5) 
  {
    display.drawTriangle(display.width()/2, display.height()/2-i,
                     display.width()/2-i, display.height()/2+i,
                     display.width()/2+i, display.height()/2+i, WHITE);
    display.display();
  }
}

void testfilltriangle(void)//填充三角形函数
{
  uint8_t color = WHITE;
  for (int16_t i=min(display.width(),display.height())/2; i>0; i-=5)
  {
    display.fillTriangle(display.width()/2, display.height()/2-i,
                     display.width()/2-i, display.height()/2+i,
                     display.width()/2+i, display.height()/2+i, WHITE);
    if (color == WHITE) color = BLACK;
    else color = WHITE;
    display.display();
  }
}

void testdrawroundrect(void) //圆矩形函数
{
  for (int16_t i=0; i<display.height()/2-2; i+=2)
  {
    display.drawRoundRect(i, i, display.width()-2*i, display.height()-2*i, display.height()/4, WHITE);
    display.display();
  }
}

void testfillroundrect(void) //填充圆矩形函数
{
  uint8_t color = WHITE;
  for (int16_t i=0; i<display.height()/2-2; i+=2) 
  {
    display.fillRoundRect(i, i, display.width()-2*i, display.height()-2*i, display.height()/4, color);
    if (color == WHITE) color = BLACK;
    else color = WHITE;
    display.display();
  }
}
   
void testdrawrect(void) //矩形函数
{
  for (int16_t i=0; i<display.height()/2; i+=2)
  {
    display.drawRect(i, i, display.width()-2*i, display.height()-2*i, WHITE);
    display.display();
  }
}

void testdrawline()//直线函数
{  
  for (int16_t i=0; i<display.width(); i+=4) 
  {
    display.drawLine(0, 0, i, display.height()-1, WHITE);
    display.display();
  }
  for (int16_t i=0; i<display.height(); i+=4)
  {
    display.drawLine(0, 0, display.width()-1, i, WHITE);
    display.display();
  }
  delay(250);
  
  display.clearDisplay();
  for (int16_t i=0; i<display.width(); i+=4) 
  {
    display.drawLine(0, display.height()-1, i, 0, WHITE);
    display.display();
  }
  for (int16_t i=display.height()-1; i>=0; i-=4) 
  {
    display.drawLine(0, display.height()-1, display.width()-1, i, WHITE);
    display.display();
  }
  delay(250);
  
  display.clearDisplay();
  for (int16_t i=display.width()-1; i>=0; i-=4) 
  {
    display.drawLine(display.width()-1, display.height()-1, i, 0, WHITE);
    display.display();
  }
  for (int16_t i=display.height()-1; i>=0; i-=4) 
  {
    display.drawLine(display.width()-1, display.height()-1, 0, i, WHITE);
    display.display();
  }
  delay(250);

  display.clearDisplay();
  for (int16_t i=0; i<display.height(); i+=4) 
  {
    display.drawLine(display.width()-1, 0, 0, i, WHITE);
    display.display();
  }
  for (int16_t i=0; i<display.width(); i+=4) 
  {
    display.drawLine(display.width()-1, 0, i, display.height()-1, WHITE); 
    display.display();
  }
  delay(250);
}

void testscrolltext(void) //卷动函数
{
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10,0);
  display.clearDisplay();
  display.println("scroll");
  display.display();
 
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);    
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
}

