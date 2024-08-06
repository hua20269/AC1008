#include <eeprom32.h>
// 初始化存储
void EEPROMinit()
{
    EEPROM.begin(EEPROM_SIZE);
}

void EE_CycleCount(uint16_t bat_per)
{
    uint16_t xh;
    if (bat_per >= 80)
    {
        if (EEPROM.read(0) != 1)
        {
            EEPROM.write(0, 1);
            delay(3);
            EEPROM.commit();
            delay(3);
        }
    }
    if (bat_per <= 20)
    {
        if (EEPROM.read(1) != 1)
        {
            EEPROM.write(1, 1);
            delay(3);
            EEPROM.commit();
            delay(3);
        }
    }

    if (EEPROM.read(0) && EEPROM.read(1))
    {
        xh = EEPROM.read(2) + 1;
        EEPROM.write(0, 0);
        delay(2);
        EEPROM.write(1, 0);
        delay(2);
        EEPROM.write(2, xh);
        delay(2);
        EEPROM.commit();
        delay(2);
    }
}

// 地址5：记录屏幕自动息屏时间
/**
 * @brief 记录屏幕自动息屏时间 在eeprom的地址为 5
 *
 * @param data 自动息屏时间  最大 255s
 */
void EE_LcdSleep(uint8_t data)

{
    EEPROM.write(5, data);
    delay(3);
    EEPROM.commit();
    delay(3);
}

/**
 * @brief 写入蓝牙的休眠时间 在eeprom的6和7地址  地址6为高八位 地址7为低八位
 *
 * @param data 蓝牙的休眠时间
 */
void EE_BLETimeWrite(uint16_t data)

{
    EEPROM.write(6, data >> 8);
    delay(2);
    EEPROM.write(7, data & 0xFF);
    delay(2);
    EEPROM.commit();
}

/**
 * @brief 读取eeprom的6和7地址的数值用于蓝牙的休眠时间
 *
 * @return uint16_t 返回蓝牙的休眠时间
 */
uint16_t EE_BLETimeRead()
{
    uint16_t data = 0;
    data = EEPROM.read(6) << 8;
    data |= EEPROM.read(7);
    return data;
}

/**
 * @brief 写入屏幕方向 在eeprom的地址为 3
 *
 * @param data 屏幕方向
 */
void EE_ScreenDirection(uint8_t data)

{
    EEPROM.write(3, data);
    delay(3);
    EEPROM.commit();
    delay(3);
}

/**
 * @brief 写入屏幕主题编号 在eeprom的地址为 4
 *
 * @param data 主题编号
 */
void EE_Them(uint8_t data)

{
    EEPROM.write(4, data);
    delay(3);
    EEPROM.commit();
    delay(3);
}

/**
 * @brief 记录小电流状态   地址  8
 *
 * @param data    小电流状态
 */
void EE_SmallA(uint8_t data)

{
    EEPROM.write(8, data);
    delay(3);
    EEPROM.commit();
    delay(3);
}

/**
 * @brief 写入四位的十进制数密码到eeprom的9和10地址  地址9为高八位 地址10为低八位
 *
 * @param data 4位的十进制数密码
 */
void EE_PassWrite(uint16_t data)
{
    EEPROM.write(9, data >> 8);
    delay(2);
    EEPROM.write(10, data & 0xFF);
    delay(2);
    EEPROM.commit();
}

/**
 * @brief 读取密码的值
 *
 */
uint16_t EE_PassRead()
{
    uint16_t data = 0;
    data = EEPROM.read(9) << 8;
    data |= EEPROM.read(10);
    return data;
}

/**
 * @brief 写1 OTA升级   EEPROM 地址 11
 *
 * @param data   记录 1    OTA升级
 */
void EE_OTA(uint8_t data) // OTA    eeprom: 11
{
    EEPROM.write(11, data);
    delay(3);
    EEPROM.commit();
    delay(3);
}
/**
 * @brief 地址12：写1 锁死 // 废掉ESP32
 *
 * @param data
 */
void EE_IDLock(uint8_t data) // 写1 锁死 // 废掉ESP32    eeprom: 12
{
    EEPROM.write(12, data);
    delay(3);
    EEPROM.commit();
    delay(3);
}
/**
 * @brief // eeprom: 13 IO4引脚按键控制
 *
 * @param data
 */
void EE_IO4()
{
    if (EEPROM.read(13) == 0)
        EEPROM.write(13, 1);
    else if (EEPROM.read(13) == 1)
        EEPROM.write(13, 0);
    else // Flash 初始状态为255
        EEPROM.write(13, 0);
    EEPROM.commit();
    delay(3);
}
