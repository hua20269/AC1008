#include <bq40z50.h>
#include <Arduino.h>
uint8_t bq40z50_word_buf[3];

/**
 * @brief IIC数据写入
 *
 * @param mcuAddr 设备地址
 * @param regAddr 寄存器地址
 * @param senddate 发送的数据
 * @return uint8_t  通讯成功：0 其他通讯失败
 * */
uint8_t BQI2C_Write(uint8_t mcuAddr, uint16_t regAddr, uint8_t senddate)
{
    /*  Write Device Address */
    Wire.beginTransmission(mcuAddr);
    /*  Write Subaddresses */
    Wire.write(regAddr);
    /*  Write Databytes */
    Wire.write(senddate);
    /* 完成一次IIC通讯 默认发送一个停止位*/
    return Wire.endTransmission();
    delay(5);
}

/**
 * @brief IIC数据读取
 *
 * @param mcuAddr 设备地址
 * @param regAddr 寄存器地址
 * @param getdate 读取到的数据
 * @return uint8_t 读取成功：0 其他：读取失败
 */
void BQI2C_Read(uint8_t mcuAddr, uint8_t regAddr)
{
    uint8_t ret = 0;
    Wire.beginTransmission(mcuAddr);
    /*  Write Subaddresses */
    Wire.write(regAddr);
    ret = Wire.endTransmission(false);
    if (ret == 0)
    {
        // Serial.println("Read-ok!");
    }
    else
    {
        Serial.println("Read-no!");
    }
    Wire.requestFrom(mcuAddr, (size_t)3, (size_t)1);
    /*成功读取后写入缓存变量处，读取失败返回失败码 */
    while (Wire.available())
    {
        bq40z50_word_buf[1] = Wire.read(); // 低位
        bq40z50_word_buf[2] = Wire.read(); // 中位
        bq40z50_word_buf[3] = Wire.read(); // 高位
    }
}
//--------------------------------------------------------------------------------以下为寄存器读取---------------------

/**
 * @brief  设置电池型号
 * @param  Address  14.4 0x03 BatteryMode()
 * @param  Unit    & 0x8000  mAH改cWh
 */
void BatteryMode()
{
    uint16_t getdate;
    BQI2C_Write(BQ40z50address, 0x03, getdate & 0xCFFF);
    BQI2C_Read(BQ40z50address, 0x03); // 
    getdate = (bq40z50_word_buf[2] << 8) + bq40z50_word_buf[1];
    

    Serial.print("BatteryMode: -----------------------------------------------------------");
    Serial.println(getdate);
}

/**
 * @brief 电池温度
 * @param  Address 14.9 0x08 Temperature()
 * @param  Unit  0.1°K
 * @return uint16_t 返回电池温度数值0.1°K 需要换算摄氏度
 */
uint16_t BQ_Temperature()
{
    uint16_t getdate;
    BQI2C_Read(BQ40z50address, 0x08); // 0x08是环境温度的寄存器地址
    getdate = (bq40z50_word_buf[2] << 8) + bq40z50_word_buf[1];
    Serial.print("BQ_Temperature: ");
    Serial.println(getdate);
    return getdate;
}
/**
 * @brief BQ电池包总电压
 * @param Address 14.10 0x09 Voltage()
 * @param Unit  mV
 * @return uint16_t 此读字函数返回测得的电池电压之和
 */
uint16_t BQ_Voltage() //
{
    uint16_t getdate;
    BQI2C_Read(BQ40z50address, 0x09); //
    getdate = (bq40z50_word_buf[2] << 8) + bq40z50_word_buf[1];
    Serial.print("BQ_Voltage: ");
    Serial.println(getdate);
    return getdate;
}
/**
 * @brief 电池电流大小
 * @param Address   14.11 0x0A Current()
 * @param Unit  mA
 * @param 负值对应输出(放电)
 * @return int 此读字函数返回来自库仑计数器的测量电流。如果器件的输入超过最大值，则该值将箝位在最大值，并且不会翻转
 */
int16_t BQ_Current()
{
    BQI2C_Read(BQ40z50address, 0x0A); // 0x0A是电池电流的寄存器地址
    Serial.print("BQ_Current: ");
    Serial.println((bq40z50_word_buf[2] << 8) + bq40z50_word_buf[1]);
    return (bq40z50_word_buf[2] << 8) + bq40z50_word_buf[1];
}
/**
 * @brief Cell 4 电压   电池4电压
 * @param Address 14.35 0x3C CellVoltage4()
 * @param Unit  mV
 * @return uint16_t 此读字函数返回 Cell 4 电压
 */
