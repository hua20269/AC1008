#include "keros_lib.h"
#include "keros_i2c_interface.h"
uint8_t I2CWrite(uint8_t bDevAddr, uint8_t *pbAddr, uint8_t wAddrLen, uint8_t *pbData, uint8_t wDataLen)
{
    /*  Write Device Address */
    Wire.beginTransmission(bDevAddr);
    /*  Write Subaddresses */
    while (wAddrLen > 0)
    {
        if (Wire.write(*pbAddr) == 0)
            return ERR_I2C_NO_DATA_ACK;
        pbAddr++;
        wAddrLen--;
    }

    /*  Write Databytes */
    while (wDataLen > 0)
    {
        if (Wire.write(*pbData) == 0)
        {
            Wire.endTransmission();
            return ERR_I2C_NO_DATA_ACK;
        }
        Serial.print(*pbData, HEX);
        pbData++;
        wDataLen--;
    }
    Serial.println();
    Wire.endTransmission();

    return 0;

} /*  I2CWrite */

uint8_t I2CRead(uint8_t bDevAddr, uint8_t *pbAddr, uint8_t wAddrLen, uint8_t *pbData, uint8_t wDataLen)
{
    uint8_t t = 200;
    /*  Write Device Address */
    Wire.beginTransmission(bDevAddr);

    /*  Write Subaddresses */
    while (wAddrLen > 0)
    {
        if (Wire.write(*pbAddr) == 0)
            return ERR_I2C_NO_DATA_ACK;
        pbAddr++;
        wAddrLen--;
    }

    if (wDataLen == 0)
        return 0;
    Wire.endTransmission(FALSE);
    Wire.requestFrom(bDevAddr, wDataLen);
    while (!Wire.available())
    {
        t--;
        delay(1);
        if (t == 0)
        {
            return 1;
        }
    }
    /*  Read Databytes with acknowledge from master */
    for (int i = 0; i < wDataLen; i++)
    {
        *pbData = Wire.read();
        Serial.print(*pbData, HEX);
        pbData++;
    }
    Serial.println();
    return 0;
} /*  I2CRead */
