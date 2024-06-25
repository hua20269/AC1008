#include <SW6208.h>

/**
 * @brief sw6208初始化
 * 初始化事件：
 *
 */
void SW6208_init()
{
    I2C_Write(SW6208_address, 0x03, 0xf);
    I2C_Write(SW6208_address, 0x30, 0x4);
}

// /**
//  * @brief 实现ln运算
//  *
//  * @param a
//  * @return double
//  */
// double myln(double a)
// {
//     int N = 15; // 我们取了前15+1项来估算
//     int k, nk;
//     double x, xx, y;
//     x = (a - 1) / (a + 1);
//     xx = x * x;
//     nk = 2 * N + 1;
//     y = 1.0 / nk;
//     for (k = N; k > 0; k--)
//     {
//         nk = nk - 2;
//         y = 1.0 / nk + xx * y;
//     }
//     return 2.0 * x * y;
// }

/**
 * @brief sw的adc数据
 * 0x13是adc高八位数据
 * 0x14是adc低四位数据
 * @return uint16_t 返回的adc值  在其他函数中进行处理
 */
uint16_t batteryADC()
{
    uint8_t r_buffer[2];
    int ADCvalue;
    r_buffer[0] = I2C_Read(SW6208_address, 0x13); // 读取adc高八位数据
    Serial.println("ADC");
    Serial.println(r_buffer[0], HEX);
    delay(5);
    r_buffer[1] = I2C_Read(SW6208_address, 0x14); // 读取adc低四位数据
    Serial.println(r_buffer[1], HEX);
    ADCvalue = (r_buffer[0] << 4) + r_buffer[1]; // adc数据的合并
    Serial.println(ADCvalue);
    return ADCvalue;
}

/**
 * @brief 电池电压的读取和计算  电压=adc*0.0012v
 */
float batteryV()
{
    if (I2C_Write(SW6208_address, 0x12, 0x0) == 0)
    {
        Serial.println("V-ok!");
    }
    else
    {
        Serial.print("V-NO!");
    }
    float v_value;
    v_value = batteryADC() * 0.0012; // 电压=adc*0.0012v
    Serial.println(v_value);
    return v_value;
}

/**
 * @brief 输入输出电压获取
 *
 * @return float 电压
 */
float battery_outinV()
{
    if (I2C_Write(SW6208_address, 0x12, 0x1) == 0)
    {
        Serial.print("wok!");
    }
    else
    {
        Serial.print("wNO!");
    }
    float a_value;
    a_value = batteryADC() * 0.004;
    Serial.println(a_value, 4);
    return a_value;
}

/**
 * @brief ic温度获取
 *
 * @return float ic温度
 */
float battery_ictemp()
{
    if (I2C_Write(SW6208_address, 0x12, 0x2) == 0)
    {
        Serial.print("wok!");
    }
    else
    {
        Serial.print("wNO!");
    }
    float a_value;
    a_value = (batteryADC() - 1839) / 6.82;
    Serial.println(a_value, 4);
    return a_value;
}

/**
 * @brief ntc电阻转换温度
 *
 * @return float ntc温度
 */
float battery_ntcV()
{
    uint8_t r_buffer;
    float temp;
    float N1, N2, N3, N4;
    r_buffer = I2C_Read(SW6208_address, 0x48);
    Serial.println("ADC");
    Serial.println(r_buffer, HEX);
    r_buffer = r_buffer & 0x1;
    Serial.println(r_buffer);
    if (I2C_Write(SW6208_address, 0x12, 0x3) == 0)
    {
        Serial.print("wok!");
    }
    else
    {
        Serial.print("wNO!");
    }
    if (r_buffer == 0)
    {
        temp = batteryADC() * 0.0022 / 0.00008;
    }
    if (r_buffer == 1)
    {
        temp = batteryADC() * 0.0011 / 0.00004;
    }
    Serial.println(temp, 4);
    temp = log(temp / R25); // ln(Rt/Rp)：Rt:当前温度下的阻值 R25:25℃下的NTC阻值(K)
    temp /= BX;             // ln(Rt/Rp)/BX 注：BX值一般为NTC的规格，本次用的NTC型号是3950
    temp += (1 / T25);
    temp = 1 / temp;
    temp -= 273.15;
    temp += 0.5;
    Serial.println(temp, 4);
    return temp;
}

