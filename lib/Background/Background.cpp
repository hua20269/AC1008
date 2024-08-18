#include <esp32gfx.h>
#include <Background.h>
#include <font1.h>
#include <imge.h>
#include <eeprom32.h>
LGFX lcd;                  // 主显示对象
LGFX_Sprite sprite1(&lcd); // 内存画板
// LGFX_Sprite sprite2(&sprite1);
int color = TFT_WHITE;                                                          // 默认白色  调用更改使用
int colorRed = TFT_RED;                                                         // 默认红色  调用更改使用
const int pink1 = lcd.color565(225, 153, 192);                                  // 粉色
const int violef1 = lcd.color565(114, 8, 188);                                  // 深紫色
const int green1 = lcd.color565(137, 230, 146);                                 // 深紫色
const int green2 = lcd.color565(44, 33, 189);                                   // 框2的颜色
const int green3 = lcd.color565(83, 161, 180);                                  // 框1的颜色
const int gre = lcd.color565(34, 208, 186);                                     // 深紫色
const int lv1 = lcd.color565(31, 143, 71);                                      // 深紫色
const int qianhui = lcd.color565(30, 30, 30);                                   // 浅灰色
const int lvse1 = lcd.color565(207, 244, 210);                                  // 绿色
const int lvse2 = lcd.color565(123, 228, 149);                                  // 绿色
const int lvse3 = lcd.color565(86, 197, 150);                                   // 绿色
const int lvse4 = lcd.color565(50, 157, 156);                                   // 绿色
const int lvse5 = lcd.color565(32, 80, 114);                                    // 绿色
int x11 = 0, y11 = 0, x22 = 90, y22 = 0, wh1 = 24, w11 = 25, h11 = 25, r11 = 3; // 位置定位
int fontsize1 = 20, space = 2;
uint16_t ic_ntc = 0;

void DisplayInit() // 屏幕初始化
{
    // 显示库初始化函数
    lcd.init();
    // 设置屏幕显示方向
    if (EEPROM.read(3) == 3)
    {
        lcd.setRotation(3);
    }
    else
        lcd.setRotation(1);
    // if (EEPROM.read(11) == 1) // OTA 更新  给WiFi退让电流
    //     lcd.setBrightness(250);//调暗
    // else
    lcd.setBrightness(200); // 设置屏幕显示亮度
    // 初始化画布及清屏
    // sprite1.createSprite(240, 135); // 设置屏幕大小
    // sprite1.fillScreen(TFT_BLACK);  // 黑色背景
    // sprite1.pushSprite(0, 0);
}

void LcdRotation() // 连接蓝牙实时刷新屏幕方向
{
    if (EEPROM.read(3) == 3)
        lcd.setRotation(3);
    else
        lcd.setRotation(1);
}

void offscreen() //  lcd.sleep  息屏
{
    lcd.sleep();
    lcd.setBrightness(255); // 息屏
}

void onscreen() // 唤醒屏幕
{
    lcd.wakeup();
    lcd.setBrightness(170);
}

void RefreshDisplay() // 刷新画布到屏幕
{
    sprite1.pushSprite(0, 0);
    // sprite1.deleteSprite();
}

void sys_init(uint16_t timenum)
{
    char draw_num[5];
    sprite1.createSprite(240, 135); // 创建画布大小
    sprite1.fillScreen(TFT_BLACK);  // 设置背景颜色
    sprite1.setTextColor(TFT_GREEN);
    sprite1.loadFont(alibb);
    sprite1.setTextDatum(CC_DATUM); // 字体居中
    sprite1.drawString("The system is initializing", 120, 40);
    sprite1.drawRoundRect(66, 53, 110, 30, 1, TFT_WHITE);
    sprite1.drawRoundRect(67, 54, 108, 28, 1, TFT_WHITE);
    sprite1.drawRoundRect(68, 55, 106, 26, 1, TFT_WHITE);
    sprite1.fillRoundRect(71, 58, timenum, 20, 3, TFT_WHITE);
    sprintf(draw_num, "%d%%", timenum);
    sprite1.drawString(draw_num, 120, 68);
    sprite1.pushSprite(0, 0); // 显示在画布1上
    sprite1.deleteSprite();   // 删除精灵
    sprite1.unloadFont();     // 释放加载字体
}

void PowerLOGO(String imgName) // 开机LOGO
{
    sprite1.createSprite(240, 135); // 创建画布大小

    if (imgName != "")
    {
        if (imgName == "imgZhongGuoLianTong")
        {
            sprite1.drawPng(imgZhongGuoLianTong, sizeof(imgZhongGuoLianTong), 0, 0); // 中国联通
            sprite1.pushSprite(0, 0);                                                // 显示在画布1上
            vTaskDelay(3000);                                                        // LOGO显示时间
        }
        if (imgName == "imgLinJieJie")
        {
            sprite1.drawPng(imgLinJieJie, sizeof(imgLinJieJie), 0, 0);
            sprite1.pushSprite(0, 0);
            vTaskDelay(3000);
        }
        if (imgName == "imgZiQiDongLai")
        {
            sprite1.drawPng(imgZiQiDongLai, sizeof(imgZiQiDongLai), 0, 0);
            sprite1.pushSprite(0, 0);
            vTaskDelay(3000);
        }
        if (imgName == "imgZhiChuBao")
        {
            sprite1.drawPng(imgZhiChuBao, sizeof(imgZhiChuBao), 0, 0);
            sprite1.pushSprite(0, 0);
            vTaskDelay(1000);
        }
        if (imgName == "imgShunFeng")
        {
            sprite1.drawPng(imgShunFeng, sizeof(imgShunFeng), 0, 0);
            sprite1.pushSprite(0, 0);
            vTaskDelay(5000);
        }
        if (imgName == "imgYiWangQinSheng")
        {
            sprite1.drawPng(imgYiWangQinSheng, sizeof(imgYiWangQinSheng), 0, 0);
            sprite1.pushSprite(0, 0);
            vTaskDelay(3000);
        }
    }

    sprite1.deleteSprite(); // 删除精灵
    sprite1.unloadFont();   // 释放加载字体
}

/**
 * @brief 主题1  经典主题  HUA
 *
 * @param bat_v 电池电压
 * @param sys_v 系统电压
 * @param sys_a 系统电流
 * @param ic_temp ic温度
 * @param ntc_temp 电池温度
 * @param bat_m 实时容量mAH
 * @param bat_per 电量百分比
 * @param sys_state 充放电状态
 * @param ac_state 端口状态
 * @param protocol 快充协议  
 * @param smalla 小电流状态
 * @param ble_state 蓝牙状态
 * @param cycle 循环次数
 */
