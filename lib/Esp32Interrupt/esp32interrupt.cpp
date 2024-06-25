#include <esp32interrupt.h>

/**
 * @brief 中断函数
 * 
 */
void PinIntEvent()
{
    interstate1 = 1;
}

/**
 * @brief 中断初始化 下降沿触发
 * 
 */
void InterruptInit()
{
    attachInterrupt(Interrupt_Pin, PinIntEvent, FALLING);
}

