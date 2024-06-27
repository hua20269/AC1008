#include <SW6306.h>

/**
 * @brief IIC数据写入
 *
 * @param mcuAddr 设备地址
 * @param regAddr 寄存器地址
 * @param senddate 发送的数据
 * @return uint8_t  通讯成功：0   其他通讯失败
 */
uint8_t I2C_Write(uint8_t mcuAddr, uint8_t regAddr, uint8_t senddate)
{
    /*  Write Device Address */
    Wire.beginTransmission(mcuAddr);
    /*  Write Subaddresses */
    Wire.write(regAddr);
    /*  Write Databytes */
    Wire.write(senddate);
    /* 完成一次IIC通讯 默认发送一个停止位*/

    delay(5);
    return Wire.endTransmission();
    // delay(5);
}

/**
 * @brief IIC数据读取
 *
 * @param mcuAddr 设备地址
 * @param regAddr 寄存器地址
 * @param getdate 读取到的数据
 * @return uint16_t 读取成功：0   其他：读取失败
 */
uint16_t I2C_Read(uint8_t mcuAddr, uint16_t regAddr)
{
    uint8_t t = 200;
    uint8_t ret = 0;
    uint16_t getdate;
    /*  Write Device Address */
    Wire.beginTransmission(mcuAddr);
    /*  Write Subaddresses */
    Wire.write(regAddr);
    ret = Wire.endTransmission(false);
    if (ret == 0)
    {
        // Serial.println("Read-ok!");
    }
    else
    {
        Serial.println("Read-NO!");
    }
    Wire.requestFrom(mcuAddr, (size_t)1, (bool)1);
    /* 5. 读出AT24C02返回的值，成功读取后写入缓存变量处，读取失败返回失败码 */
    while (!Wire.available())
    {
        t--;
        delay(1);
        if (t == 0)
        {
            return 1;
        }
    }
    getdate = Wire.read(); // receive a byte as character
    return getdate;
}

/**
 * @brief REG0x24: I2C使能
 * @brief I2C写操作使能，如果要写其他寄存器（除
 * @brief REG0x20~0x24之外），需要先执行如下操作：
 * @param 1) 写REG0x24=0x20；
 * @param 2) 写REG0x24=0x40；
 * @param 3) 写REG0x24=0x80；
 * @param 4) 若要操作REG0x100~REG0x156，写REG0x24=0x81
 * @param 若要退出I2C写操作使能，则写REG0x24=0x00
 */
// 以下两个不能同时使能
void I2C_Write_0_100()
{

    I2C_Write(SW6306_address, 0x23, 0x01);
    I2C_Write(SW6306_address, 0x24, 0x20);
    I2C_Write(SW6306_address, 0x24, 0x40);
    I2C_Write(SW6306_address, 0x24, 0x80);
    // I2C_Write(SW6306_address, 0x1FF, 0x0) ; // 0

    Serial.println("--------------------------------------------------------------------------");
}
// uint8_t I2C_Write_0_100()
// {
//     Wire.beginTransmission(0X3C);
//     Wire.write(0x23);
//     Wire.write(0x01); // 第一关闭低功耗步
//     Wire.endTransmission();
//     Serial.println(I2C_Read(0X3C, 0x23), HEX);
//     /*  Write Device Address */
//     Wire.beginTransmission(0X3C);
//     /*  Write Subaddresses */
//     Wire.write(0x24);
//     /*  Write Databytes */
//     Wire.write(0x20);
//     // 第二步
//     Wire.endTransmission(); /* 完成一次IIC通讯 默认发送一个停止位*/
//     Serial.println(I2C_Read(0X3C, 0x24), HEX);
//     Wire.beginTransmission(0X3C);
//     Wire.write(0x24);
//     Wire.write(0x40); // 第三步
//     Wire.endTransmission();
//     Serial.println(I2C_Read(0X3C, 0x24), HEX);
//     Wire.beginTransmission(0X3C);
//     Wire.write(0x24);
//     Wire.write(0x80); // 第四步

//     return Wire.endTransmission();
// }
void I2C_Write_100_156()
{
    I2C_Write(SW6306_address, 0x24, 0x81); // 100-156 寄存器写使能
}

//-----------------------------------------------------------------------------------------以下为寄存器操作--------前半段是0x100以内，后半段是0x100及以后-------
//-----------------------------------------------------------------------全可读-------------默认只可写 REG0x20~0x24-------------------------

/**
 * @brief sw6306的adc数据    注意：先读 ADC 数据低 8 位，再读数据高 4 位
 * @param REG0x31: ADC数据低8位
 * @param REG0x32: ADC数据高4位
 * @return uint16_t 返回的adc值  在其他函数中进行处理
 */
