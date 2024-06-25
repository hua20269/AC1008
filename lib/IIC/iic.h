#ifndef _IIC_H
#define _IIC_H
#include <Wire.h>
#include <Arduino.h>
#define SDA_PIN 26          // SDA引脚
#define SCL_PIN 25          // SCL引脚
#define IRQ_PIN 27          // IRQ引脚
#define CK22AT_address 0x1C // 设别地址

#ifndef uint8_t
typedef unsigned char uint8_t;
#endif

#ifndef uint16_t
typedef unsigned short int uint16_t;
#endif


void IICinit();
uint8_t I2C_Write(uint8_t mcuAddr, uint8_t regAddr, uint8_t senddate); // 写数据
uint8_t I2C_Read(uint8_t mcuAddr, uint8_t regAddr);                    // 读数据

#endif