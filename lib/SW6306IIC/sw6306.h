#ifndef _SW6306_H_
#define _SW6306_H_
#include <iic.h>
#define SW6306_address 0x3C // 设备地址
#ifndef uint8_t
typedef unsigned char uint8_t;
#endif

#ifndef uint16_t
typedef unsigned short int uint16_t;
#endif

// define MAX_AES_BUFFER_SIZ 1
// ntc相关参数
#define BX 3435 // B值
#define T25 298.15
#define R25 10000
#ifndef uint16_t
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
#endif

// uint8_t I2C_Write(uint8_t mcuAddr, uint8_t regAddr, uint8_t senddate); // 写数据
// uint16_t I2C_Read(uint8_t mcuAddr, uint16_t regAddr);                  // 读数据
void I2C_Write_0_100(); // 0-100 寄存器写使能

// uint8_t I2C_Write_100_156(); // 100-156 寄存器写使能
// uint16_t ADC_Data();         // SW6306_ADC

float SYS_V();           // 系统电压
float SYS_A();           // 系统 输入/输出 电流
float Battery_V();       // 电池电压
float Battery_A();       // 电池 输入/输出 电流
float IC_Temp();         // 芯片温度
float NTC_Temp();        // NTC温度
float Battery_Volume();  // 电池容量
uint8_t Battery_Per();   // 电池电量百分比
uint8_t SYS_State();     // 充放电状态   1 放电   2 充电
uint8_t AC_State();      // 端口在线状态 C2是L口   0:空闲   1:C2   2:C1   3:C1C2   4:A2   5:A2C2   6:A2C1   7:A2C1C2   8:A1   9:A1C2   A:A1C1   B:A1C1C2   C:A1A2   D:A1A2C2   E:A1A2C1   F:A1A2C1C2
uint8_t Small_A_State(); // 小电流状态   0: 关    1: 开
void Small_A_ON();
void Small_A_OFF();

//  void sysstate(uint8_t *H_value, uint8_t *L_value, float *battery_A); // 系统充放电L口和A1口的状态
//  uint8_t batzhuangtai();                                              // 电池状态

// REG0x24: I2C使能  操作0x100及以上寄存器使能

// /**
//  * @brief 小电流开关用到的函数
//  */
// void kqxdl();     // 开启小电流
// void I2Csmall_A_ON(); // 小电流使能
// void I2Csmall_A_OFF(); // 小电流不使能
// uint8_t xdlzt();  // 小电流状态
// void A2_ON();  // A2口插入
// void A2_OFF(); // A2口拔出

// void A1shijian(); // 触发A1口的插入

// void AC_OFF(); // 关闭所有输出口  bit 自动清零

// void NTCLimit();//设置NTC温度上限保护60℃ 改为 65℃     温度每上升1度，电压下降800mv

// void L_State(); // 控制L口输入功率30w，关闭输出

void SW6306init(); // sw6306初始化
void cuowu();
#endif