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
地址13：IO4引脚按键控制   按键切换40Z50界面
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

void EEPROMinit(); // 初始化存储

void EE_CycleCount(uint16_t bat_per);  // eeprom: 2 判断循环次数    容量是否高于80    容量是否低于20
void EE_LcdSleep(uint8_t data);        // eeprom: 5 写入屏幕自动息屏时间
void EE_BLETimeWrite(uint16_t data);   // eeprom: 6,7  6高八位  7低八位   写入蓝牙打开的时间
uint16_t EE_BLETimeRead();             // 读取蓝牙打开的时间
void EE_ScreenDirection(uint8_t data); // eeprom: 3 写入屏幕方向
void EE_Them(uint8_t data);            // eeprom: 4 写入主题号   eeprom: 4
void EE_SmallA(uint8_t data);          // eeprom: 8 写小电流设置开关
void EE_PassWrite(uint16_t data);      // eeprom: 9,10 高八位 10低八位  写入四位的十进制数密码
uint16_t EE_PassRead();                // 读取密码的值
void EE_OTA(uint8_t data);             // eeprom: 11 OTA
void EE_IDLock(uint8_t data);          // eeprom: 12 写1关闭所有输出口
void EE_IO4();                         // eeprom: 13 IO4引脚按键控制

#endif