void Theme1(float bat_v, float sys_v, float sys_a, float ic_temp, float ntc_temp, float bat_m,
            uint8_t bat_per, uint8_t sys_state, uint8_t ac_state, uint8_t protocol, uint8_t smalla, uint8_t ble_state, uint8_t cycle)
{
    char num_n[10];
    float a = 0, b = 0;
    int bat = 0;
    if (bat_per > 100)
        bat_per = 100;
    if (bat_per < 0)
        bat_per = 0;

    if (bat_per >= 0 && bat_per < 15)
        bat = 7;
    if (bat_per >= 15 && bat_per < 30)
        bat = 6;
    if (bat_per >= 30 && bat_per < 45)
        bat = 5;
    if (bat_per >= 45 && bat_per < 60)
        bat = 4;
    if (bat_per >= 60 && bat_per < 75)
        bat = 3;
    if (bat_per >= 75 && bat_per < 90)
        bat = 2;
    if (bat_per >= 90 && bat_per <= 100)
        bat = 1;
    sprite1.createSprite(240, 135); // 设置精灵画布大小    //横屏  240  135
    sprite1.fillScreen(TFT_BLACK);  // 背景填充

    sprite1.loadFont(KaiTi22);      // 字体
    sprite1.setTextDatum(TL_DATUM); // 字体左上角为原点

    /**
     *  第一列 固定
     */
    sprite1.fillRoundRect(0, 0, 24, 24, 5, TFT_CYAN);
    sprite1.fillRoundRect(0, 27, 24, 24, 5, TFT_YELLOW);
    sprite1.fillRoundRect(0, 54, 24, 24, 5, TFT_PINK);
    sprite1.fillRoundRect(0, 81, 24, 24, 5, TFT_YELLOW);
    sprite1.fillRoundRect(0, 111, 24, 24, 5, TFT_PINK);
    sprite1.setTextColor(TFT_BLACK);
    sprite1.drawString("温", 1, 1);
    sprite1.drawString("压", 1, 28);
    sprite1.drawString("流", 1, 55);
    sprite1.drawString("功", 1, 82);
    sprite1.drawString("协", 1, 112);

    /**
     * 第二列 固定
     */
    sprite1.fillRoundRect(90, 0, 24, 24, 5, TFT_PINK);
    sprite1.fillRoundRect(90, 27, 24, 24, 5, TFT_PINK);
    sprite1.fillRoundRect(90, 54, 24, 24, 5, TFT_YELLOW);
    sprite1.fillRoundRect(90, 81, 24, 24, 5, TFT_PINK);
    sprite1.fillRoundRect(90, 111, 24, 24, 5, TFT_CYAN);
    sprite1.drawString("电", 91, 1);
    sprite1.drawString("池", 91, 28);
    sprite1.drawString("循", 90, 112);
    sprite1.loadFont(JianTi26);
    sprite1.drawString("A", 94, 55);
    sprite1.drawString("C", 94, 82);

    /**
     * 第一列  实时数据
     */
    sprite1.loadFont(JianTi20);
    sprite1.setTextColor(TFT_WHITE);
    sprintf(num_n, "%.1f℃", ntc_temp); // 电池温度
    sprite1.drawString(num_n, 27, 4);
    sprintf(num_n, "%.2fV", sys_v); // 压
    sprite1.drawString(num_n, 27, 31);
    sprintf(num_n, "%.2fA", sys_a); // 流
    sprite1.drawString(num_n, 27, 58);
    sprintf(num_n, "%.1fw", sys_v * sys_a); // 功
    sprite1.drawString(num_n, 27, 85);
    // 快充协议
    // bit 3-0 快充指示
    // 0：None   1：QC2   2：QC3   3：QC3+   4：FCP   5：SCP   6：PD FIX   7：PD PPS   8：PE 1.1   9：PE 2.0   10：VOOC 1.0   11：VOOC 4.0   12：SuperVOOC   13：SFCP   14：AFC   15：UFCS
    // if (sys_state == 2) // 充电
    if (sys_state == 2 || sys_state == 1) // 充电
    {
        switch (protocol) // 快充协议
        {
        case 0:
            sprite1.drawString("None", 27, 115);
            break;
        case 1:
            sprite1.drawString("QC2.0", 27, 115);
            break;
        case 2:
            sprite1.drawString("QC3.0", 27, 115);
            break;
        case 3:
            sprite1.drawString("QC3+", 27, 115);
            break;
        case 4:
            sprite1.drawString("FCP", 27, 115);
            break;
        case 5:
            sprite1.drawString("SCP", 27, 115);
            break;
        case 6:
            sprite1.drawString("FIX", 27, 115);
            break;
        case 7:
            sprite1.drawString("PPS", 27, 115);
            break;
        case 8:
            sprite1.drawString("PE1.1", 27, 115);
            break;
        case 9:
            sprite1.drawString("PE2.0", 27, 115);
            break;
        case 10:
            sprite1.drawString("VOOC1", 27, 115);
            break;
        case 11:
            sprite1.drawString("VOOC4", 27, 115);
            break;
        case 12:
            sprite1.drawString("SVOOC", 27, 115);
            break;
        case 13:
            sprite1.drawString("SFCP", 27, 115);
            break;
        case 14:
            sprite1.drawString("AFC", 27, 115);
            break;
        case 15:
            sprite1.drawString("UFCS", 27, 115);
            break;

        default:
            break;
        }
        // }
        // else if (sys_state == 1) // 放电
        // {
        // switch (  protocol) // 快放协议
        // {
        // case 0:
        //     sprite1.drawString("None", 27, 115);
        //     break;
        // case 1:
        //     sprite1.drawString("PD2.0", 27, 115);
        //     break;
        // case 2:
        //     sprite1.drawString("PPS", 27, 115);
        //     break;
        // case 3:
        //     sprite1.drawString("QC2.0", 27, 115);
        //     break;
        // case 4:
        //     sprite1.drawString("QC3.0", 27, 115);
        //     break;
        // case 5:
        //     sprite1.drawString("FCP", 27, 115);
        //     break;
        // case 6:
        //     sprite1.drawString("PE2/1", 27, 115);
        //     break;
        // case 7:
        //     sprite1.drawString("SFCP", 27, 115);
        //     break;
        // case 8:
        //     sprite1.drawString("AFC", 27, 115);
        //     break;
        // case 9:
        //     sprite1.drawString("SCP", 27, 115);
        //     break;
        // case 10:
        //     sprite1.drawString("PD3.0", 27, 115);
        //     break;
        // default:
        //     break;
        // }
    }
    else
        sprite1.drawString("NULL", 27, 115);
    /**
     * 第二列 实时数据
     */
    sprintf(num_n, "%.2fV", bat_v); // 电池电压
    sprite1.drawString(num_n, 117, 4);
    sprintf(num_n, "%.2fA", sys_v * sys_a / bat_v); // 电池电流
    sprite1.drawString(num_n, 117, 31);

    // 0:空闲   1:C2   2:C1   3:C1C2   4:A2   5:A2C2   6:A2C1   7:A2C1C2   8:A1   9:A1C2   10/A:A1C1   11/B:A1C1C2   12/C:A1A2   13/D:A1A2C2   14/E:A1A2C1   15/F:A1A2C1C2
    // A1/A2口状态
    if (ac_state >= 4)
        sprite1.drawString("ON", 117, 58);
    else
        sprite1.drawString("OFF", 117, 58);
    // C1口状态
    if (ac_state == 2 || ac_state == 3 || ac_state == 6 || ac_state == 7 || ac_state == 10 || ac_state == 11 || ac_state == 14 || ac_state == 15)
        sprite1.drawString("ON", 117, 85);
    else
        sprite1.drawString("OFF", 117, 85);

    // 循环次数
    sprintf(num_n, "%d", cycle);
    sprite1.drawString(num_n, 117, 115); // 循环次数

    if (ble_state) // 蓝牙
    {
        sprite1.drawPng(imgbleimg24, sizeof(imgbleimg24), 156, 111); // 蓝牙图标
    }

    /**
     * 右侧电池部分
     */
    sprite1.loadFont(JianTi26);
    sprite1.setTextColor(TFT_WHITE);
    sprite1.setTextDatum(CC_DATUM); // 字体居中
    // 电量百分比   右上角
    if (bat_per < 10)
    {
        sprite1.setTextColor(TFT_RED); // 小于10   改为红色字体
    }
    sprintf(num_n, "%d", bat_per);
    sprite1.drawString(num_n, 211, 17); // 电池百分比显示
    /**
     * 大电池图标
     */
    if (smalla == 1) // 小电流打开  电池框变红色
    {
        sprite1.fillRoundRect(208 - 6, 33, 20, 4, 2, TFT_RED); // 电池框
        sprite1.drawRoundRect(198 - 6, 36, 40, 66, 7, TFT_RED);
        sprite1.drawRoundRect(199 - 6, 37, 38, 64, 7, TFT_RED);
    }
    else
    {
        sprite1.fillRoundRect(208 - 6, 33, 20, 4, 2, TFT_GREEN); // 电池框
        sprite1.drawRoundRect(198 - 6, 36, 40, 66, 7, TFT_GREEN);
        sprite1.drawRoundRect(199 - 6, 37, 38, 64, 7, TFT_GREEN);
    }
    switch (bat)
    {
    case 1:
        sprite1.fillRoundRect(201 - 6, 39, 34, 8, 2, green3);
    case 2:
        sprite1.fillRoundRect(201 - 6, 48, 34, 8, 2, green2);
    case 3:
        sprite1.fillRoundRect(201 - 6, 57, 34, 8, 2, green1);
    case 4:
        sprite1.fillRoundRect(201 - 6, 66, 34, 8, 2, TFT_GOLD);
    case 5:
        sprite1.fillRoundRect(201 - 6, 75, 34, 8, 2, TFT_MAGENTA);
    case 6:
        sprite1.fillRoundRect(201 - 6, 84, 34, 8, 2, TFT_DARKGREY);
    case 7:
        sprite1.fillRoundRect(201 - 6, 92, 34, 8, 2, TFT_RED);
        break;
    default:
        break;
    }
    /**
     * 以下 充放电 及 A  C  口 状态信息------------------------------------------------------------------------------------------
     *   0 放电  1 充电    0x4
     */
    // 状态
    sprite1.loadFont(KaiTi18); // 设置字体
    sprite1.setTextColor(TFT_WHITE);

    if (sys_state == 1)
    {
        sprite1.drawString("放电中", 210, 124);
        // sprite1.fillRoundRect(123, 0, 30, 30, 5, TFT_RED); // 3
    }
    else if (sys_state == 2)
    {
        sprite1.drawString("充电中", 210, 124);
        // sprite1.fillRoundRect(123, 0, 30, 30, 5, TFT_RED); // 3
    }
    else
    {
        // sprite1.fillRoundRect(123, 0, 30, 30, 5, TFT_GREEN); // 3
        sprite1.drawString("空闲", 210, 124); // 态
    }

    //-----------------------------------------------------------------------
    // sprite1.drawLine(0, 108, 240, 108, TFT_PINK); // 下方分割直线  两点确定一条直线，两点坐标
    sprite1.drawFastHLine(0, 108, 240, TFT_PINK); // 下方分割横线  坐标  ,宽度
    // sprite1.drawFastVLine(88, 0, 135, TFT_PINK);  // 分割竖线  坐标  ,高度

    sprite1.pushSprite(0, 0); // 显示在led画布上
    sprite1.deleteSprite();   // 删除精灵
    sprite1.unloadFont();     // 释放字库
}

/**
 * @brief 主题2  极简主题  貹
 *
 * @param bat_v 电池电压
 * @param sys_v 系统电压
 * @param sys_a 系统电流
 * @param ic_temp ic温度
 * @param ntc_temp 电池温度
 * @param bat_m 实时容量mAH
 * @param bat_per 电量百分比
 * @param sys_state 充放电状态
 * @param ac_state 端口状态
 * @param protocol 快充协议  
 * @param smalla 小电流状态
 * @param ble_state 蓝牙状态
 * @param cycle 循环次数
 */
