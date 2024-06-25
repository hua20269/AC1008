#include <iic.h>

void IICinit()
{
    Wire.begin(SDA_PIN, SCL_PIN);
}

/**
 * @brief IIC数据写入
 *
 * @param mcuAddr 设备地址
 * @param regAddr 寄存器地址
 * @param senddate 发送的数据
 * @return uint8_t  通讯成功：0 其他通讯失败
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
uint8_t I2C_Read(uint8_t mcuAddr, uint8_t regAddr)
{
    uint8_t t = 200;
    uint8_t ret = 0;
    uint8_t getdate;
    /*  Write Device Address */
    Wire.beginTransmission(mcuAddr);
    /*  Write Subaddresses */
    Wire.write(regAddr);
    ret = Wire.endTransmission(false);
    if (ret == 0)
    {
        Serial.println("Read-ok!");
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