uint16_t ADC_Data()
{
    uint16_t ADCvalue;
    uint16_t r_buffer[2];
    r_buffer[1] = I2C_Read(SW6306_address, 0x31); // 读取ADC数据低8位
    Serial.println(r_buffer[1], HEX);
    r_buffer[0] = I2C_Read(SW6306_address, 0x32); // 读取ADC数据高4位
    Serial.println(r_buffer[0], HEX);
    ADCvalue = (r_buffer[0] << 8) + r_buffer[1];                                       // adc数据的合并

    // ADCvalue = (I2C_Read(SW6306_address, 0x31) + I2C_Read(SW6306_address, 0x32) << 8); // adc数据的合并   低 8 位必须先读

    Serial.print("ADC_Data: ");
    Serial.println(ADCvalue);
    return ADCvalue;
}

/**
 * @brief  REG0x30: ADC配置 ,   ADC 数据选择 0：输入输出电压（8mV）
 * @return float 系统 输入/输出 电压
 */
float SYS_V()
{
    I2C_Write(SW6306_address, 0x30, 0x0); // 选择系统 输入/输出 电压
    float value;
    value = ADC_Data() * 0.008; // 系统电压 = adc * 0.008V
    Serial.print("--SYS_V: ");
    Serial.println(value);
    return value;
}
/**
 * @brief  REG0x30: ADC配置 ,   ADC 数据选择  1：输入/输出IBUS电流（4mA）
 * @return float 系统 输入/输出 电流
 */
float SYS_A()
{
    I2C_Write(SW6306_address, 0x30, 0x1); // 选择系统 输入/输出 电流
    float value;
    value = ADC_Data() * 0.004; // 系统输入/输出IBUS电流 = adc * 0.004V
    Serial.print("--SYS_A: ");
    Serial.println(value);
    return value;
}
/**
 * @brief  REG0x30: ADC配置 ,   ADC 数据选择 2：电池电压（7mV）
 * @return float 电池电压
 */
float Battery_V()
{
    I2C_Write(SW6306_address, 0x30, 0x2); // 选择电池电压
    float value;
    value = ADC_Data() * 0.007; // 电池电压 = adc * 0.007V
    Serial.print("--Battery_V: ");
    Serial.println(value);
    return value;
}
/**
 * @brief  REG0x30: ADC配置 ,   ADC 数据选择 3：输入/输出IBAT电流（5mA）
 * @return float 电池 输入/输出 电流
 */
float Battery_A()
{
    I2C_Write(SW6306_address, 0x30, 0x3); // 选择电池 输入/输出 电流
    float value;
    value = ADC_Data() * 0.005; // 电池电流 = adc * 0.005A
    Serial.print("--Battery_A: ");
    Serial.println(value);
    return value;
}

/**
 * @brief  REG0x30: ADC配置 ,   ADC 数据选择  9：芯片温度计算公式为：(Adc_data[11:0] – 1839)/6.82℃
 * @return float 芯片温度
 */
float MCU_Temp()
{
    I2C_Write(SW6306_address, 0x30, 0x9);        // 选择 芯片温度
    float mcu_temp = (ADC_Data() - 1839) / 6.82; // 芯片温度 = (adc-1839) / 6.82℃
    Serial.print("--MCU_Temp: ");
    Serial.println(mcu_temp);
    return mcu_temp;
}

/**
 * @brief REG0x30: ADC配置   10：NTC电压（1.1mV）
 *
 * @param REG0x1A: NTC电流指示   bit:7-6   1.通过ADC REG0x31/REG0x32读取NTC电阻上电压
 * @param REG0x1A: NTC电流指示   bit:7-6   2.通过REG0x1A[7:6]获取NTC上的电流档位
 * @param REG0x1A: NTC电流指示   bit:7-6   NTC电流档位标志  0：80uA   1：40uA    2：20uA    3：reserved
 *
 * @return float ntc温度
 */
float NTC_Temp()
{
    I2C_Write(SW6306_address, 0x30, 0xA); // 选择  10：NTC电压（1.1mV）
    float ntc_v = ADC_Data() * 0.0011;    // NTC电阻上实际电压

    uint8_t ntc_a;
    float ntc_r, ntc_temp;
    ntc_a = I2C_Read(SW6306_address, 0x1A) >> 6;
    if (ntc_a == 0)
    {
        ntc_r = ntc_v / 0.00008;
    }
    else if (ntc_a == 1)
    {
        ntc_r = ntc_v / 0.00004;
    }
    else if (ntc_a == 2)
    {
        ntc_r = ntc_v / 0.00002;
    }
    ntc_temp = log(ntc_r / R25); // ln(Rt/Rp)：Rt: 当前温度下的阻值 ,  R25: 25℃下的NTC阻值(K)
    ntc_temp /= BX;              // ln(Rt/Rp)/BX 注：BX值一般为NTC的规格，本次用的NTC型号是3950
    ntc_temp += (1 / T25);
    ntc_temp = 1 / ntc_temp;
    ntc_temp -= 273.15;
    ntc_temp += 0.5;
    Serial.print("--NTC_Temp: ");
    Serial.println(ntc_temp);
    return ntc_temp;
}