void Theme2(float bat_v, float sys_v, float sys_a, float ic_temp, float ntc_temp, float bat_m,
            uint8_t bat_per, uint8_t sys_state, uint8_t ac_state, uint8_t protocol, uint8_t smalla, uint8_t ble_state, uint8_t cycle)
{
    char draw_num[20];
    if (bat_per > 100)
        bat_per = 100;
    if (bat_per < 0)
        bat_per = 0;
    sprite1.createSprite(240, 135); // 创建画布大小
    sprite1.fillScreen(TFT_BLACK);  // 设置背景颜色
    // sprite1.setTextDatum(TL_DATUM); // 字体左上角
    sprite1.setTextDatum(CC_DATUM);                        // 字体居中
    sprite1.setTextColor(TFT_BLACK);                       // 设置字体颜色
    sprite1.fillRoundRect(0, 0, 135, 135, 5, qianhui);     // 左背景
    sprite1.fillRoundRect(137, 0, 103, 135, 5, qianhui);   // 右背景
    sprite1.fillRoundRect(5, 29, 125, 33, 5, TFT_RED);     // 压背景
    sprite1.fillRoundRect(5, 64, 125, 33, 5, TFT_GOLD);    // 流背景
    sprite1.fillRoundRect(5, 101, 125, 33, 5, violef1);    // 功背景
    sprite1.fillRoundRect(140, 44, 27, 27, 5, TFT_CYAN);   // 右1
    sprite1.fillRoundRect(140, 75, 27, 27, 5, TFT_GOLD);   // 右2
    sprite1.fillRoundRect(140, 106, 27, 27, 5, TFT_GREEN); // 右3

    // AC_State
    sprite1.fillRoundRect(1, 0, 25, 25, 5, lvse4);   // A1
    sprite1.fillRoundRect(28, 0, 25, 25, 5, lvse4);  // A2
    sprite1.fillRoundRect(82, 0, 25, 25, 5, lvse4);  // C1
    sprite1.fillRoundRect(109, 0, 25, 25, 5, lvse4); // L(C2)

    switch (ac_state)
    {
    case 0:
        break;
    case 1:
        sprite1.fillRoundRect(109, 0, 25, 25, 5, TFT_PINK); // L(C2)
        break;
    case 2:
        sprite1.fillRoundRect(82, 0, 25, 25, 5, TFT_PINK); // C1
        break;
    case 3:
        sprite1.fillRoundRect(82, 0, 25, 25, 5, TFT_PINK);  // C1
        sprite1.fillRoundRect(109, 0, 25, 25, 5, TFT_CYAN); // L(C2)
        break;
    case 4:
        sprite1.fillRoundRect(28, 0, 25, 25, 5, TFT_CYAN); // A2
        break;
    case 5:
        sprite1.fillRoundRect(28, 0, 25, 25, 5, TFT_PINK);  // A2
        sprite1.fillRoundRect(109, 0, 25, 25, 5, TFT_CYAN); // L(C2)
        break;
    case 6:
        sprite1.fillRoundRect(28, 0, 25, 25, 5, TFT_PINK); // A2
        sprite1.fillRoundRect(82, 0, 25, 25, 5, TFT_CYAN); // C1
        break;
    case 7:
        sprite1.fillRoundRect(28, 0, 25, 25, 5, TFT_CYAN);  // A2
        sprite1.fillRoundRect(82, 0, 25, 25, 5, TFT_PINK);  // C1
        sprite1.fillRoundRect(109, 0, 25, 25, 5, TFT_CYAN); // L(C2)
        break;
    case 8:
        sprite1.fillRoundRect(1, 0, 25, 25, 5, TFT_PINK); // A1
        break;
    case 9:
        sprite1.fillRoundRect(1, 0, 25, 25, 5, TFT_PINK);   // A1
        sprite1.fillRoundRect(109, 0, 25, 25, 5, TFT_CYAN); // L(C2)
        break;
    case 10:
        sprite1.fillRoundRect(1, 0, 25, 25, 5, TFT_PINK);  // A1
        sprite1.fillRoundRect(82, 0, 25, 25, 5, TFT_CYAN); // C1
        break;
    case 11:
        sprite1.fillRoundRect(1, 0, 25, 25, 5, TFT_CYAN);   // A1
        sprite1.fillRoundRect(82, 0, 25, 25, 5, TFT_CYAN);  // C1
        sprite1.fillRoundRect(109, 0, 25, 25, 5, TFT_PINK); // L(C2)
        break;
    case 12:
        sprite1.fillRoundRect(1, 0, 25, 25, 5, TFT_PINK);  // A1
        sprite1.fillRoundRect(28, 0, 25, 25, 5, TFT_CYAN); // A2
        break;
    case 13:
        sprite1.fillRoundRect(1, 0, 25, 25, 5, TFT_PINK);   // A1
        sprite1.fillRoundRect(28, 0, 25, 25, 5, TFT_CYAN);  // A2
        sprite1.fillRoundRect(109, 0, 25, 25, 5, TFT_PINK); // L(C2)
        break;
    case 14:
        sprite1.fillRoundRect(1, 0, 25, 25, 5, TFT_PINK);  // A1
        sprite1.fillRoundRect(28, 0, 25, 25, 5, TFT_CYAN); // A2
        sprite1.fillRoundRect(82, 0, 25, 25, 5, TFT_PINK); // C1
        break;
    case 15:
        sprite1.fillRoundRect(1, 0, 25, 25, 5, TFT_PINK);   // A1
        sprite1.fillRoundRect(28, 0, 25, 25, 5, TFT_CYAN);  // A2
        sprite1.fillRoundRect(82, 0, 25, 25, 5, TFT_PINK);  // C1
        sprite1.fillRoundRect(109, 0, 25, 25, 5, TFT_CYAN); // L(C2)
        break;
    default:
        break;
    }

    if (bat_per <= 5)
    {
        sprite1.drawPng(dianchiicon0, sizeof(dianchiicon0), 139, 1);
    }
    else if (bat_per > 5 && bat_per <= 25)
    {
        sprite1.drawPng(dianchiicon1, sizeof(dianchiicon1), 139, 1);
    }
    else if (bat_per > 25 && bat_per <= 50)
    {
        sprite1.drawPng(dianchiicon2, sizeof(dianchiicon2), 139, 1);
    }
    else if (bat_per > 50 && bat_per <= 75)
    {
        sprite1.drawPng(dianchiicon3, sizeof(dianchiicon3), 139, 1);
    }
    else if (bat_per > 75 && bat_per <= 100)
    {
        sprite1.drawPng(dianchiicon4, sizeof(dianchiicon4), 139, 1);
    }
    if (ble_state)
    {
        sprite1.drawPng(lanya, sizeof(lanya), 210, 108);
    }
    sprite1.loadFont(huxiaobo);
    sprite1.drawString("池", 154, 58);  // 上5
    sprite1.drawString("系", 154, 89);  // 上5
    sprite1.drawString("循", 154, 120); // 上5
    sprite1.setTextColor(TFT_CYAN);     // 设置字体颜色
    sprintf(draw_num, "%.1f℃", ntc_temp);
    sprite1.drawString(draw_num, 203, 58); // 上5

    sprite1.setTextColor(TFT_GOLD); // 协议
    // 快充协议
    // bit 3-0 快充指示
    // 0：None   1：QC2   2：QC3   3：QC3+   4：FCP   5：SCP   6：PD FIX   7：PD PPS   8：PE 1.1   9：PE 2.0   10：VOOC 1.0   11：VOOC 4.0   12：SuperVOOC   13：SFCP   14：AFC   15：UFCS
    // if (sys_state == 2) // 充电
    if (sys_state == 2 || sys_state == 1) // 充电
    {
        switch (protocol) // 快充协议
        {
        case 0:
            sprite1.drawString("None", 203, 89);
            break;
        case 1:
            sprite1.drawString("QC2.0", 203, 89);
            break;
        case 2:
            sprite1.drawString("QC3.0", 203, 89);
            break;
        case 3:
            sprite1.drawString("QC3+", 203, 89);
            break;
        case 4:
            sprite1.drawString("FCP", 203, 89);
            break;
        case 5:
            sprite1.drawString("SCP", 203, 89);
            break;
        case 6:
            sprite1.drawString("FIX", 203, 89);
            break;
        case 7:
            sprite1.drawString("PPS", 203, 89);
            break;
        case 8:
            sprite1.drawString("PE1.1", 203, 89);
            break;
        case 9:
            sprite1.drawString("PE2.0", 203, 89);
            break;
        case 10:
            sprite1.drawString("VOOC1", 203, 89);
            break;
        case 11:
            sprite1.drawString("VOOC4", 203, 89);
            break;
        case 12:
            sprite1.drawString("SVOOC", 203, 89);
            break;
        case 13:
            sprite1.drawString("SFCP", 203, 89);
            break;
        case 14:
            sprite1.drawString("AFC", 203, 89);
            break;
        case 15:
            sprite1.drawString("UFCS", 203, 89);
            break;

        default:
            break;
        }
    }
    else
        sprite1.drawString("NULL", 203, 89);

    // sprite1.setTextColor(TFT_GOLD);        // 设置字体颜色
    // sprintf(draw_num, "%.1f℃", ic_ntc);
    // sprite1.drawString(draw_num, 203, 89); // 上5
    sprite1.setTextColor(TFT_GREEN); // 设置字体颜色
    sprintf(draw_num, "%d", cycle);
    sprite1.drawString(draw_num, 185, 120); // 上5
    sprite1.setTextColor(TFT_BLACK);        // 设置字体颜色
    sprite1.loadFont(alibb);                // 字体设置
    sprite1.drawString("A1", 14, 13);       // 上1
    sprite1.drawString("A2", 41, 13);
    // sprite1.drawString("B", 68, 13);
    sprite1.drawString("C", 95, 13);
    sprite1.drawString("L", 122, 13); // 上5
    sprite1.loadFont(alibb40);        // 字体设置
    sprite1.setTextColor(TFT_WHITE);  // 设置字体颜色
    sprintf(draw_num, "%.2fV", sys_v);
    sprite1.drawString(draw_num, 65, 45);
    sprintf(draw_num, "%.2fA", sys_a);
    sprite1.drawString(draw_num, 65, 80);
    sprintf(draw_num, "%.1fW", sys_v * sys_a);
    sprite1.drawString(draw_num, 65, 117);

    sprite1.pushSprite(0, 0); // 显示在画布1上
    sprite1.deleteSprite();   // 删除精灵
    sprite1.unloadFont();     // 释放加载字体
}

/**
 * @brief 主题3  win10风格  小东
 *
 * @param bat_v 电池电压
 * @param sys_v 系统电压
 * @param sys_a 系统电流
 * @param ic_temp ic温度
 * @param ntc_temp 电池温度
 * @param bat_m 实时容量mAH
 * @param bat_per 电量百分比
 * @param sys_state 充放电状态
 * @param ac_state 端口状态
 * @param protocol 快充协议  
 * @param smalla 小电流状态
 * @param ble_state 蓝牙状态
 * @param cycle 循环次数
 *
 * @param year   年
 * @param month  月
 * @param day    日
 * @param hour   时
 * @param minute 分
 * @param sec    秒
 * @param week   周
 *
 */