uint16_t BQ_CellVoltage4()
{
    uint16_t getdate;
    BQI2C_Read(BQ40z50address, 0x3C);
    getdate = (bq40z50_word_buf[2] << 8) + bq40z50_word_buf[1];
    Serial.print("BQ_CellVoltage4: ");
    Serial.println(getdate);
    return getdate;
}
/**
 * @brief Cell 3 电压   电池3电压
 * @param Address 14.36 0x3D CellVoltage3()
 * @param Unit  mV
 * @return uint16_t 此读字函数返回 Cell 3 电压
 */
uint16_t BQ_CellVoltage3()
{
    uint16_t getdate;
    BQI2C_Read(BQ40z50address, 0x3D);
    getdate = (bq40z50_word_buf[2] << 8) + bq40z50_word_buf[1];
    Serial.print("BQ_CellVoltage3: ");
    Serial.println(getdate);
    return getdate;
}
/**
  * @brief Cell 2 电压   电池2电压
  * @param Address  14.37 0x3E CellVoltage2()
  * @param Unit  mV
  * @return uint16_t 此读字函数返回 Cell 2 电压
  */
uint16_t BQ_CellVoltage2()
{
    uint16_t getdate;
    BQI2C_Read(BQ40z50address, 0x3E);
    getdate = (bq40z50_word_buf[2] << 8) + bq40z50_word_buf[1];
    Serial.print("BQ_CellVoltage2: ");
    Serial.println(getdate);
    return getdate;
}
/**
  * @brief Cell 1 电压   电池1电压
  * @param Address  14.38 0x3F CellVoltage1()
  * @param Unit  mV
  * @return uint16_t 此读字函数返回 Cell 1 电压
  */
uint16_t BQ_CellVoltage1()
{
    uint16_t getdate;
    BQI2C_Read(BQ40z50address, 0x3F);
    getdate = (bq40z50_word_buf[2] << 8) + bq40z50_word_buf[1];
    Serial.print("BQ_CellVoltage1: ");
    Serial.println(getdate);
    return getdate;
}

/**
  * @brief 电池组充满电的剩余时间
  * @param Address  14.6 0x05 AtRateTimeToFull()
  * @param Unit   min
  * @return uint16_t 此字读取函数返回电池组充满电的剩余时间
  */
uint16_t BQ_AtRateTimeToFull()
{
    uint16_t getdate;
    BQI2C_Read(BQ40z50address, 0x05);
    getdate = (bq40z50_word_buf[2] << 8) + bq40z50_word_buf[1];
    Serial.print("BQ_AtRateTimeToFull: ");
    Serial.println(getdate);
    return getdate;
}
/**
  * @brief 电池组完全放电的剩余时间。
  * @param Address   14.7 0x06 AtRateTimeToEmpty(
  * @param Unit   min
  * @return uint16_t   此字读取函数返回电池组完全放电的剩余时间。
  */
uint16_t BQ_AtRateTimeToEmpty()
{
    uint16_t getdate;
    BQI2C_Read(BQ40z50address, 0x06);
    getdate = (bq40z50_word_buf[2] << 8) + bq40z50_word_buf[1];
    Serial.print("BQ_AtRateTimeToEmpty: ");
    Serial.println(getdate);
    return getdate;
}

/**
  * @brief 电池容量百分比 （根据充满电预测最大容量）
  * @param Address    14.14 0x0D RelativeStateOfCharge()
  * @param Unit  %
  * @return uint16_t  此读字函数以 FullChargeCapacity（） 的百分比形式返回预测的剩余电池容量
  */
uint8_t BQ_RelativeStateOfCharge()
{
    uint16_t getdate;
    BQI2C_Read(BQ40z50address, 0x0D);
    getdate = (bq40z50_word_buf[2] << 8) + bq40z50_word_buf[1];
    Serial.print("BQ_AbsoluteStateOfCharge: ");
    Serial.println(getdate);
    return getdate;
}
/**
  * @brief 电池容量百分比  （系统预测） Test会超100  104   (弃用)
  * @param Address    14.15 0x0E AbsoluteStateOfCharge()
  * @param Unit  %
  * @return uint16_t  此读字函数以百分比形式返回预测的剩余电池容量
  */
uint8_t BQ_AbsoluteStateOfCharge()
{
    uint16_t getdate;
    BQI2C_Read(BQ40z50address, 0x0E);
    getdate = (bq40z50_word_buf[2] << 8) + bq40z50_word_buf[1];
    Serial.print("BQ_AbsoluteStateOfCharge: ");
    Serial.println(getdate);
    return getdate;
}
/**
  * @brief 预测的剩余电池容量
  * @param Address   14.16 0x0F RemainingCapacity(
  * @param Unit  mAh/cWh
  * @return uint16_t  此读字函数返回预测的剩余电池容量
  */