/**
 * @brief  电池容量
 * @param  REG0x88: 库仑计当前容量低8位   Bit: 7-0 库仑计当前容量低8位    0.07964mWh/step
 * @param  REG0x89: 库仑计当前容量中8位   Bit: 7-0 库仑计当前容量低8位    0.07964mWh/step
 * @param  REG0x8A: 库仑计当前容量高8位   Bit: 7-0 库仑计当前容量低8位    0.07964mWh/step
 *
 * @return float  电池容量
 */
float Battery_Volume()
{
    float battery_volume = (I2C_Read(SW6306_address, 0x88) + I2C_Read(SW6306_address, 0x89) << 8 + I2C_Read(SW6306_address, 0x8A) << 16) * 0.00007964;
    Serial.print("--Battery_Volume: ");
    Serial.println(battery_volume);
    return battery_volume;
}

/**
 * @brief  REG0x94: 最终处理电量   bit: 7-0 最终处理电量    1%/step
 * @return uint8_t  电池电量百分比
 */
uint8_t Battery_Per()
{
    uint8_t battery_per = I2C_Read(SW6306_address, 0x94); // 电池电量百分比
    Serial.print("--Battery_Per: ");
    Serial.println(battery_per);
    return battery_per;
}

/**
 * @brief  REG0x18: 系统状态指示   充放电状态
 * @param  Bit:5    0：充电关闭   1：充电打开
 * @param  Bit:4    0：放电关闭   1：放电打开
 *
 * @return uint8_t  充放电状态   1 放电   2 充电
 */
uint8_t SYS_State()
{
    uint8_t sys_state = I2C_Read(SW6306_address, 0x18) >> 4;
    Serial.print("--SYS_State: ");
    Serial.println(sys_state);
    return sys_state;
}

/**
 * @brief  REG0x1D: 端口状态指示
 * @param  bit: 3    A1口在线状态指示   0: 不在线  1: 在线
 * @param  bit: 2    A2口在线状态指示   0: 不在线  1: 在线
 * @param  bit: 1    C1口在线状态指示   0: 不在线  1: 在线
 * @param  bit: 0    C2口在线状态指示   0: 不在线  1: 在线
 * @return uint8_t 端口状态   0:空闲   1:C2   2:C1   3:C1C2   4:A2   5:A2C2   6:A2C1   7:A2C1C2   8:A1   9:A1C2   A:A1C1   B:A1C1C2   C:A1A2   D:A1A2C2   E:A1A2C1   F:A1A2C1C2
 * @return ----C2替换为L
 */
uint8_t AC_State()
{
    uint8_t ac_state = I2C_Read(SW6306_address, 0x1D) & 0XF; // 端口状态
    Serial.print("--AC_State: ");
    Serial.println(ac_state);
    return ac_state;
}

/**
 * @brief  REG0x12: 模式状态   bit: 1   小电流模式指示
 * @param  0：未处于小电流模式
 * @param  1：处于小电流模式
 * @return uint8_t  小电流状态   0: 关    1: 开
 */
uint8_t Small_A_State()
{
    uint8_t smalla_state = I2C_Read(SW6306_address, 0x12) & 0X2; // 小电流状态
    Serial.print("--Small_A_State: ");
    Serial.println(smalla_state);
    return smalla_state;
}

// /**
//  * @brief 系统充放电状态 AC口状态
//  *
//  * @param H_value  sys接收 充放状态   1 放电    2 充电
//  * @param L_value   A_C接收   1_A1   2_A2  3_A1A2  4_C  5_A1C  6_A2C  7_A1A2C  8_B  .....9-16BL充其他放电无意义.....16_L
//  * @param battery_A 充放电流
//  *
//  */
// void sysstate(uint8_t *H_value, uint8_t *L_value, float *battery_A)
// {
//     uint8_t svalue;
//     svalue = I2C_Read(SW6208_address, 0x0C);
//     Serial.println(svalue, HEX);
//     *H_value = svalue >> 6;
//     Serial.println(*H_value);
//     *L_value = svalue & 0x1F;
//     Serial.println(*L_value);
//     if (*H_value == 1) // 放电
//     {
//         *battery_A = battery_outA(); // 放电电流
//     }
//     if (*H_value == 2) // 充电
//     {
//         *battery_A = battery_inA(); // 充电电流
//     }
//     else
//         *battery_A = battery_outA();
// }

// // 充放电状态
// uint8_t batzhuangtai()
// {
//     uint8_t svalue;
//     svalue = I2C_Read(SW6208_address, 0x0C);
//     svalue = svalue >> 6;
//     return svalue;
// }