void Theme3(float bat_v, float sys_v, float sys_a, float ic_temp, float ntc_temp, float bat_m,
            uint8_t bat_per, uint8_t sys_state, uint8_t ac_state, uint8_t protocol, uint8_t smalla, uint8_t ble_state, uint8_t cycle,
            uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t sec, uint8_t week)
{
    char draw_num[20];
    if (bat_per > 100)
        bat_per = 100;
    if (bat_per < 0)
        bat_per = 0;
    sprite1.createSprite(240, 135);                    // 创建画布大小
    sprite1.fillScreen(TFT_BLACK);                     // 设置背景颜色
    sprite1.loadFont(huxiaobo);                        // 字体设置
    sprite1.setTextColor(TFT_WHITE);                   // 设置字体颜色
    sprite1.drawPng(dianchi3, sizeof(dianchi3), 0, 2); // 电池3
    sprite1.setTextDatum(CC_DATUM);                    // 字体居中
    sprite1.fillRoundRect(4, 6, 29, 17, 1, lv1);
    sprintf(draw_num, "%d", bat_per);
    sprite1.drawString(draw_num, 18, 15);
    sprite1.fillRoundRect(206, 0, 30, 30, 5, TFT_GREEN);      // 周
    sprite1.fillRoundRect(0, 38, 62, 25, 3, pink1);           // 中1
    sprite1.fillRoundRect(68, 38, 62, 25, 3, TFT_GOLD);       // 2
    sprite1.fillRoundRect(138, 38, 62, 25, 3, TFT_GREEN);     // 3
    sprite1.fillRoundRect(206, 35, 30, 30, 5, TFT_CYAN);      // A
    sprite1.fillRoundRect(0, 70, 62, 25, 3, TFT_CYAN);        // 电池电压
    sprite1.fillRoundRect(68, 70, 75, 25, 3, TFT_GREEN);      // 容量
    sprite1.fillRoundRect(151, 70, 48, 25, 3, TFT_BROWN);     // 7
    sprite1.fillRoundRect(206, 69, 30, 30, 5, TFT_GREEN);     // C
    sprite1.fillRoundRect(0, 104, 30, 30, 5, TFT_PINK);       // 电池温度图标
    sprite1.fillRoundRect(80, 104, 30, 30, 5, TFT_LIGHTGRAY); // 循环
    // sprite1.drawFastVLine(200, 0, 30, TFT_WHITE);         // 线条竖
    sprite1.setTextColor(TFT_BLACK); // 设置字体颜色
    switch (week)
    {
    case 1:
        sprite1.drawString("一", 221, 15);
        break;
    case 2:
        sprite1.drawString("二", 221, 15);
        break;
    case 3:
        sprite1.drawString("三", 221, 15);
        break;
    case 4:
        sprite1.drawString("四", 221, 15);
        break;
    case 5:
        sprite1.drawString("五", 221, 15);
        break;
    case 6:
        sprite1.drawString("六", 221, 15);
        break;
    case 0:
        sprite1.drawString("日", 221, 15);
        break;
    default:
        sprite1.drawString("-", 221, 15);
        break;
    }
    sprite1.drawFastHLine(0, 101, 240, TFT_WHITE);  // 线条横
    sprite1.drawFastVLine(151, 102, 33, TFT_WHITE); // 线条竖
    sprite1.loadFont(alibb);

    // AC端口
    if (ac_state == 1 || ac_state == 4 || ac_state == 5)
    {
        if (ac_state == 1)
        {
            sprite1.fillRoundRect(206, 35, 30, 30, 5, TFT_RED); // A
        }
        if (ac_state == 4)
        {
            sprite1.fillRoundRect(206, 69, 30, 30, 5, TFT_RED); // C
        }
        if (ac_state == 5)
        {
            sprite1.fillRoundRect(206, 35, 30, 30, 5, TFT_RED); // A
            sprite1.fillRoundRect(206, 69, 30, 30, 5, TFT_RED); // C
        }
    }
    // 放下面，盖上面方块
    sprite1.drawString("A", 221, 50);
    sprite1.drawString("C", 221, 84);

    // 充放电状态
    if (sys_state == 1)
        sprite1.drawString("OUT", 175, 83);
    else if (sys_state == 2)
        sprite1.drawString("IN", 175, 83);
    else
        sprite1.drawString("NO", 175, 83); // 态

    // 蓝牙状态
    if (ble_state)
        sprite1.fillRoundRect(47, 0, 30, 30, 5, TFT_RED);
    else
        sprite1.fillRoundRect(47, 0, 30, 30, 5, pink1);
    sprite1.drawPng(lanya3, sizeof(lanya3), 50, 3); // 蓝牙
    sprite1.setTextDatum(TL_DATUM);                 // 字体左上角
    sprintf(draw_num, "%.1fV", sys_v);
    sprite1.drawString(draw_num, 3, 41);
    sprintf(draw_num, "%.2fA", sys_a);
    sprite1.drawString(draw_num, 71, 41);
    sprintf(draw_num, "%.1fW", sys_v * sys_a);
    sprite1.drawString(draw_num, 140, 41);
    sprintf(draw_num, "%.1fV", bat_v);
    sprite1.drawString(draw_num, 3, 73);
    sprintf(draw_num, "%.1fwH", bat_m);
    sprite1.drawString(draw_num, 69, 73);

    //-------------------------------------------------------------------------------------曾经的上下部分分离

    sprite1.setTextDatum(CC_DATUM);  // 字体居中
    sprite1.setTextColor(TFT_BLACK); // 设置字体颜色
    if (smalla == 1)
    {
        sprite1.fillRoundRect(83, 0, 30, 30, 5, TFT_RED); // 3
    }
    else
    {
        sprite1.fillRoundRect(83, 0, 30, 30, 5, TFT_GREEN); // 3
    }
    sprite1.drawPng(chongdian3, sizeof(chongdian3), 84, 1); // 充电
    sprite1.fillRoundRect(120, 0, 80, 30, 5, TFT_GOLD);     // 日期
    sprintf(draw_num, "%02d-%02d", month, day);
    sprite1.drawString(draw_num, 160, 16);
    sprite1.setTextColor(TFT_WHITE);               // 设置字体颜色
    sprite1.drawPng(batt3, sizeof(batt3), 3, 108); // batt
    sprintf(draw_num, "%.0f℃", ntc_temp);
    sprite1.drawString(draw_num, 55, 123);

    // sprite1.drawPng(cput3, sizeof(cput3), 78, 108); // cput
    // sprintf(draw_num, "%.0f℃", temp);
    // sprite1.drawString(draw_num, 128, 121);
    sprintf(draw_num, "%02d:%02d:%02d", hour, minute, sec);
    sprite1.drawString(draw_num, 197, 121);
    sprintf(draw_num, "%d", cycle); // 循环
    sprite1.drawString(draw_num, 130, 121);

    sprite1.loadFont(KaiTi22);
    sprite1.setTextColor(TFT_BLACK);
    sprite1.drawString("循", 94, 120);
    sprite1.pushSprite(0, 0); // 显示在画布1上
    sprite1.deleteSprite();   // 删除精灵
    sprite1.unloadFont();     // 释放加载字体
}

/**
 * @brief 主题4  闪极风格  HUA
 *
 * @param bat_v 电池电压
 * @param sys_v 系统电压
 * @param sys_a 系统电流
 * @param ic_temp ic温度
 * @param ntc_temp 电池温度
 * @param bat_m 实时容量mAH
 * @param bat_per 电量百分比
 * @param sys_state 充放电状态
 * @param ac_state 端口状态
 * @param protocol 快充协议  
 * @param smalla 小电流状态
 * @param ble_state 蓝牙状态
 * @param cycle 循环次数
 */