uint16_t BQ_RemainingCapacity()
{
    uint16_t getdate;
    BQI2C_Read(BQ40z50address, 0x0F);
    getdate = (bq40z50_word_buf[2] << 8) + bq40z50_word_buf[1];
    Serial.print("BQ_RemainingCapacity: ");
    Serial.println(getdate);
    return getdate;
}

/**
  * @brief 充满电时预测的电池容量 (当前健康 最大电池的容量)
  * @param Address   14.17 0x10 FullChargeCapacity()
  * @param Unit  mAh/cWh
  * @return uint16_t  此读字函数返回充满电时预测的电池容量。在充电过程中，返回的值不会更新
  */
uint16_t BQ_FullChargeCapacity()
{
    uint16_t getdate;
    BQI2C_Read(BQ40z50address, 0x10);
    getdate = (bq40z50_word_buf[2] << 8) + bq40z50_word_buf[1];
    Serial.print("BQ_FullChargeCapacity: ");
    Serial.println(getdate);
    return getdate;
}

/**
  * @brief 当前放电率返回预测的剩余电池容量
  * @param Address   14.18 0x11 RunTimeToEmpty()
  * @param Unit  min
  * @return uint16_t  此读字函数根据当前放电率返回预测的剩余电池容量。
  */
uint16_t BQ_RunTimeToEmpty()
{
    uint16_t getdate;
    BQI2C_Read(BQ40z50address, 0x11);
    getdate = (bq40z50_word_buf[2] << 8) + bq40z50_word_buf[1];
    Serial.print("BQ_RunTimeToEmpty: ");
    Serial.println(getdate);
    return getdate;
}
/**
  * @brief  充满电时间
  * @param Address    14.20 0x13 AverageTimeToFull()
  * @param Unit  min
  * @return uint16_t   此读字函数根据 AverageCurrent（） 返回预测的充满电时间
  */
uint16_t BQ_AverageTimeToFull()
{
    uint16_t getdate;
    BQI2C_Read(BQ40z50address, 0x13);
    getdate = (bq40z50_word_buf[2] << 8) + bq40z50_word_buf[1];
    Serial.print("BQ_AverageTimeToFull: ");
    Serial.println(getdate);
    return getdate;
}

/**
  * @brief 经历的放电循环次数
  * @param Address   14.24 0x17 CycleCount()
  * @param Unit   cycles
  * @return uint16_t  此读字函数返回电池所经历的放电循环次数。默认值存储在数据闪存值 Cycle Count 中，该值在运行时更新
  */
uint16_t BQ_CycleCount()
{
    uint16_t getdate;
    BQI2C_Read(BQ40z50address, 0x17);
    getdate = (bq40z50_word_buf[2] << 8) + bq40z50_word_buf[1];
    Serial.print("BQ_CycleCount: ");
    Serial.println(getdate);
    return getdate;
}

/**
  * @brief 电池平衡时间信息。
  * @param Address   14.69 0x76 CBStatus
  * @param Unit
  * @return uint16_t  此命令指示设备返回电池平衡时间信息。
  */
uint16_t BQ_CBStatus()
{
    uint16_t getdate;
    BQI2C_Read(BQ40z50address, 0x76);
    getdate = (bq40z50_word_buf[2] << 8) + bq40z50_word_buf[1];
    Serial.print("BQ_CBStatus: ");
    Serial.println(getdate);
    return getdate;
}

/**
  * @brief 返回健康状态
  * @param Address   14.70 0x77 State-of-Health
  * @param Unit
  * @return uint16_t  此命令指示设备返回健康状态、完全充电容量和能量。
  */
uint16_t BQ_Health()
{
    uint16_t getdate;
    BQI2C_Read(BQ40z50address, 0x77);
    getdate = (bq40z50_word_buf[2] << 8) + bq40z50_word_buf[1];
    Serial.print("BQ_Health: ");
    Serial.println(getdate);
    return getdate;
}

/**
  * @brief 过滤后的容量和能量
  * @param Address   14.71 0x78 FilteredCapacity
  * @param Unit
  * @return uint16_t  此命令指示设备返回过滤后的容量和能量，即使 [SMOOTH] = 0。
  */
uint16_t BQ_FilteredCapacity()
{
    uint16_t getdate;
    BQI2C_Read(BQ40z50address, 0x78);
    getdate = (bq40z50_word_buf[2] << 8) + bq40z50_word_buf[1];
    Serial.print("BQ_FilteredCapacity: ");
    Serial.println(getdate);
    return getdate;
}
