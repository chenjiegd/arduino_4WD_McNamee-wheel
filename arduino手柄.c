#include <Wire.h>
#include <SoftwareSerial.h>
#include "MeOrion.h"

MeUSBHost joypad(PORT_3);
//	手柄代码（红灯亮模式）
//	默认：128-127-128-127-15-0-0-128
//	左一：128-127-128-127-15-1-0-128
//	右一：128-127-128-127-15-2-0-128
//	左二：128-127-128-127-15-4-0-128
//	右二：128-127-128-127-15-8-0-128
//	三角：128-127-128-127-31-0-0-128 （0001 1111）
//	方形：128-127-128-127-143-0-0-128 （1000 1111）
//	叉号：128-127-128-127-79-0-0-128 （0100 1111）
//	圆圈：128-127-128-127-47-0-0-128 （0010 1111）
//	向上：128-127-128-127-0-0-0-128 （0000 0000）
//	向下：128-127-128-127-4-0-0-128 （0000 0100）
//	向左：128-127-128-127-6-0-0-128 （0000 0110）
//	向右：128-127-128-127-2-0-0-128 （0000 0010）
//	左上：128-127-128-127-7-0-0-128 （0000 0111）
//	左下：128-127-128-127-5-0-0-128 （0000 0101）
//	右上：128-127-128-127-1-0-0-128 （0000 0001）
//	右下：128-127-128-127-3-0-0-128 （0000 0011）
//	选择：128-127-128-127-15-16-0-128
//	开始：128-127-128-127-15-32-0-128
//	摇杆：右X-右Y-左X-左Y-15-0-0-128


MeEncoderMotor motor1(0x02, SLOT2);
MeEncoderMotor motor2(0x02, SLOT1);
MeEncoderMotor motor3(0x0A, SLOT2);
MeEncoderMotor motor4(0x0A, SLOT1);

//  底盘：a = 130mm, b = 120mm

float linearSpeed = 100;
float angularSpeed = 100;
float maxLinearSpeed = 200;
float maxAngularSpeed = 200;
float minLinearSpeed = 30;
float minAngularSpeed = 30;

void setup()
{
    //  要上电才能工作，不能只是插上 USB 线来调试。
	motor1.begin();
	motor2.begin();
	motor3.begin();
	motor4.begin();
	
	Serial.begin(57600);
	joypad.init(USB1_0);
}

void loop()
{
    Serial.println("loop:");
	//setEachMotorSpeed(100, 50, 50, 100);
	if(!joypad.device_online)
    {
        //  若一直输出离线状态，重新拔插 USB Host 的 RJ25 线试一下。
        Serial.println("Device offline.");
        joypad.probeDevice();
        delay(1000);
    }
    else
    {
        int len = joypad.host_recv();
        parseJoystick(joypad.RECV_BUFFER);
        delay(5);
    }
	//delay(500);
}


void setEachMotorSpeed(float speed1, float speed2, float speed3, float speed4)
{
	motor1.runSpeed(speed1);
	motor2.runSpeed(-speed2);
	motor3.runSpeed(-speed3);
	motor4.runSpeed(-speed4);
}

void parseJoystick(unsigned char *buf)   //Analytic function, print 8 bytes from USB Host
{
    //  输出手柄的数据，调试用
    // int i = 0;
    // for(i = 0; i < 7; i++)
    // {
    //     Serial.print(buf[i]);  //It won't work if you connect to the Makeblock Orion.
    //     Serial.print('-');
    // }
    // Serial.println(buf[7]);
    // delay(10);

    //  速度增减
    switch (buf[5])
    {
        case 1:
            linearSpeed += 5;
            if (linearSpeed > maxLinearSpeed)
            {
                linearSpeed = maxLinearSpeed;
            }
            break;
        case 2:
            angularSpeed += 5;
            if (angularSpeed > maxAngularSpeed)
            {
                angularSpeed = maxAngularSpeed;
            }
            break;
        case 4:
            linearSpeed -= 5;
            if (linearSpeed < minLinearSpeed)
            {
                linearSpeed = minLinearSpeed;
            }
            break;
        case 8:
            angularSpeed -= 5;
            if (angularSpeed < minAngularSpeed)
            {
                angularSpeed = minAngularSpeed;
            }
            break;
        default:
            break;
    }


    //	向上：128-127-128-127-0-0-0-128 （0000 0000）
    //	向下：128-127-128-127-4-0-0-128 （0000 0100）
    //	向左：128-127-128-127-6-0-0-128 （0000 0110）
    //	向右：128-127-128-127-2-0-0-128 （0000 0010）
    //	左上：128-127-128-127-7-0-0-128 （0000 0111）
    //	左下：128-127-128-127-5-0-0-128 （0000 0101）
    //	右上：128-127-128-127-1-0-0-128 （0000 0001）
    //	右下：128-127-128-127-3-0-0-128 （0000 0011）
    //	摇杆：右X-右Y-左X-左Y-15-0-0-128
    if ((128 != buf[0]) || (127 != buf[1]) || (128 != buf[2]) || (127 != buf[3]))
    {
        //  处理摇杆
        float x = ((float)(buf[2]) - 127) / 128;
        float y = (127 - (float)(buf[3])) / 128;
        float a = (127 - (float)(buf[0])) / 128;
        mecanumRun(x * linearSpeed, y * linearSpeed, a * angularSpeed);
    }
    else
    {
        switch (buf[4])
        {
            case 0:
                mecanumRun(0, linearSpeed, 0);
                break;
            case 4:
                mecanumRun(0, -linearSpeed, 0);
                break;
            case 6:
                mecanumRun(-linearSpeed, 0, 0);
                break;
            case 2:
                mecanumRun(linearSpeed, 0, 0);
                break;
            case 7:
                mecanumRun(-linearSpeed/2, linearSpeed/2, 0);
                break;
            case 5:
                mecanumRun(-linearSpeed/2, -linearSpeed/2, 0);
                break;
            case 1:
                mecanumRun(linearSpeed/2, linearSpeed/2, 0);
                break;
            case 3:
                mecanumRun(linearSpeed/2,  -linearSpeed/2, 0);
                break;
            default:
                mecanumRun(0, 0, 0);
                break;
        }
    }
}

void mecanumRun(float xSpeed, float ySpeed, float aSpeed)
{
    float speed1 = ySpeed - xSpeed + aSpeed; 
    float speed2 = ySpeed + xSpeed - aSpeed;
    float speed3 = ySpeed - xSpeed - aSpeed;
    float speed4 = ySpeed + xSpeed + aSpeed;
    
    float max = speed1;
    if (max < speed2)   max = speed2;
    if (max < speed3)   max = speed3;
    if (max < speed4)   max = speed4;
    
    if (max > maxLinearSpeed)
    {
        speed1 = speed1 / max * maxLinearSpeed;
        speed2 = speed2 / max * maxLinearSpeed;
        speed3 = speed3 / max * maxLinearSpeed;
        speed4 = speed4 / max * maxLinearSpeed;
    }
    
    setEachMotorSpeed(speed1, speed2, speed3, speed4);
}