void Theme4(float bat_v, float sys_v, float sys_a, float ic_temp, float ntc_temp, float bat_m,
            uint8_t bat_per, uint8_t sys_state, uint8_t ac_state, uint8_t protocol, uint8_t smalla, uint8_t ble_state, uint8_t cycle)
{
    char draw_num[20];
    float v = 0, a = 0; // 电压  // 电流
    int bat = 0;
    if (bat_per > 100)
        bat_per = 100;
    if (bat_per < 0)
        bat_per = 0;

    if (bat_per >= 0 && bat_per < 15)
        bat = 7;
    if (bat_per >= 15 && bat_per < 30)
        bat = 6;
    if (bat_per >= 30 && bat_per < 45)
        bat = 5;
    if (bat_per >= 45 && bat_per < 60)
        bat = 4;
    if (bat_per >= 60 && bat_per < 75)
        bat = 3;
    if (bat_per >= 75 && bat_per < 90)
        bat = 2;
    if (bat_per >= 90 && bat_per <= 100)
        bat = 1;

    sprite1.createSprite(240, 135); // 创建画布大小
    sprite1.fillScreen(TFT_BLACK);  // 设置背景颜色
    sprite1.loadFont(KaiTi22);      // 字体设置
    sprite1.setTextDatum(TL_DATUM); // 字体左上角为原点
    /**
     *  上 两行信息
     */
    sprite1.fillRoundRect(0, 0, 24, 24, 5, TFT_CYAN);    // 左上
    sprite1.fillRoundRect(0, 27, 24, 24, 5, TFT_YELLOW); // 坐下
    sprite1.fillRoundRect(90, 0, 24, 24, 5, TFT_PINK);   // 右上
    sprite1.setTextColor(TFT_BLACK);
    sprite1.drawString("电", 1, 1);
    sprite1.drawString("池", 1, 28);
    sprite1.drawString("温", 91, 1);
    sprite1.drawPng(imgbleimg24, sizeof(imgbleimg24), 90, 27); // 蓝牙图标

    /**
     * 上两行值写入
     */
    sprite1.loadFont(JianTi20); // 设置自定义字库 字体
    sprite1.setTextColor(TFT_WHITE);
    sprintf(draw_num, "%.2fV", bat_v); // 电池电压
    sprite1.drawString(draw_num, 27, 4);
    sprintf(draw_num, "%.2fA", sys_v * sys_a / bat_v); // 电池电流   功率/电池电压
    sprite1.drawString(draw_num, 27, 31);
    sprintf(draw_num, "%.2f℃", ntc_temp); // 电池温度NTC
    sprite1.drawString(draw_num, 117, 4);
    if (ble_state) // 蓝牙
    {
        sprite1.drawString("ON", 117, 31);
    }
    else
    {
        sprite1.drawString("OFF", 117, 31);
    }

    /**
     *
     * 屏幕  左下半部分  输入输出实时信息
     */
    //-------------------------------------------------------------输入信息 ------  左下------------------------------------------

    // 输入信息   左
    sprite1.fillRoundRect(0, 58, 36, 36, 5, TFT_CYAN); // 位置，宽高，圆角半径 ，背景色  ////实心圆角矩形
    sprite1.setTextColor(TFT_BLACK);
    sprite1.loadFont(JianTi16);
    sprite1.drawString("IN", 10, 70); // 0,58
    // 充电输入 电压  上
    // sprite1.fillRoundRect(41, 59, 50, 16, 5, TFT_CYAN); // 位置，宽高，圆角半径 ，背景色  ////实心圆角矩形
    sprite1.setTextColor(TFT_WHITE);
    // sys  充电状态  ==2  充电
    if (sys_state == 2) // 充电
    {                   // 充电状态 传值，，否则置零
        v = sys_v;      // 充放电压
        a = sys_a;      // 充放电流
    }
    sprintf(draw_num, "%.2fV", v); // 输入电压  sys_outinv
    sprite1.drawString(draw_num, 43, 59);
    // 充电 输入 电流   下方
    sprite1.fillRoundRect(41, 77, 50, 16, 5, TFT_CYAN); // 位置，宽高，圆角半径 ，背景色  ////实心圆角矩形
    sprite1.setTextColor(TFT_BLACK);
    sprintf(draw_num, "%.2fA", a); // 输入电流
    sprite1.drawString(draw_num, 45, 78);
    // 充电输入  功率   右
    // sprite1.fillRoundRect(96, 70, 80, 24, 5, TFT_WHITE); // 位置，宽高，圆角半径 ，背景色  ////实心圆角矩形
    sprite1.setTextColor(TFT_PINK);
    sprite1.loadFont(JianTi26);
    sprintf(draw_num, "%.2fw", v * a); // 输入功率 = v * a
    sprite1.drawString(draw_num, 96, 70);

    //-------------------------------------------------------------输出信息 ------  左下角------------------------------------------

    // 输出信息   左下
    sprite1.fillRoundRect(0, 99, 36, 36, 5, TFT_PINK); // 位置，宽高，圆角半径 ，背景色  ////实心圆角矩形
    sprite1.setTextColor(TFT_BLACK);
    sprite1.loadFont(JianTi16);
    sprite1.drawString("OUT", 2, 111); //  0,99
    // 输出电压   上
    // sprite1.fillRoundRect(41, 100, 50, 16, 5, TFT_GREENYELLOW); // 位置，宽高，圆角半径 ，背景色  ////实心圆角矩形
    sprite1.setTextColor(TFT_WHITE);
    v = 0;
    a = 0;
    if (sys_state == 1) // 放电
    {                   // 放电状态 传值，，否则置零
        v = sys_v;      // 充放电压
        a = sys_a;      // 充放电流
    }
    sprintf(draw_num, "%.2fV", v); // 输出电压 sys_outinv
    sprite1.drawString(draw_num, 43, 100);
    // 输出电流   下
    sprite1.fillRoundRect(41, 118, 50, 16, 5, TFT_YELLOW); // 实心圆角矩形
    sprite1.setTextColor(TFT_BLACK);
    sprintf(draw_num, "%.2fA", a); // 输出电流
    sprite1.drawString(draw_num, 45, 119);

    sprite1.setTextColor(TFT_CYAN);
    sprite1.loadFont(JianTi26);
    sprintf(draw_num, "%.2fw", v * a); // 输出功率 = v * a
    sprite1.drawString(draw_num, 96, 111);

    /**
     * 右侧电池部分
     */
    sprite1.loadFont(JianTi26);
    sprite1.setTextColor(TFT_WHITE);
    sprite1.setTextDatum(CC_DATUM); // 字体居中
    // 电量百分比   右上角
    if (bat_per < 10)
    {
        sprite1.setTextColor(TFT_RED); // 小于10   改为红色字体
    }
    sprintf(draw_num, "%d", bat_per);
    sprite1.drawString(draw_num, 211, 17); // 电池百分比显示

    /**
     * 大电池图标
     */
    if (smalla == 1) // 小电流打开  电池框变红色
    {
        sprite1.fillRoundRect(208 - 6, 33, 20, 4, 2, TFT_RED); // 电池框
        sprite1.drawRoundRect(198 - 6, 36, 40, 66, 7, TFT_RED);
        sprite1.drawRoundRect(199 - 6, 37, 38, 64, 7, TFT_RED);
    }
    else
    {
        sprite1.fillRoundRect(208 - 6, 33, 20, 4, 2, TFT_PINK); // 电池框
        sprite1.drawRoundRect(198 - 6, 36, 40, 66, 7, TFT_PINK);
        sprite1.drawRoundRect(199 - 6, 37, 38, 64, 7, TFT_PINK);
    }
    switch (bat)
    {
    case 1:
        sprite1.fillRoundRect(201 - 6, 39, 34, 8, 2, green3);
    case 2:
        sprite1.fillRoundRect(201 - 6, 48, 34, 8, 2, green2);
    case 3:
        sprite1.fillRoundRect(201 - 6, 57, 34, 8, 2, green1);
    case 4:
        sprite1.fillRoundRect(201 - 6, 66, 34, 8, 2, TFT_GOLD);
    case 5:
        sprite1.fillRoundRect(201 - 6, 75, 34, 8, 2, TFT_MAGENTA);
    case 6:
        sprite1.fillRoundRect(201 - 6, 84, 34, 8, 2, TFT_DARKGREY);
    case 7:
        sprite1.fillRoundRect(201 - 6, 92, 34, 8, 2, TFT_RED);
        break;
    default:
        break;
    }
    /**
     * 以下 充放电 及 A  C  口 状态信息------------------------------------------------------------------------------------------
     *   0 放电  1 充电    0x4
     */
    // 充放状态   AC状态
    sprite1.loadFont(JianTi26);
    sprite1.setTextColor(TFT_PINK);

    if (sys_state == 1)
    {
        if (ac_state == 1)
        {
            // sprite1.fillRoundRect(210 - 12, 124 - 12, 24, 24, 5, TFT_CYAN);
            sprite1.drawString("A", 210, 124);
        }
        if (ac_state == 4)
        {
            // sprite1.fillRoundRect(210 - 12, 124 - 12, 24, 24, 5, TFT_PINK);
            sprite1.drawString("C", 210, 124);
        }
        if (ac_state == 5)
        {
            // sprite1.fillRoundRect(210 - 5 - 24, 124 - 12, 24, 24, 5, TFT_CYAN);
            // sprite1.fillRoundRect(210 + 5, 124 - 12, 24, 24, 5, TFT_PINK);
            sprite1.drawString("A", 210 - 12, 124);
            sprite1.drawString("C", 210 + 12, 124);
        }
    }
    else if (sys_state == 2)
    {
        if (ac_state == 1)
        {
            // sprite1.fillRoundRect(210 - 12, 124 - 12, 24, 24, 5, TFT_CYAN);
            sprite1.drawString("A", 210, 124);
        }
        if (ac_state == 4)
        {
            // sprite1.fillRoundRect(210 - 12, 124 - 12, 24, 24, 5, TFT_PINK);
            sprite1.drawString("C", 210, 124);
        }
        if (ac_state == 5)
        {
            // sprite1.fillRoundRect(210 - 5 - 24, 124 - 12, 24, 24, 5, TFT_CYAN);
            // sprite1.fillRoundRect(210 + 5, 124 - 12, 24, 24, 5, TFT_PINK);
            sprite1.drawString("A", 210 - 12, 124);
            sprite1.drawString("C", 210 + 12, 124);
        }
    }
    else
    {
        sprite1.loadFont(KaiTi18); // 设置字体
        sprite1.setTextColor(TFT_WHITE);
        sprite1.drawString("空闲", 210, 124); // 态
    }
    sprite1.pushSprite(0, 0); // 显示在画布1上
    sprite1.deleteSprite();   // 删除精灵
    sprite1.unloadFont();     // 释放加载字体
}
/**
 * @brief 主题5   手机风格  HUA
 *
 * @param bat_v 电池电压
 * @param sys_v 系统电压
 * @param sys_a 系统电流
 * @param ic_temp ic温度
 * @param ntc_temp 电池温度
 * @param bat_m 实时容量mAH
 * @param bat_per 电量百分比
 * @param sys_state 充放电状态
 * @param ac_state 端口状态
 * @param protocol 快充协议  
 * @param smalla 小电流状态
 * @param ble_state 蓝牙状态
 * @param cycle 循环次数
 *
 * @param year   年
 * @param month  月
 * @param day    日
 * @param hour   时
 * @param minute 分
 * @param sec    秒
 * @param week   周
 *
 */