/**
 * @brief 充电电流获取
 *
 * @return float 返回充电电流值
 */

float battery_inA()
{
    if (I2C_Write(SW6208_address, 0x12, 0x4) == 0)
    {
        Serial.print("wok!");
    }
    else
    {
        Serial.print("wNO!");
    }
    float a_value;
    a_value = batteryADC() * 0.002273;
    Serial.println(a_value, 4);
    return a_value;
}

/**
 * @brief 输出电流的获取
 *
 * @return float 返回输出电流  小数形式
 */
float battery_outA()
{
    if (I2C_Write(SW6208_address, 0x12, 0x5) == 0)
    {
        Serial.print("wok!");
    }
    else
    {
        Serial.print("wNO!");
    }
    float a_value;
    a_value = batteryADC() * 0.002273;
    Serial.println(a_value, 4);
    return a_value;
}

/**
 * @brief 电量获取
 *
 * @return float 返回电量值  小数形式
 */
float battery_volume()
{
    uint8_t r_buffer[2];
    float a_value;
    int ADCvalue;
    r_buffer[0] = I2C_Read(SW6208_address, 0x73);
    Serial.println("ADC");
    Serial.println(r_buffer[0], HEX);
    delay(5);
    r_buffer[1] = I2C_Read(SW6208_address, 0x74);
    Serial.println(r_buffer[1], HEX);
    ADCvalue = (r_buffer[1] << 8) + r_buffer[0];
    Serial.println(ADCvalue);
    a_value = ADCvalue * 0.1695;
    return a_value;
}

/**
 * @brief 电量百分比获取
 *
 * @return uint16_t 返回电量百分比数值
 */
uint16_t battery_per()
{
    uint8_t r_buffer;
    uint16_t a_value;
    r_buffer = I2C_Read(SW6208_address, 0x7E);
    Serial.println("ADC");
    Serial.println(r_buffer, HEX);
    a_value = r_buffer;
    Serial.println(a_value);
    return a_value;
}

/**
 * @brief 系统充放电状态 AC口状态
 *
 * @param H_value  sys接收 充放状态   1 放电    2 充电
 * @param L_value   A_C接收   1_A1   2_A2  3_A1A2  4_C  5_A1C  6_A2C  7_A1A2C  8_B  .....9-16BL充其他放电无意义.....16_L
 * @param battery_A 充放电流
 *
 */
void sysstate(uint8_t *H_value, uint8_t *L_value, float *battery_A)
{
    uint8_t svalue;
    svalue = I2C_Read(SW6208_address, 0x0C);
    Serial.println(svalue, HEX);
    *H_value = svalue >> 6;
    Serial.println(*H_value);
    *L_value = svalue & 0x1F;
    Serial.println(*L_value);
    if (*H_value == 1) // 放电
    {
        *battery_A = battery_outA(); // 放电电流
    }
    if (*H_value == 2) // 充电
    {
        *battery_A = battery_inA(); // 充电电流
    }
    else
        *battery_A = battery_outA();
}

// 充放电状态
uint8_t batzhuangtai()
{
    uint8_t svalue;
    svalue = I2C_Read(SW6208_address, 0x0C);
    svalue = svalue >> 6;
    return svalue;
}

/**
 * @brief 开启或退出小电流   0X10    4  写1
 */
void kqxdl()
{
    // uint8_t svalue;
    // svalue = I2C_Read(SW6208_address, 0x2E);
    // svalue = svalue & 0X10;
    // if (
    I2C_Write(SW6208_address, 0x2E, 0X10) == 0; // 写1开启或退出小电流
    // {
    //     Serial.print("kaiqichenggong!");
    // }
    // else
    // {
    //     Serial.print("kaiqishibai");
    // }
}
/**
 * @brief 0x33: 小电流充电配置
 */
