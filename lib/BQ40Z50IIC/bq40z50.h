#ifndef _BQ40Z50_H
#define _BQ40Z50_H
#include <iic.h>

#define BQ40z50address 0x0B

void BatteryMode(); //设置电池型号
uint16_t BQ_Temperature();  // 电池 温度
uint16_t BQ_Voltage();      // 电池包 总电压
int16_t BQ_Current();       // 电池 电流大小
uint16_t BQ_CellVoltage4(); // 电压包 电池4电压
uint16_t BQ_CellVoltage3(); // 电压包 电池3电压
uint16_t BQ_CellVoltage2(); // 电压包 电池2电压
uint16_t BQ_CellVoltage1(); // 电压包 电池1电压

uint16_t BQ_AtRateTimeToFull();  // 电池组充满电的剩余时间。    //不走数  65535
uint16_t BQ_AtRateTimeToEmpty(); // 电池组完全放电的剩余时间。   //不走数  65535

uint8_t BQ_RelativeStateOfCharge(); // 电池容量百分比 （根据充满电预测最大容量）
uint8_t BQ_AbsoluteStateOfCharge(); // 电池容量百分比  （系统预测） Test会超100  104   (弃用)

uint16_t BQ_RemainingCapacity();  // 预测的剩余电池容量
uint16_t BQ_FullChargeCapacity(); // 充满电时预测的电池容量 (当前健康 最大电池的容量)

uint16_t BQ_RunTimeToEmpty();    // 当前放电率返回预测的剩余电池容量  // 时间单位 min  √
uint16_t BQ_AverageTimeToFull(); // 充满电时间  √

uint16_t BQ_CycleCount(); // 经历的放电循环次数

//

uint16_t BQ_CBStatus(); // 电池平衡时间信息
uint16_t BQ_Health();   // 返回健康状态

uint16_t BQ_FilteredCapacity(); // 过滤后的容量和能量

#endif