void Theme5(float bat_v, float sys_v, float sys_a, float ic_temp, float ntc_temp, float bat_m,
            uint8_t bat_per, uint8_t sys_state, uint8_t ac_state, uint8_t protocol, uint8_t smalla, uint8_t ble_state, uint8_t cycle,
            uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t sec, uint8_t week)
{
    char draw_num[20];
    // float v = 0, a = 0; // 电压  // 电流
    if (bat_per > 100)
        bat_per = 100;
    if (bat_per < 0)
        bat_per = 0;

    sprite1.createSprite(240, 135); // 创建画布大小
    sprite1.fillScreen(TFT_BLACK);  // 设置背景颜色
    // sprite1.loadFont(KaiTi22);      // 字体设置
    sprite1.setTextDatum(TL_DATUM); // 字体左上角显示
    /**
     * 上方 状态栏 ： 时分秒   蓝牙   闪电图标   电量  百分比
     */
    sprite1.setTextColor(TFT_WHITE);
    sprite1.loadFont(JianTi26);
    sprintf(draw_num, "%02d:%02d:%02d", hour, minute, sec); // 时间
    sprite1.drawString(draw_num, 0, 0);

    sprite1.setTextColor(TFT_GREEN);
    sprite1.loadFont(JianTi20_datetime);
    String weeks = "";
    switch (week) // 获取的为数字信息
    {
    case 1:
        weeks = "一";
        break;
    case 2:
        weeks = "二";
        break;
    case 3:
        weeks = "三";
        break;
    case 4:
        weeks = "四";
        break;
    case 5:
        weeks = "五";
        break;
    case 6:
        weeks = "六";
        break;
    case 0:
        weeks = "日";
        break;
    default:
        weeks = "0";
        break;
    }
    sprintf(draw_num, "%d月%d日 周%s", month, day, weeks); // 日期 星期
    sprite1.drawString(draw_num, 0, 24);

    if (ble_state) // 蓝牙
    {
        // sprite1.pushImage(110, 0, 20, 20, imgbleimg20); // 蓝牙图标
        sprite1.drawPng(imgbleimg20, sizeof(imgbleimg20), 110, 1); // 蓝牙图标
    }
    // sprite1.drawPng(imgbleimg20, sizeof(imgbleimg20), 110, 1); // 蓝牙图标

    /**
     * 电量图标 外框
     * 155--193  电量图标长度  高16像素   对齐20像素
     */
    if (smalla == 1) // 小电流模式  电池框改成红色
    {
        sprite1.fillRoundRect(155, 8, 4, 6, 2, TFT_RED);   // 实心，圆角矩形   //正极头  电池帽
        sprite1.drawRoundRect(158, 3, 35, 16, 3, TFT_RED); // 空心，圆角矩形   //框
    }
    else
    {
        sprite1.fillRoundRect(155, 8, 4, 6, 2, TFT_WHITE);   // 实心，圆角矩形   //正极头  电池帽
        sprite1.drawRoundRect(158, 3, 35, 16, 3, TFT_WHITE); // 空心，圆角矩形   //框
    }

    if (sys_state == 2) // 充电
    {
        color = TFT_GREEN;
        colorRed = TFT_GREEN;
        if (sys_v * sys_a > 15 || sys_v > 8)
        {
            sprite1.drawPng(imglvshandian20, sizeof(imglvshandian20), 135, 0); // 快充闪电图标
        }
        else
        {
            sprite1.drawPng(imgbaishandian20, sizeof(imgbaishandian20), 139, 1); // 5v充电闪电图标
        }
    }
    else
    { // else 可不要，但效果不好，延迟
        color = TFT_WHITE;
        colorRed = TFT_RED;
    }
    if (bat_per >= 0 && bat_per < 15)
    {
        sprite1.fillRoundRect(160 + 28, 5, 31 - 28, 12, 1, colorRed); // 位置，宽高，圆角半径 ，背景色
    }
    if (bat_per >= 15 && bat_per < 30)
    {
        sprite1.fillRoundRect(160 + 23, 5, 31 - 23, 12, 1, color); // 位置，宽高，圆角半径 ，背景色
    }
    if (bat_per >= 30 && bat_per < 45)
    {
        sprite1.fillRoundRect(160 + 18, 5, 31 - 18, 12, 1, color); // 位置，宽高，圆角半径 ，背景色
    }
    if (bat_per >= 45 && bat_per < 60)
    {
        sprite1.fillRoundRect(160 + 13, 5, 31 - 13, 12, 1, color); // 位置，宽高，圆角半径 ，背景色
    }
    if (bat_per >= 60 && bat_per < 75)
    {
        sprite1.fillRoundRect(160 + 8, 5, 31 - 8, 12, 1, color); // 位置，宽高，圆角半径 ，背景色
    }
    if (bat_per >= 75 && bat_per < 90)
    {
        sprite1.fillRoundRect(160 + 3, 5, 31 - 3, 12, 1, color); // 位置，宽高，圆角半径 ，背景色
    }
    if (bat_per >= 90 && bat_per <= 100)
    {
        sprite1.fillRoundRect(160, 5, 31, 12, 1, color); // 位置，宽高，圆角半径 ，背景色
    }
    /**
     *  百分比 数字显示  右上角
     */
    sprite1.loadFont(JianTi26);
    sprite1.setTextColor(TFT_WHITE);
    sprite1.setTextDatum(TC_DATUM); // 字体顶部居中
    if (bat_per < 10)               // 小于10    改为红色字体
        sprite1.setTextColor(TFT_RED);
    sprintf(draw_num, "%d", bat_per);     // 传值
    sprite1.drawString(draw_num, 212, 0); // 电量百分比显示

    /**
     *  系统信息  温度  功率
     */
    sprite1.setTextDatum(TL_DATUM);                      // 字体左上角显示
    sprite1.fillRoundRect(0, 45, 24, 24, 5, TFT_YELLOW); // 位置，宽高，圆角半径 ，背景色  ////实心圆角矩形
    sprite1.loadFont(KaiTi22);
    sprite1.setTextColor(TFT_BLACK);
    sprite1.drawString("协", 1, 45 + 1);

    sprite1.loadFont(JianTi26);
    sprite1.setTextColor(TFT_CYAN);
    // 快充协议
    //* bit:7 PD版本指示 0: PD2.0 1: PD3.0   注意此指示只在PD沟通后有效
    //* bit:6-4 充电sink快充协议指示 0: 非快充  * 1: PD sink 2: / 3: HV sink 4: AFC sink 5: FCP sink 6: SCP sink 7: PE1.1 sink  (2:PD3.0)
    //* bit:3-0 放电source快充协议指示 0: 非快充 1: PD source 2: PPS source 3: QC2.0 source 4: QC3.0 source 5: FCP source
    //                          6: PE2.0 /1.1 source 7: SFCP source 8: AFC source 9: SCP source 10-15: reserved(10:PD3.0)
    if (sys_state == 2) // 充电
    {
        switch (protocol) // 快充协议
        {
        case 0:
            sprite1.drawString("NOT", 27, 48 - 2);
            break;
        case 1:
            sprite1.drawString("PD2.0", 27, 48 - 2);
            break;
        case 2:
            sprite1.drawString("PD3.0", 27, 48 - 2);
            break;
        case 3:
            sprite1.drawString("HV", 27, 48 - 2);
            break;
        case 4:
            sprite1.drawString("AFC", 27, 48 - 2);
            break;
        case 5:
            sprite1.drawString("FCP", 27, 48 - 2);
            break;
        case 6:
            sprite1.drawString("SCP", 27, 48 - 2);
            break;
        case 7:
            sprite1.drawString("PE1.1", 27, 48 - 2);
            break;

        default:
            break;
        }
    }
    else if (sys_state == 1) // 放电
    {
        switch (  protocol) // 快放协议
        {
        case 0:
            sprite1.drawString("NOT", 27, 48 - 2);
            break;
        case 1:
            sprite1.drawString("PD2.0", 27, 48 - 2);
            break;
        case 2:
            sprite1.drawString("PPS", 27, 48 - 2);
            break;
        case 3:
            sprite1.drawString("QC2.0", 27, 48 - 2);
            break;
        case 4:
            sprite1.drawString("QC3.0", 27, 48 - 2);
            break;
        case 5:
            sprite1.drawString("FCP", 27, 48 - 2);
            break;
        case 6:
            sprite1.drawString("PE2/1", 27, 48 - 2);
            break;
        case 7:
            sprite1.drawString("SFCP", 27, 48 - 2);
            break;
        case 8:
            sprite1.drawString("AFC", 27, 48 - 2);
            break;
        case 9:
            sprite1.drawString("SCP", 27, 48 - 2);
            break;
        case 10:
            sprite1.drawString("PD3.0", 27, 48 - 2);
            break;
        default:
            break;
        }
    }
    else
        sprite1.drawString("NULL", 27, 48 - 2);
    // sprintf(draw_num, "%.2f℃", ic_temp);      // 芯片 温度
    // sprite1.drawString(draw_num, 27, 48 - 2); // 芯片 位置
    // 功率
    sprite1.fillRoundRect(0, 45 + 27, 24, 24, 5, TFT_YELLOW); // 位置，宽高，圆角半径 ，背景色  ////实心圆角矩形
    sprite1.loadFont(KaiTi22);
    sprite1.setTextColor(TFT_BLACK);
    sprite1.drawString("功", 1, 45 + 27 + 1);

    sprite1.loadFont(JianTi26);
    sprite1.setTextColor(TFT_CYAN);
    sprintf(draw_num, "%.2fw", sys_v * sys_a); // 功率 = sys_v * sys_a
    sprite1.drawString(draw_num, 27, 48 + 24);

    /**
     *  电池信息  温度  容量
     */
    sprite1.fillRoundRect(120, 45, 24, 24, 5, TFT_YELLOW); // 位置，宽高，圆角半径 ，背景色  ////实心圆角矩形
    sprite1.loadFont(KaiTi22);
    sprite1.setTextColor(TFT_BLACK);
    sprite1.drawString("温", 120 + 1, 45 + 1);

    sprite1.loadFont(JianTi26);
    sprite1.setTextColor(TFT_PINK);                 // 粉色
    sprintf(draw_num, "%.2f℃", ntc_temp);           // 电池温度NTC
    sprite1.drawString(draw_num, 120 + 27, 48 - 2); // 电池温度位置
    // 电池容量
    sprite1.fillRoundRect(120, 45 + 27, 24, 24, 5, TFT_YELLOW); // 位置，宽高，圆角半径 ，背景色  ////实心圆角矩形
    sprite1.loadFont(KaiTi22);
    sprite1.setTextColor(TFT_BLACK);
    sprite1.drawString("容", 120 + 1, 45 + 27 + 1);

    sprite1.loadFont(JianTi26);
    sprite1.setTextColor(TFT_CYAN);
    sprintf(draw_num, "%.1fwH", bat_m); // 容量
    sprite1.drawString(draw_num, 120 + 27, 48 + 24);

    // //-------------------------------下方电压 电流信息-----------------------------------------------------------
    // 芯片画图
    sprite1.fillRoundRect(0, 75 + 24, 36, 36, 5, TFT_PINK); // 位置，宽高，圆角半径 ，背景色  // 实心圆角矩形
    sprite1.setTextColor(TFT_BLACK);
    sprite1.loadFont(KaiTi22);
    sprite1.drawString("芯", 6, 75 + 8 + 24);
    // 电池画图
    sprite1.fillRoundRect(120, 75 + 24, 36, 36, 5, TFT_CYAN); // 位置，宽高，圆角半径 ，背景色  // 实心圆角矩形
    sprite1.drawString("池", 120 + 7, 75 + 8 + 24);
    /**
     *  系统 电压  电流
     */
    sprite1.setTextColor(TFT_WHITE);
    sprite1.loadFont(JianTi20);
    sprintf(draw_num, "%.2fV", sys_v); // 充放 电压
    sprite1.drawString(draw_num, 43, 73 + 24);

    sprite1.setTextColor(TFT_BLACK);
    sprite1.fillRoundRect(41, 93 + 24, 58, 17, 5, TFT_CYAN); // 实心圆角矩形
    sprintf(draw_num, "%.2fA", sys_a);                       // 系统 电流
    sprite1.drawString(draw_num, 45, 93 + 24);
    /**
     *  电池 电压  电流
     */
    sprite1.setTextColor(TFT_YELLOW);
    sprite1.loadFont(JianTi20);
    sprintf(draw_num, "%.2fV", bat_v); // 电池电压
    sprite1.drawString(draw_num, 120 + 43, 73 + 24);

    sprite1.setTextColor(TFT_BLACK);
    sprite1.fillRoundRect(120 + 41, 93 + 24, 58, 17, 5, TFT_PINK); // 实心圆角矩形
    sprintf(draw_num, "%.2fA", sys_v * sys_a / bat_v);             // 电池电流 ：   功率 / 电池电压
    sprite1.drawString(draw_num, 120 + 45, 93 + 24);
    /**
     * 充放状态   AC状态
     */
    sprite1.loadFont(JianTi20);
    sprite1.setTextColor(TFT_BLACK);

    if (sys_state == 1)
    {
        if (ac_state == 1)
        {
            sprite1.fillRoundRect(231 - 18, 24, 18, 18, 3, TFT_CYAN);
            sprite1.drawString("A", 231 - 15, 25);
        }
        if (ac_state == 4)
        {
            sprite1.fillRoundRect(231 - 18, 24, 18, 18, 3, TFT_PINK);
            sprite1.drawString("C", 231 - 15, 25);
        }
        if (ac_state == 5)
        {
            sprite1.fillRoundRect(231 - 18 - 22, 24, 18, 18, 3, TFT_CYAN);
            sprite1.fillRoundRect(231 - 18, 24, 18, 18, 3, TFT_PINK);
            sprite1.drawString("A", 231 - 15 - 22, 25);
            sprite1.drawString("C", 231 - 15, 25);
        }
    }
    else if (sys_state == 2)
    {
        if (ac_state == 1)
        {
            sprite1.fillRoundRect(231 - 18, 24, 18, 18, 3, TFT_CYAN);
            sprite1.drawString("A", 231 - 15, 25);
        }
        if (ac_state == 4)
        {
            sprite1.fillRoundRect(231 - 18, 24, 18, 18, 3, TFT_PINK);
            sprite1.drawString("C", 231 - 15, 25);
        }
        if (ac_state == 5)
        {
            sprite1.fillRoundRect(231 - 18 - 22, 24, 18, 18, 3, TFT_CYAN);
            sprite1.fillRoundRect(231 - 18, 24, 18, 18, 3, TFT_PINK);
            sprite1.drawString("A", 231 - 15 - 22, 25);
            sprite1.drawString("C", 231 - 15, 25);
        }
    }
    else
    {
        sprite1.loadFont(KaiTi18); // 设置字体
        sprite1.setTextColor(TFT_PINK);
        sprite1.setTextDatum(TR_DATUM);      // 字体右上角为原点
        sprite1.drawString("空闲", 231, 24); // 态
    }
    sprite1.pushSprite(0, 0); // 显示在画布1上
    sprite1.deleteSprite();   // 删除精灵
    sprite1.unloadFont();     // 释放加载字体
}

