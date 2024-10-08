#include <SW6306.h>

/**
 * @brief IIC数据写入  uint8_t
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
 * @brief IIC数据写入  uint16_t
 *
 * @param mcuAddr 设备地址
 * @param regAddr 寄存器地址
 * @param senddate 发送的数据
 * @return uint8_t  通讯成功：0   其他通讯失败
 */
uint8_t I2C_Write_16(uint8_t mcuAddr, uint16_t regAddr, uint8_t senddate)
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
    uint8_t t = 32; // 16
    uint8_t ret = 0;
    uint16_t getdate;
    Wire.beginTransmission(mcuAddr); // 设备地址 写入
    Wire.write(regAddr);             // 寄存器地址 写入
    ret = Wire.endTransmission(false);
    if (ret == 0) // 判断是否成功传输
    {
        Wire.requestFrom(mcuAddr, (size_t)1, (bool)1); // 请求数据
        /* 5. 读出AT24C02返回的值，成功读取后写入缓存变量处，读取失败返回失败码 */
        while (!Wire.available()) // 读取数据非一个字节遍历
        {
            t--;
            delay(1);
            if (t == 0)
                return 1;
        }
        getdate = Wire.read(); // 读取数据
        return getdate;
    }
    else
    {
        // 根据不同的错误代码进行处理
        Serial.print("Error: Transmission failed with code ");
        Serial.println(ret);
        return 0;
    }
}
// I2C读取事例
// #include <Wire.h>

// void setup()
// {
//     Wire.begin();       // 初始化I2C通信
//     Serial.begin(9600); // 初始化串口通信，用于输出数据
// }

// void loop()
// {
//     int sensorAddress = 0x48; // I2C设备地址
//     int numBytes = 1;         // 要读取的字节数

//     Wire.beginTransmission(sensorAddress);
//     Wire.write(0); // 如果设备需要寄存器地址，可以在这里写入
//     byte transmissionStatus = Wire.endTransmission();

//     if (transmissionStatus == 0)
//     {                                              // 判断是否成功传输
//         Wire.requestFrom(sensorAddress, numBytes); // 请求数据

//         if (Wire.available() == numBytes)
//         {                                   // 确认是否有足够数据可读取
//             byte temperature = Wire.read(); // 读取数据
//             Serial.print("Temperature: ");
//             Serial.println(temperature);

//             // 判断温度是否超过阈值
//             int threshold = 25; // 阈值设为25度
//             if (temperature > threshold)
//             {
//                 Serial.println("Temperature is above threshold!");
//             }
//             else
//             {
//                 Serial.println("Temperature is below threshold.");
//             }
//         }
//         else
//         {
//             Serial.println("Error: Not enough data available.");
//         }
//     }
//     else
//     {
//         // 根据不同的错误代码进行处理
//         Serial.print("Error: Transmission failed with code ");
//         Serial.println(transmissionStatus);
//     }

//     delay(1000); // 每秒读取一次
// }

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
void I2C_Write_0_100()
{
    I2C_Write(SW6306_address, 0x23, 0x01); // 关闭低功耗
    I2C_Write(SW6306_address, 0x24, 0x20);
    I2C_Write(SW6306_address, 0x24, 0x40);
    I2C_Write(SW6306_address, 0x24, 0x80); // 最终状态
}
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
    r_buffer[0] = I2C_Read(SW6306_address, 0x32); // 读取ADC数据高4位
    ADCvalue = r_buffer[1] + (r_buffer[0] << 8);  // adc数据的合并

    // ADCvalue = (I2C_Read(SW6306_address, 0x31) + I2C_Read(SW6306_address, 0x32) << 8); // adc数据的合并   低 8 位必须先读

    Serial.print(r_buffer[0], HEX);
    Serial.print("\t");
    Serial.println(r_buffer[1], HEX);

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
    Serial.print("|--SYS_V: ");
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
    Serial.print("|--SYS_A: ");
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
    Serial.print("|--Battery_V: ");
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
    Serial.print("|--Battery_A: ");
    Serial.println(value);
    return value;
}

/**
 * @brief  REG0x30: ADC配置 ,   ADC 数据选择  9：芯片温度计算公式为：(Adc_data[11:0] – 1839)/6.82℃
 * @return float 芯片温度
 */
float IC_Temp()
{
    I2C_Write(SW6306_address, 0x30, 0x9);       // 选择 芯片温度
    float ic_temp = (ADC_Data() - 1839) / 6.82; // 芯片温度 = (adc-1839) / 6.82℃
    Serial.print("|--IC_Temp: ");
    Serial.println(ic_temp);
    return ic_temp;
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
    Serial.print("|--NTC_Temp: ");
    Serial.println(ntc_temp);
    return ntc_temp;
}

/**
 * @brief  库仑计当前容量
 * @param  REG0x86: 库仑计最大容量低 8 位   bit: 7-0   库仑计最大容量低 8 位   326.2236mWh/step
 * @param  REG0x87: 库仑计最大容量高 4 位   bit: 3-0   库仑计最大容量高 4 位   326.2236mWh/step
 *
 * @param  REG0x88: 库仑计当前容量低8位
 * @param  REG0x89: 库仑计当前容量中8位
 * @param  REG0x8A: 库仑计当前容量高8位
 *
 * @return float 库仑计当前容量
 */