// /**
//  * @brief 开启或退出小电流   0X10    4  写1
//  */
// void kqxdl()
// {
//     // uint8_t svalue;
//     // svalue = I2C_Read(SW6208_address, 0x2E);
//     // svalue = svalue & 0X10;
//     // if (
//     I2C_Write(SW6208_address, 0x2E, 0X10) == 0; // 写1开启或退出小电流
//     // {
//     //     Serial.print("kaiqichenggong!");
//     // }
//     // else
//     // {
//     //     Serial.print("kaiqishibai");
//     // }
// }
// /**
//  * @brief 0x33: 小电流充电配置
//  */
// void I2Csmall_A_ON()
// {
//     I2C_Write(SW6208_address, 0x33, 0X3) == 0; // 小电流使能
// }
// void I2Csmall_A_OFF()
// {
//     I2C_Write(SW6208_address, 0x33, 0X2) == 0; // 小电流不使能
// }
// /**
//  * @brief 小电流状态
//  *
//  * @return uint8_t   1/0
//  */
// uint8_t xdlzt()
// {
//     uint8_t svalue;
//     svalue = I2C_Read(SW6208_address, 0x2E);
//     svalue = svalue & 0X1;
//     return svalue;
// }
// /**
//  * @brief A1口触发插入事件
//  *
//  */
// void A1shijian()
// {
//     uint8_t svalue;
//     svalue = I2C_Read(SW6208_address, 0x19);
//     svalue = svalue & 0X1;
//     if (I2C_Write(SW6208_address, 0x19, svalue) == 0)
//     {
//         Serial.print("A2yes");
//     }
//     else
//     {
//         Serial.print("A2NO");
//     }
// }
// // 本设备没有A2物理接口，给小电流开关调用
// void A2_ON() // A2口触发插入事件
// {
//     I2C_Write(SW6208_address, 0x19, 0X4) == 0;
// }

// void A2_OFF() // A2口触发拔出事件
// {
//     I2C_Write(SW6208_address, 0x19, 0X8) == 0;
// }

// // 关闭所有输出口
// void AC_OFF()
// {
//     I2C_Write(SW6208_address, 0x18, 0X10) == 0;
// }

// // void sw6208push(float *battery_V, float *sys_outinv, float *ic_temp, float *ntc_v, float *battery_A, float *sys_w,float *bat_m, float *bat_ntc,uint16_t *sys, uint16_t *A_C, uint16_t *bat_per)
// // {
// //     Serial.print("batv:");
// //     *battery_V = batteryV(); // 电池电压
// //     //Serial.println(*battery_V, 4);
// //     delay(10);
// //     Serial.print("sysstade:");
// //     sysstate(&sys, &A_C); // 充电电流 放电电流
// //     if (sys == 4)         // 放电
// //     {
// //         Serial.print("outa:");
// //         *battery_A = battery_outA();
// //         Serial.println(*battery_A, 4);
// //     }
// //     if (sys == 8) // 充电
// //     {
// //         Serial.print("ina:");
// //         *battery_A = battery_inA();
// //         Serial.println(*battery_A, 8);
// //     }
// //     else
// //         *battery_A = battery_outA();
// //     Serial.println(sys);
// //     Serial.println(A_C);
// //     delay(10);
// //     Serial.print("outinv:");
// //     *sys_outinv = battery_outinV(); // 输入输出电压
// //     Serial.println(*sys_outinv);
// //     Serial.print("ictemp:");
// //     *ic_temp = battery_ictemp(); // ic温度
// //     Serial.println(*ic_temp);
// //     delay(10);
// //     Serial.print("W:");
// //     *sys_w = sys_outinv * battery_A;
// //     Serial.println("batper:");
// //     *bat_per = battery_per();
// //     Serial.println("batm:");
// //     *bat_m = battery_volume() * bat_per / 100;
// //     Serial.println("ntc:");
// //     *bat_ntc = battery_ntcV(); // 电池温度
// // }

// // uint8_t IICread(uint8_t date1)
// // {
// //     uint8_t t = 200;
// //     uint8_t ret = 0;
// //     Wire.beginTransmission(SW6208_address); // 0x78
// //     // Serial.println(SW6208_address, HEX);
// //     Wire.write(0x13); // 寄存器地址
// //     // Wire.write(0x00);
// //     ret = Wire.endTransmission(false);
// //     Wire.requestFrom(SW6208_address, MAX_AES_BUFFER_SIZE); // MAX_AES_BUFFER_SIZE=1
// //     date1 = 0;
// //     while (!Wire.available())
// //     {
// //         t--;
// //         delay(1);
// //         if (t == 0)
// //         {
// //             return 1;
// //         }
// //     }
// //     date1 = Wire.read();
// //     Serial.print(date1, HEX);
// //     Serial.println();
// //     return ret;
// // }