/**
 * @brief 主题6   新简风  HUA
 *
 * @param bat_v 电池电压
 * @param sys_v 系统电压
 * @param sys_a 系统电流
 * @param ic_temp ic温度
 * @param ntc_temp 电池温度
 * @param bat_m 实时容量mAH
 * @param bat_per 电量百分比
 * @param sys_state 充放电状态
 * @param ac_state 端口状态
 * @param protocol 快充协议  
 * @param smalla 小电流状态
 * @param ble_state 蓝牙状态
 * @param cycle 循环次数
 */
void Theme6(float bat_v, float sys_v, float sys_a, float ic_temp, float ntc_temp, float bat_m,
            uint8_t bat_per, uint8_t sys_state, uint8_t ac_state, uint8_t protocol, uint8_t smalla, uint8_t ble_state, uint8_t cycle)
{
    char draw_num[20];
    // float v = 0, a = 0; // 电压  // 电流
    if (bat_per > 100)
        bat_per = 100;
    if (bat_per < 0)
        bat_per = 0;

    sprite1.createSprite(240, 135); // 创建画布大小
    sprite1.fillScreen(TFT_BLACK);  // 设置背景颜色
    sprite1.setTextDatum(TL_DATUM); // 字体左上角(原点)

    sprite1.loadFont(JianTi26);
    sprite1.setTextColor(TFT_WHITE);
    if (sys_state == 2) // 充电
    {
        sprite1.drawString("IN", 90, 0);
        switch (protocol) // 快充协议
        {
        case 0:
            sprite1.drawString("NOT", 0, 0);
            break;
        case 1:
            sprite1.drawString("PD2.0", 0, 0);
            break;
        case 2:
            sprite1.drawString("PD3.0", 0, 0);
            break;
        case 3:
            sprite1.drawString("HV", 0, 0);
            break;
        case 4:
            sprite1.drawString("AFC", 0, 0);
            break;
        case 5:
            sprite1.drawString("FCP", 0, 0);
            break;
        case 6:
            sprite1.drawString("SCP", 0, 0);
            break;
        case 7:
            sprite1.drawString("PE1.1", 0, 0);
            break;

        default:
            break;
        }
    }
    else if (sys_state == 1) // 放电
    {
        sprite1.drawString("OUT", 90, 0);
        switch (  protocol) // 快放协议
        {
        case 0:
            sprite1.drawString("NOT", 0, 0);
            break;
        case 1:
            sprite1.drawString("PD2.0", 0, 0);
            break;
        case 2:
            sprite1.drawString("PPS", 0, 0);
            break;
        case 3:
            sprite1.drawString("QC2.0", 0, 0);
            break;
        case 4:
            sprite1.drawString("QC3.0", 0, 0);
            break;
        case 5:
            sprite1.drawString("FCP", 0, 0);
            break;
        case 6:
            sprite1.drawString("PE2/1", 0, 0);
            break;
        case 7:
            sprite1.drawString("SFCP", 0, 0);
            break;
        case 8:
            sprite1.drawString("AFC", 0, 0);
            break;
        case 9:
            sprite1.drawString("SCP", 0, 0);
            break;
        case 10:
            sprite1.drawString("PD3.0", 0, 0);
            break;
        default:
            break;
        }
    }
    else
    {
        sprite1.drawString("NULL", 0, 0); // 协议
        sprite1.drawString("NO", 90, 0);
    }

    sprite1.setTextDatum(CC_DATUM); // 字体中部中间(原点)

    sprite1.loadFont(JianTi26);
    sprite1.setTextColor(TFT_WHITE);
    sprite1.setTextSize(3);           // 设置字体大小 (注意：会储存，全局此设置。用完 下方 恢复1)
    sprintf(draw_num, "%d", bat_per); // 百分比
                                      // if (bat_per == 100)
                                      //     sprite1.drawString(draw_num, 70, 80);
                                      // else
                                      // {
    if (bat_per <= 20)
        sprite1.setTextColor(TFT_RED); // 20 以下 红色
    sprite1.drawString(draw_num, 70, 80);
    // }
    sprite1.setTextSize(1); // 设置字体大小 (设置回默认)
    sprite1.setTextColor(TFT_WHITE);
    if (smalla == 1)
    {
        sprite1.setTextColor(TFT_RED); // 小电流打开  % 变红色
    }
    sprite1.drawString("%", 110, 120);
    // 画圈
    sprite1.drawCircle(70, 90, 68, TFT_BLUE);
    /**
     *  右侧电池信息  温度  电压  电流  功率  (循环  蓝牙)
     */
    sprite1.fillRoundRect(140, 0, 24, 24, 5, TFT_YELLOW);
    sprite1.fillRoundRect(140, 27, 24, 24, 5, TFT_PINK);
    sprite1.fillRoundRect(140, 27 * 2, 24, 24, 5, TFT_PINK);
    sprite1.fillRoundRect(140, 27 * 3, 24, 24, 5, TFT_CYAN);
    sprite1.fillRoundRect(140, 27 * 4, 24, 24, 5, TFT_GREEN);
    sprite1.loadFont(KaiTi22);
    sprite1.setTextColor(TFT_BLACK);
    sprite1.setTextDatum(TL_DATUM); // 字体左上角为原点
    sprite1.drawString("温", 140 + 1, 0 + 1);
    sprite1.drawString("压", 140 + 1, 27 + 1);
    sprite1.drawString("流", 140 + 1, 27 * 2 + 1);
    sprite1.drawString("功", 140 + 1, 27 * 3 + 1);
    sprite1.drawString("循", 140 + 1, 27 * 4 + 1);

    // 实时数据
    sprite1.loadFont(JianTi26);
    sprite1.setTextColor(TFT_PINK); // 粉色

    sprintf(draw_num, "%.1f℃", ntc_temp);
    sprite1.drawString(draw_num, 140 + 27, 0); // 电池温度NTC
    sprintf(draw_num, "%.1fV", sys_v);
    sprite1.drawString(draw_num, 140 + 27, 27); // 压
    sprintf(draw_num, "%.1fA", sys_a);
    sprite1.drawString(draw_num, 140 + 27, 27 * 2); // 流
    sprintf(draw_num, "%.1fw", sys_v * sys_a);
    sprite1.drawString(draw_num, 140 + 27, 27 * 3); // 功
    sprintf(draw_num, "%d", cycle);
    sprite1.drawString(draw_num, 140 + 27, 27 * 4); // 循环
    if (ble_state == 1)
    {
        sprite1.drawPng(imgbleimg24, sizeof(imgbleimg24), 215, 27 * 4); // 蓝牙图标
    }

    sprite1.pushSprite(0, 0); // 显示在画布1上
    sprite1.deleteSprite();   // 删除精灵
    sprite1.unloadFont();     // 释放加载字体
}

/**
 * @brief 主题7   全功能主题   HUA
 *
 * @param bat_v 电池电压
 * @param sys_v 系统电压
 * @param sys_a 系统电流
 * @param ic_temp ic温度
 * @param ntc_temp 电池温度
 * @param bat_m 实时容量mAH
 * @param bat_per 电量百分比
 * @param sys_state 充放电状态
 * @param ac_state 端口状态
 * @param protocol 快充协议  
 * @param smalla 小电流状态
 * @param ble_state 蓝牙状态
 * @param cycle 循环次数
 */
