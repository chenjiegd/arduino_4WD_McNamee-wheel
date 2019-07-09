#define wusicaijuan chenshuaiqi

#include <Adafruit_NeoPixel.h>
#include <Wire.h>
void RGB_Config(uint8_t RGB);

#define PIN 6
#define MAX_LED 1

#define ADD true
#define SUB false

int val = 0;
boolean stat = ADD;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(MAX_LED, PIN, NEO_RGB + NEO_KHZ800);

//Device Address
//ADDR->Low
#define RGB_2 0x00
#define ADDR 0x53
#define W_ADDR_L 0x66
#define R_ADDR_L 0x67
//ADDR->High
#define RGB_1 0x01
#define W_ADDR_H 0x98
#define R_ADDR_H 0x99

//Register Address
#define ControlReg_Addr 0x00
#define RGBCON_Addr 0x04

#define ID_Addr 0x06

#define CDATAL_Addr 0x12
#define CDATAH_Addr 0x13

#define RDATAL_Addr 0x10
#define RDATAM_Addr 0x11
#define RDATAH_Addr 0x12

#define GDATAL_Addr 0x0D
#define GDATAM_Addr 0x0E
#define GDATAH_Addr 0x0F

#define BDATAL_Addr 0x13
#define BDATAM_Addr 0x14
#define BDATAH_Addr 0x15

/*  Confi Arg  */
//Control Register
#define RST 0x00

const uint8_t i = 0;
uint32_t color;

unsigned int Red = 0;
unsigned int Green = 0;
unsigned int Blue = 0;

unsigned int val_red = 0;
unsigned int val_green = 0;
unsigned int val_blue = 0;

/**
  * @brief  RGB配置
  * @param  
  * @arg     RGB:RGB传感器选择
  * @retval 无
  */
void RGB_Config(void)
{
	// uint8_t W_ADDR;

	//复位寄存器，200ms积分时间，CRGB通道全部开启
	Wire.beginTransmission(ADDR); //发送Device地址
	Wire.write(ControlReg_Addr);
	Wire.write(0x06);
	Wire.endTransmission(); //发送结速信号

	Wire.beginTransmission(ADDR); //发送Device地址
	Wire.write(0x04);
	Wire.write(0x41);		//16bit 50ms
	Wire.endTransmission(); //发送结速信号

	Wire.beginTransmission(ADDR); //发送Device地址
	Wire.write(0x05);
	Wire.write(0x01);
	Wire.endTransmission(); //发送结速信号
}

void ReadColor() //传感器获取颜色
{
	int index = 0;
	char ColorData[6] = {0};
	//红色
	Wire.beginTransmission(ADDR); //发送Device地址
	Wire.write(0x10);
	Wire.endTransmission();		  //发送结束信号
	Wire.beginTransmission(ADDR); //发送Device地址
	Wire.requestFrom(ADDR, 2);	// request 1 bytes from slave device #2
	while (Wire.available())	  // slave may send less than requested
	{
		char ff = Wire.read(); // receive a byte as character
		ColorData[index] = ff;
		index++;
		//printf("RED:%x\n", ff);
	}
	Wire.endTransmission(); //发送结束信号

	//绿色
	Wire.beginTransmission(ADDR); //发送Device地址
	Wire.write(0x0D);
	Wire.endTransmission();		  //发送结束信号
	Wire.beginTransmission(ADDR); //发送Device地址
	Wire.requestFrom(ADDR, 2);	// request 1 bytes from slave device #2
	while (Wire.available())	  // slave may send less than requested
	{
		char ff = Wire.read(); // receive a byte as character
		ColorData[index] = ff;
		index++;
		//printf("Green:%x\n", ff);
	}
	Wire.endTransmission(); //发送结束信号

	//蓝色
	Wire.beginTransmission(ADDR); //发送Device地址
	Wire.write(0x13);
	Wire.endTransmission();		  //发送结束信号
	Wire.beginTransmission(ADDR); //发送Device地址
	Wire.requestFrom(ADDR, 2);	// request 1 bytes from slave device #2
	while (Wire.available())	  // slave may send less than requested
	{
		char ff = Wire.read(); // receive a byte as character
		ColorData[index] = ff;
		index++;
		//printf("Blue:%x\n", ff);
	}
	Wire.endTransmission();													  //发送结束信号
	Red = ((unsigned int)(ColorData[1] & 0xff) << 8 | (ColorData[0] & 0xff)); //2.06
	Green = ((unsigned int)(ColorData[3] & 0xff) << 8 | (ColorData[2] & 0xff));
	Blue = ((unsigned int)(ColorData[5] & 0xff) << 8 | (ColorData[4] & 0xff));
	printf("R:%d, G:%d, B:%d\n", Red, Green, Blue); //串口打印数据
	if (Red > 1822)
		Red = 1822; //4500.7600.4600分别是检测到纯白色纸张时，所检测到的校准值，如果颜色识别不准确可以修改这里的校准值。
	if (Green > 3482)
		Green = 3482;
	if (Blue > 2665)
		Blue = 2665;
	val_red = map(Red, 0, 1822, 0, 255);
	val_green = map(Green, 0, 3482, 0, 255);
	val_blue = map(Blue, 0, 2665, 0, 255);

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
	else if (val_blue > val_red && val_blue > val_green)
	{
		val_blue = 255;
		val_red /= 2;
		val_green /= 2;
	}

	printf("val_red:%d, val_green:%d, val_blue:%d\n", val_red, val_green, val_blue); //串口打印数据
}

/*printf格式化字符串初始化*/
int serial_putc(char c, struct __file *)
{
	Serial.write(c);
	return c;
}
void printf_begin(void)
{
	fdevopen(&serial_putc, 0);
}

void setup()
{
	// put your setup code here, to run once:
	Wire.begin();
	Wire.setClock(100000);
	Serial.begin(9600); //串口波特率设置，打印数据时串口需要选择和这里一样的波特率

	printf_begin();
	Serial.println("\nI2C Scanner");
	RGB_Config();

	strip.begin(); //初始化Adafruit_NeoPixel；
	strip.show();  //显示所有LED为关状态;
}

void loop()
{
	// put your main code here, to run repeatedly:
	// ReadColor();
	// uint32_t color = strip.Color(val_green, val_red, val_blue); // 绿红蓝
	ReadColor();
	val_green = map(val_green, 0, 255, 0, 10);
	val_red = map(val_red, 0, 255, 0, 10);
	val_blue = map(val_blue, 0, 255, 0, 10);
	color = strip.Color(val_green, val_red, val_blue); // 绿红蓝
	strip.setPixelColor(i, color);
	strip.show();
}

void serialEvent()
{
	while (Serial.available())
	{
		//一个字节一个字节地读,下一句是读到的放入字符串数组中组成一个完成的数据包
		char IncomingByte = Serial.read();
		Serial.print(IncomingByte);
	}
}
