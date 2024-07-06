#ifndef _IIC_H
#define _IIC_H
#include <Wire.h>
#include <Arduino.h>
#define SDA_PIN 26          // SDA引脚
#define SCL_PIN 25          // SCL引脚
#define IRQ_PIN 27          // IRQ引脚   sw6306
#define CK22AT_address 0x1C // 设别地址  加密芯片地址




void IICinit();


#endif