float Battery_Volume()
{
    Serial.print("|--Battery_Volume_Max: X ");
    Serial.println((((I2C_Read(SW6306_address, 0x86) + I2C_Read(SW6306_address, 0x87) << 8)) & 0x0FFF) * 0.3262236);                                                  // 库仑计最大容量    数据不对
    float battery_volume = (((I2C_Read(SW6306_address, 0x88) + I2C_Read(SW6306_address, 0x89) << 8) + I2C_Read(SW6306_address, 0x8A) << 16) & 0xFFFFFF) * 0.00007964; // 库仑计当前容量

    Serial.print("|--Battery_Volume: ");
    Serial.println(battery_volume);
    return battery_volume;
}

/**
 * @brief  REG0x94: 最终处理电量
 * @param  bit: 7-0 最终处理电量    1%/step
 * @return uint8_t  电池电量百分比
 */
uint8_t Battery_Per()
{
    uint8_t battery_per = I2C_Read(SW6306_address, 0x94); // 电池电量百分比
    Serial.print("|--Battery_Per: ");
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
    sys_state = sys_state & 0x3;
    Serial.print("|--SYS_State: ");
    Serial.println(sys_state);
    return sys_state;
}

/**
 * @brief  REG0x1D: 端口状态指示
 * @param  bit: 3    A1口在线状态指示   0: 不在线  1: 在线
 * @param  bit: 2    A2口在线状态指示   0: 不在线  1: 在线
 * @param  bit: 1    C1口在线状态指示   0: 不在线  1: 在线
 * @param  bit: 0    C2口在线状态指示   0: 不在线  1: 在线
 * @return uint8_t 端口状态   0:空闲   1:C2   2:C1   3:C1C2   4:A2   5:A2C2   6:A2C1   7:A2C1C2   8:A1   9:A1C2   10/A:A1C1   11/B:A1C1C2   12/C:A1A2   13/D:A1A2C2   14/E:A1A2C1   15/F:A1A2C1C2
 * @return ----C2替换为L
 */
uint8_t AC_State()
{
    uint8_t ac_state = I2C_Read(SW6306_address, 0x1D) & 0XF; // 端口状态
    Serial.print("|--AC_State: ");
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
    uint8_t smalla_state = I2C_Read(SW6306_address, 0x12) >> 1; // 小电流状态
    Serial.print("|--Small_A_State: ");
    Serial.println(smalla_state);
    return smalla_state;
}

/**
 * @brief  REG0x28: 模式设置
 * @param  bit: 1  控制进入小电流充电
 * @param  1) 要进入小电流模式，此bit先写0，之后再写1，小电流模式定时重新打开
 * @param  2) 要退出小电流模式，此bit写0
 * @return
 */
void Small_A_ON_or_OFF()
{
    if (Small_A_State() == 0)
    {
        I2C_Write(SW6306_address, 0x28, 0X0);
        I2C_Write(SW6306_address, 0x28, 0X2); // 此bit先写0，之后再写1，小电流模式定时重新打开
    }
    else if (Small_A_State() == 1)
        I2C_Write(SW6306_address, 0x28, 0X0); // 要退出小电流模式，此bit写0
}

/**
 * @brief  REG0x28: 模式设置
 * @param  bit:3  强制关闭输出
 * @param  0：无影响      1：强制关闭输出使能      此bit不会自动清零，在强制关闭输出期间，A口插入检测关闭，不响应按键打开输出，Type-C口只作为sink
 * @return
 */
void AC_OFF()
{
    I2C_Write(SW6306_address, 0x28, 0X8); // 此bit不会自动清零

    Serial.print("|--AC_OFF");
}
void AC_ON() // 解除
{
    if (I2C_Read(SW6306_address, 0x28) & 0x8 == 8) // bit3
    {
        I2C_Write(SW6306_address, 0x28, 0X0);
        Serial.print("|--AC_ON");
    }
}

/**
 * @brief  REG0x0F: 快充指示
 * @param  bit 3-0 快充指示
 * @param  0：None   1：QC2   2：QC3   3：QC3+   4：FCP   5：SCP   6：PD FIX   7：PD PPS   8：PE 1.1   9：PE 2.0   10：VOOC 1.0   11：VOOC 4.0   12：SuperVOOC   13：SFCP   14：AFC   15：UFCS
 * @return
 */
uint8_t Protocol()
{
    Serial.println("  0:None   1:QC2   2:QC3   3:QC3+   4:FCP   5:SCP   6:PD FIX   7:PD PPS   8:PE 1.1   9:PE 2.0   10:VOOC 1.0   11:VOOC 4.0   12:SuperVOOC   13:SFCP   14:AFC   15:UFCS");
    Serial.print("|--Sink_Protocol: ");
    Serial.println(I2C_Read(SW6306_address, 0x0F) & 0x0F);

    return I2C_Read(SW6306_address, 0x0F) & 0x0F; // 快充协议
}