void I2Csmall_A_ON()
{
    I2C_Write(SW6208_address, 0x33, 0X3) == 0; // 小电流使能
}
void I2Csmall_A_OFF()
{
    I2C_Write(SW6208_address, 0x33, 0X2) == 0; // 小电流不使能
}
/**
 * @brief 小电流状态
 *
 * @return uint8_t   1/0
 */
uint8_t xdlzt()
{
    uint8_t svalue;
    svalue = I2C_Read(SW6208_address, 0x2E);
    svalue = svalue & 0X1;
    return svalue;
}
/**
 * @brief A1口触发插入事件
 *
 */
void A1shijian()
{
    uint8_t svalue;
    svalue = I2C_Read(SW6208_address, 0x19);
    svalue = svalue & 0X1;
    if (I2C_Write(SW6208_address, 0x19, svalue) == 0)
    {
        Serial.print("A2yes");
    }
    else
    {
        Serial.print("A2NO");
    }
}
// 本设备没有A2物理接口，给小电流开关调用
void A2_ON() // A2口触发插入事件
{
    I2C_Write(SW6208_address, 0x19, 0X4) == 0;
}

void A2_OFF() // A2口触发拔出事件
{
    I2C_Write(SW6208_address, 0x19, 0X8) == 0;
}

// 关闭所有输出口
void AC_OFF()
{
    I2C_Write(SW6208_address, 0x18, 0X10) == 0;
}

// void sw6208push(float *battery_V, float *sys_outinv, float *ic_temp, float *ntc_v, float *battery_A, float *sys_w,float *bat_m, float *bat_ntc,uint16_t *sys, uint16_t *A_C, uint16_t *bat_per)
// {
//     Serial.print("batv:");
//     *battery_V = batteryV(); // 电池电压
//     //Serial.println(*battery_V, 4);
//     delay(10);
//     Serial.print("sysstade:");
//     sysstate(&sys, &A_C); // 充电电流 放电电流
//     if (sys == 4)         // 放电
//     {
//         Serial.print("outa:");
//         *battery_A = battery_outA();
//         Serial.println(*battery_A, 4);
//     }
//     if (sys == 8) // 充电
//     {
//         Serial.print("ina:");
//         *battery_A = battery_inA();
//         Serial.println(*battery_A, 8);
//     }
//     else
//         *battery_A = battery_outA();
//     Serial.println(sys);
//     Serial.println(A_C);
//     delay(10);
//     Serial.print("outinv:");
//     *sys_outinv = battery_outinV(); // 输入输出电压
//     Serial.println(*sys_outinv);
//     Serial.print("ictemp:");
//     *ic_temp = battery_ictemp(); // ic温度
//     Serial.println(*ic_temp);
//     delay(10);
//     Serial.print("W:");
//     *sys_w = sys_outinv * battery_A;
//     Serial.println("batper:");
//     *bat_per = battery_per();
//     Serial.println("batm:");
//     *bat_m = battery_volume() * bat_per / 100;
//     Serial.println("ntc:");
//     *bat_ntc = battery_ntcV(); // 电池温度
// }

// uint8_t IICread(uint8_t date1)
// {
//     uint8_t t = 200;
//     uint8_t ret = 0;
//     Wire.beginTransmission(SW6208_address); // 0x78
//     // Serial.println(SW6208_address, HEX);
//     Wire.write(0x13); // 寄存器地址
//     // Wire.write(0x00);
//     ret = Wire.endTransmission(false);
//     Wire.requestFrom(SW6208_address, MAX_AES_BUFFER_SIZE); // MAX_AES_BUFFER_SIZE=1
//     date1 = 0;
//     while (!Wire.available())
//     {
//         t--;
//         delay(1);
//         if (t == 0)
//         {
//             return 1;
//         }
//     }
//     date1 = Wire.read();
//     Serial.print(date1, HEX);
//     Serial.println();
//     return ret;
// }