void Theme7(float bat_v, float sys_v, float sys_a, float ic_temp, float ntc_temp, float bat_m,
            uint8_t bat_per, uint8_t sys_state, uint8_t ac_state, uint8_t protocol, uint8_t smalla, uint8_t ble_state, uint8_t cycle)
{
    char draw_num[20];
    // float v = 0, a = 0; // 电压  // 电流
    if (bat_per > 100)
        bat_per = 100;
    if (bat_per < 0)
        bat_per = 0;

    sprite1.createSprite(240, 135); // 创建画布大小
    sprite1.fillScreen(TFT_BLACK);  // 设置背景颜色
    // sprite1.loadFont(KaiTi22);      // 字体设置

    // sprite1.fillRoundRect(0, 0, 70, 24, 5, TFT_YELLOW);
    sprite1.fillRoundRect(0, 27, 70, 24, 5, TFT_PINK);
    // sprite1.fillRoundRect(0, 27 * 2, 70, 24, 5, TFT_PINK);
    sprite1.fillRoundRect(0, 27 * 3, 70, 24, 5, TFT_CYAN);
    // sprite1.fillRoundRect(0, 27 * 4, 70, 24, 5, TFT_GREEN);

    sprite1.fillRoundRect(85, 0, 70, 24, 5, TFT_YELLOW);
    // sprite1.fillRoundRect(85, 27, 70, 24, 5, TFT_PINK);
    sprite1.fillRoundRect(85, 27 * 2, 70, 24, 5, TFT_PINK);
    // sprite1.fillRoundRect(85, 27 * 3, 70, 24, 5, TFT_CYAN);
    sprite1.fillRoundRect(85, 27 * 4, 70, 24, 5, TFT_GREEN);

    // sprite1.fillRoundRect(170, 0, 70, 24, 5, TFT_YELLOW);
    sprite1.fillRoundRect(170, 27, 70, 24, 5, TFT_PINK);
    // sprite1.fillRoundRect(170, 27 * 2, 70, 24, 5, TFT_PINK);
    sprite1.fillRoundRect(170, 27 * 3, 70, 24, 5, TFT_CYAN);
    // sprite1.fillRoundRect(170, 27 * 4, 70, 24, 5, TFT_GREEN);

    sprite1.setTextDatum(TC_DATUM); // 字体顶部中间(原点)
    // 实时数据  左侧
    sprite1.loadFont(JianTi26);
    sprite1.setTextColor(TFT_WHITE);
    sprintf(draw_num, "%.1f℃", ntc_temp);
    sprite1.drawString(draw_num, 35, 0); // 电池温度NTC
    sprintf(draw_num, "%.1fV", bat_v);
    sprite1.setTextColor(TFT_BLACK);
    sprite1.drawString(draw_num, 35, 27); // 电
    sprintf(draw_num, "%.1fA", sys_v * sys_a / bat_v);
    sprite1.setTextColor(TFT_WHITE);
    sprite1.drawString(draw_num, 35, 27 * 2); // 池
    sprintf(draw_num, "%d", cycle);
    sprite1.setTextColor(TFT_BLACK);
    sprite1.drawString(draw_num, 35, 27 * 3); // 循环
    sprintf(draw_num, "%.fwH", bat_m);
    sprite1.setTextColor(TFT_WHITE);
    sprite1.drawString(draw_num, 35, 27 * 4); // 容量

    // 实时数据  中间
    sprite1.loadFont(JianTi26);
    sprite1.setTextColor(TFT_BLACK);
    if (sys_state == 2) // 充电
    {
        switch (protocol) // 快充协议
        {
        case 0:
            sprite1.drawString("NOT", 120, 0);
            break;
        case 1:
            sprite1.drawString("PD2.0", 120, 0);
            break;
        case 2:
            sprite1.drawString("PD3.0", 120, 0);
            break;
        case 3:
            sprite1.drawString("HV", 120, 0);
            break;
        case 4:
            sprite1.drawString("AFC", 120, 0);
            break;
        case 5:
            sprite1.drawString("FCP", 120, 0);
            break;
        case 6:
            sprite1.drawString("SCP", 120, 0);
            break;
        case 7:
            sprite1.drawString("PE1.1", 120, 0);
            break;

        default:
            break;
        }
    }
    else if (sys_state == 1) // 放电
    {
        switch (  protocol) // 快放协议
        {
        case 0:
            sprite1.drawString("NOT", 120, 0);
            break;
        case 1:
            sprite1.drawString("PD2.0", 120, 0);
            break;
        case 2:
            sprite1.drawString("PPS", 120, 0);
            break;
        case 3:
            sprite1.drawString("QC2.0", 120, 0);
            break;
        case 4:
            sprite1.drawString("QC3.0", 120, 0);
            break;
        case 5:
            sprite1.drawString("FCP", 120, 0);
            break;
        case 6:
            sprite1.drawString("PE2/1", 120, 0);
            break;
        case 7:
            sprite1.drawString("SFCP", 120, 0);
            break;
        case 8:
            sprite1.drawString("AFC", 120, 0);
            break;
        case 9:
            sprite1.drawString("SCP", 120, 0);
            break;
        case 10:
            sprite1.drawString("PD3.0", 120, 0);
            break;
        default:
            break;
        }
    }
    else
        sprite1.drawString("NULL", 120, 0); // 协议

    sprite1.setTextColor(TFT_WHITE);
    if (sys_state == 2)
        sprite1.drawString("IN", 120, 27); // 输入输出
    else if (sys_state == 1)
        sprite1.drawString("OUT", 120, 27); // 输入输出
    else
        sprite1.drawString("NO", 120, 27); // 输入输出

    sprintf(draw_num, "%.1fV", sys_v);
    sprite1.setTextColor(TFT_BLACK);
    sprite1.drawString(draw_num, 120, 27 * 2); // 压
    sprintf(draw_num, "%.1fA", sys_a);
    sprite1.setTextColor(TFT_WHITE);
    sprite1.drawString(draw_num, 120, 27 * 3); // 流
    sprintf(draw_num, "%.1fw", sys_v * sys_a);
    sprite1.setTextColor(TFT_BLACK);
    sprite1.drawString(draw_num, 120, 27 * 4); // 功

    // 实时数据  右侧
    sprintf(draw_num, "%d%%", bat_per);
    sprite1.setTextColor(TFT_WHITE);
    sprite1.drawString(draw_num, 205, 0); // 百分比

    if (ac_state == 1)
    {
        sprite1.setTextColor(TFT_BLACK);
        sprite1.drawString("A", 205, 27); // A
        sprite1.setTextColor(TFT_WHITE);
        sprite1.drawString("--", 205, 27 * 2); // A
    }
    else if (ac_state == 4)
    {
        sprite1.setTextColor(TFT_BLACK);
        sprite1.drawString("--", 205, 27); // C
        sprite1.setTextColor(TFT_WHITE);
        sprite1.drawString("C", 205, 27 * 2); // C
    }
    else if (ac_state == 5)
    {
        sprite1.setTextColor(TFT_BLACK);
        sprite1.drawString("A", 205, 27); // A
        sprite1.setTextColor(TFT_WHITE);
        sprite1.drawString("C", 205, 27 * 2); // C
    }
    else
    {
        sprite1.setTextColor(TFT_BLACK);
        sprite1.drawString("--", 205, 27); // A
        sprite1.setTextColor(TFT_WHITE);
        sprite1.drawString("--", 205, 27 * 2); // C
    }

    sprite1.setTextColor(TFT_BLACK);
    if (smalla == 1)
        sprite1.drawString("smallA", 205, 27 * 3); // 小电流
    else
        sprite1.drawString("NO", 205, 27 * 3); // 小电流

    sprite1.setTextColor(TFT_WHITE);
    if (ble_state == 1)
    {
        sprite1.drawString("BLE", 205, 27 * 4); // 蓝牙
    }
    else
    {
        sprite1.drawString("NO", 205, 27 * 4); // 蓝牙
    }

    sprite1.pushSprite(0, 0); // 显示在画布1上
    sprite1.deleteSprite();   // 删除精灵
    sprite1.unloadFont();     // 释放加载字体
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
 *
 *
 *
 *
 *
 *
 */

//----------------------------------------------------------------------------------以下为提示页面------------------------
//----------------------------------------------------------------------------------以下为提示页面------------------------
//----------------------------------------------------------------------------------以下为提示页面------------------------
//----------------------------------------------------------------------------------以下为提示页面------------------------
//----------------------------------------------------------------------------------以下为提示页面------------------------
//----------------------------------------------------------------------------------以下为提示页面------------------------

// 非正当获得设备采取措施 ，强制关闭输出，保留充电
void lost_Page()
{
    sprite1.createSprite(240, 135); // 创建画布大小
    sprite1.fillScreen(TFT_BLACK);  // 设置背景颜色
    sprite1.loadFont(JianTi26);     // 字体设置
    sprite1.setTextColor(TFT_RED);  // 字体颜色
    // sprite1.setTextDatum(TL_DATUM);  // 字体左上角
    sprite1.setTextDatum(CC_DATUM); // 字体居中

    sprite1.drawString("Wrongful acquisition", 120, 50);
    sprite1.drawString("The AC is closed.", 120, 80);

    sprite1.pushSprite(0, 0); // 显示在画布1上
    sprite1.deleteSprite();   // 删除精灵
    sprite1.unloadFont();     // 释放加载字体
}

// 等待连接WiFi
void WiFi_Page()
{
    sprite1.createSprite(240, 135);  // 创建画布大小
    sprite1.fillScreen(TFT_BLACK);   // 设置背景颜色
    sprite1.loadFont(JianTi26);      // 字体设置
    sprite1.setTextColor(TFT_WHITE); // 字体颜色
    // sprite1.setTextDatum(TL_DATUM);  // 字体左上角
    sprite1.setTextDatum(CC_DATUM); // 字体居中

    sprite1.drawString("SSID: 2024", 120, 50);
    sprite1.drawString("PASS: 12345678", 120, 80);

    sprite1.loadFont(JianTi20); // 字体设置
    sprite1.drawString("Click to exit OTA", 120, 120);

    // sprite1.setTextDatum(TL_DATUM); // 字体左上角
    sprite1.drawString("Connecting to WiFi...", 120, 10);

    sprite1.pushSprite(0, 0); // 显示在画布1上
    sprite1.deleteSprite();   // 删除精灵
    sprite1.unloadFont();     // 释放加载字体
}

// OTA 配网提示页面   使用此方法请弃用 WiFi_Page();
void ota_AP()
{
    sprite1.createSprite(240, 135);  // 创建画布大小
    sprite1.fillScreen(TFT_BLACK);   // 设置背景颜色
    sprite1.loadFont(JianTi26);      // 字体设置
    sprite1.setTextColor(TFT_WHITE); // 字体颜色
    // sprite1.setTextDatum(TL_DATUM);  // 字体左上角
    sprite1.setTextDatum(CC_DATUM); // 字体居中

    sprite1.drawString("AP: AP_Page", 120, 50);
    sprite1.drawString("IP: 192.168.4.1", 120, 80);

    sprite1.loadFont(JianTi20); // 字体设置
    sprite1.drawString("Click to exit OTA", 120, 120);

    sprite1.pushSprite(0, 0); // 显示在画布1上
    sprite1.deleteSprite();   // 删除精灵
    sprite1.unloadFont();     // 释放加载字体
}

// 更新进度
void ota_Page(int a, int b)
{
    char draw_num[20];

    sprite1.createSprite(240, 135); // 创建画布大小
    sprite1.fillScreen(TFT_BLACK);  // 设置背景颜色
    sprite1.setTextColor(TFT_GREEN);
    // sprite1.loadFont(KaiTi22);
    // sprite1.setTextDatum(TL_DATUM); // 字体左上角
    sprite1.setTextDatum(CC_DATUM); // 字体居中

    sprite1.loadFont(JianTi26);
    sprite1.drawString("Updating...", 120, 50);
    // sprintf(draw_num, "%.1f%%", cur * 100.0 / total); // 100.0%
    sprintf(draw_num, "%.f %%", a * 100.0 / b); // 无小数 100 %
    sprite1.drawString(draw_num, 120, 80);

    sprite1.loadFont(JianTi20); // 字体设置
    sprite1.drawString("Click to exit OTA", 120, 120);

    sprite1.pushSprite(0, 0); // 显示在画布1上
    sprite1.deleteSprite();   // 删除精灵
    sprite1.unloadFont();     // 释放加载字体
}