/**
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */
//-------------------------------------------------------------------------SW6306init初始化---------------------
//-------------------------------------------------------------------------SW6306init初始化---------------------
//-------------------------------------------------------------------------SW6306init初始化---------------------
//-------------------------------------------------------------------------SW6306init初始化---------------------
//-------------------------------------------------------------------------SW6306init初始化---------------------
//-------------------------------------------------------------------------SW6306init初始化---------------------

void SW6306init() // sw6306初始化
{
    // if (I2C_Read(SW6306_address, 0x45) != 0x64 || I2C_Read(SW6306_address, 0x4F) != 0x64) //
    // {
    //     I2C_Write(SW6306_address, 0x40, 0x84); // 输入 输出功率IIC控制  8高位输出   4低位输入
    //     I2C_Write(SW6306_address, 0x45, 0x64); // 输入 设置100w   (1-100w)
    //     I2C_Write(SW6306_address, 0x4F, 0x64); // 输出 设置100w   (1-100w)
    // }
    if (Battery_V() >= 12.4) // 电池电压大于等于12.4V，则设置涓流充电电压为系统自动控制
    {
        if ((I2C_Read(SW6306_address, 0X50) != 0X0))
        {
            I2C_Write(SW6306_address, 0x50, 0x0); // 涓流充电电压为系统自动控制
        }
    }
    else // 电池电压低于12.4V，则设置涓流充电电压为9V
    {
        if ((I2C_Read(SW6306_address, 0X50) != 0X09))
        {
            I2C_Write(SW6306_address, 0x50, 0x09); // 涓流充电申请电压为9V
        }
    }
    //-----------------------------------------------------------------------------------------------------------------------

    Serial.println("-----------------------------------------------------------------0x15 --18----2A----2b---- 2c--");
    Serial.println(I2C_Read(SW6306_address, 0x15));
    Serial.println(I2C_Read(SW6306_address, 0x18));
    Serial.println(I2C_Read(SW6306_address, 0x2A));
    Serial.println(I2C_Read(SW6306_address, 0x2B));
    Serial.println(I2C_Read(SW6306_address, 0x2C));
    I2C_Write(SW6306_address, 0x15, 0xFF);
    I2C_Write_100_156(); // 100-156 寄存器写使能

    //

    Serial.println("-----------------------0x104------------------------");
    Serial.println(I2C_Read(SW6306_address, 0x104));
    if (I2C_Read(SW6306_address, 0x104) != 0x60)   // 6-4 UVLO迟滞，N为电池节数         0：0.4V*N     1：0.1V*N     2：0.2V*N     3：0.3V*N     4：0.5V*N     5：0.6V*N     6：0V     7：0.8V*N
        I2C_Write_16(SW6306_address, 0x104, 0x60); // 三元锂电池欠压门限，N为电池节数     0：3.0V*N     1：2.6V*N     2：2.7V*N     3：2.8V*N     4：2.9V*N     5：3.1V*N     6：3.2V*N     7：3.3V*N

    if (I2C_Read(SW6306_address, 0x10D) != 0x30)
        I2C_Write_16(SW6306_address, 0x10D, 0x30); // 设置涓流电流400ma    0：100mA    1：200mA    2：300mA    3：400mA

    if (I2C_Read(SW6306_address, 0x11D) != 0x80)
        I2C_Write_16(SW6306_address, 0x11D, 0x80); // C2口配置为B/L口模式

    if (I2C_Read(SW6306_address, 0x11B) != 0x30)
        I2C_Write_16(SW6306_address, 0x11B, 0X30); // 小电流使能

    if (I2C_Read(SW6306_address, 0x14E) != 0x10)
        I2C_Write_16(SW6306_address, 0x14E, 0X10); // 容量学习使能       bit 4      0：禁止      1：使能      容量学习使能后，触发UVLO后开始充电就开始容量学习

    if (I2C_Read(SW6306_address, 0x119) != 0x59)
        I2C_Write_16(SW6306_address, 0x119, 0x59); // 单口  多口空载时间设置     1：8s (min)   无线充空载时间设置   1：16s (min)

    if (I2C_Read(SW6306_address, 0x107) != 0x1B || I2C_Read(SW6306_address, 0x100) != 0x8E) // 功率设置
    {
        I2C_Write_16(SW6306_address, 0x107, 0x1B); // 输入功率 设置 45w   0：27W    1：30W    2：35W    3：45W    4：60W    5：65W    6：100W    7：100W
        I2C_Write_16(SW6306_address, 0x100, 0x8E); // 输出功率 设置 100w  0：27W    1：30W    2：35W    3：45W    4：60W    5：65W    6：100W    7：reserved
    }

    if (I2C_Read(SW6306_address, 0x108) != 0x0C)   // 充电配置
        I2C_Write_16(SW6306_address, 0x108, 0x0C); // 设置电池类型4.2V    电池节数4节 0000 1100

    I2C_Write_16(SW6306_address, 0x1FF, 0x0); // 切换回 0-100 写使能
}
