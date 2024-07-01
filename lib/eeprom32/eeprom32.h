#ifndef _EEPROM32_H
#define _EEPROM32_H
#include <EEPROM.h>
#define EEPROM_SIZE 30

/*
eeprom可用地址为0~4096
以下地址和对应的值不可随意变动，若想使用eeprom，则另外开辟新的储存地址的值 每一个地址储存的值为0~255，大于此数值 不能存储，切记
地址0：被用作判断电池的容量是否高于80
地址1：被用作判断电池的容量是否低于20
地址2：记录电池的循环次数

地址3：记录屏幕方向

地址4：记录屏幕主题编号

地址5：记录屏幕自动息屏时间

地址6：记录蓝牙休眠时间的高八位
地址7：记录蓝牙休眠时间的低八位

地址8：记录小电流状态

地址9：记录密码的高八位
地址10：记录密码的低八位

地址11：写1 OTA升级

地址12：写1 锁放电，关闭所有输出口
地址13：
地址14：
地址15：
地址16：
地址17：
地址18：
地址19：

地址20：
地址21：
地址22：
地址23：
地址24：
地址25：
地址26：
地址27：


*/

void eeprominit(); // 初始化存储

void eebat_circ(uint16_t bat_per); // 判断循环次数  eeprom: 2   容量是否高于80    容量是否低于20

void eepromLcdSleep(uint8_t data); // 写入屏幕自动息屏时间   eeprom: 5

void eepromwrite(uint16_t data); // 写入蓝牙打开的时间   eeprom: 6, 7
uint16_t eepromread();           // 读取蓝牙打开的时间

void eepromRotationWrite(uint8_t data); // 写入屏幕方向   eeprom: 3

void eepromThemWrite(uint8_t data); // 写入主题号   eeprom: 4

void eepromsmallaWrite(uint8_t data); // 写小电流设置开关   eeprom: 8

void eepromwritepass(uint16_t data); // 写入四位的十进制数密码到eeprom的9和10地址  地址9为高八位 地址10为低八位
uint16_t eepromreadpass();           // 读取密码的值

void eepromOTA(uint8_t data); // OTA    eeprom: 11

void eepromIDLock(uint8_t data); // 写1 锁死 // 废掉ESP32  或关闭所有输出口    eeprom: 12

#endif