#include <sw6306.h>
#include <Background.h>
#include <Ticker.h>
#include <ESP32bt.h>
#include <ck22.h>
#include <eeprom32.h>
#include <ArduinoJson.h>
#include <OneButton.h>
#include "ota.h"
#include <WiFi.h>
#include <esp_task_wdt.h>
#include "bq40z50.h"
#include "rtc_time.h" //内置时钟

#define BUTTON_PIN_BITMASK 0x0010 // GPIOs 4    io4 按钮

BleKeyboard bleKeyboard("AC1008", "OCRC", 50); // 蓝牙
Ticker ticker1;                                // 计时器
OneButton button(4, true);                     // IO4按键
ESP32Time rtc;                                 // offset in seconds GMT+1  // 内置时钟

uint8_t bt_state = 0, sleeptime, espthem, ota, idlock, smalla, topic; // 蓝牙状态(开蓝牙用)   [屏幕睡眠  主题  OTA    ID锁  (接收蓝牙数据)]   // 小电流开关   屏幕方向1上3下;
// 子线程函数
void Task_OTA(void *pvParameters);    // 子线程 OTA更新
void Task_AC_OFF(void *pvParameters); // 子线程 关闭所有输出口
void Task_IO4(void *pvParameters);    // 子线程 检测单击IO4  切换主题菜单

void setup()
{
    Serial.begin(115200);
    pinMode(4, INPUT_PULLUP);
    pinMode(27, INPUT_PULLUP);
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_27, 0);                              // 唤醒引脚配置 低电平唤醒
    esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK, ESP_EXT1_WAKEUP_ALL_LOW); // 唤醒引脚配置 低电平唤醒
    IICinit();                                                                 // 初始化 IIC 通讯
    EEPROMinit();                                                              // 初始化 EEPROM 寄存器
    DisplayInit();                                                             // 显示初始化
    // PowerLOGO();                                                               // 开机LOGO
    // delay(500);
    // 配置倒计时
    if (EEPROM.read(5) < 10 && EEPROM.read(5) != 0 || EEPROM.read(5) > 120) // 亮屏时间30-120s
        EEPROM.write(5, 30);                                                // 不在范围  设置为 30
    if (EE_BLETimeRead() < 150 || EE_BLETimeRead() > 3600)                  // 蓝牙休眠时间150-3600s
        EE_BLETimeWrite(150);                                               // 不在范围  设置为 150
    EEPROM.commit();                                                        // 保存
    // 多线程配置
    if (EEPROM.read(12) == 1) // 丢失模式，关闭所有输出口
    {
        xTaskCreatePinnedToCore(Task_AC_OFF,   // 具体实现的函数
                                "Task_AC_OFF", // 任务名称
                                1024,          // 堆栈大小
                                NULL,          // 输入参数
                                1,             // 任务优先级
                                NULL,          //
                                0              // 核心  0\1  不指定
        );
        lost_Page(); // 丢失设备提示页   内部延时 显示10s
    }
    // OTA更新
    if (EEPROM.read(11) == 1) // OTA更新
    {
        EEPROM.write(11, 0); // 写非1  更新完成
        EEPROM.write(5, 30); // 屏幕睡眠设置为 30
        EEPROM.commit();     // 保存
        // 配网 更新任务
        xTaskCreatePinnedToCore(Task_OTA,   // 具体实现的函数
                                "Task_OTA", // 任务名称
                                10240,      // 堆栈大小
                                NULL,       // 输入参数
                                1,          // 任务优先级
                                NULL,       //
                                0           // 核心  0\1  不指定
        );
        updateBin(); // OTA  含配网
    }
    // button.reset(); // 清除按钮状态机的状态
    // button.attachDoubleClick(doubleclick);   // 注册双击
    // delay(600);                              // 600
    // 单击IO4  切换主题菜单
    xTaskCreatePinnedToCore(Task_IO4,   // 具体实现的函数
                            "Task_IO4", // 任务名称
                            2048,       // 堆栈大小
                            NULL,       // 输入参数
                            1,          // 任务优先级
                            NULL,       //
                            0           // 核心  0\1 自动选择
    );
    // CK22AT  2314
    if (keros_main() != 1) // CK22AT  2314
        esp_deep_sleep_start();
    // sw6306 配置初始化
    I2C_Write_0_100(); // 可写100内寄存器
    SW6306init();      // sw6306初始化   // 设置100W最大充放功率   // C2口配置为B/L口模式   // 空载时间设置min:8s
    BatteryMode();     // 设置电池型号  BQ_40Z50init();
    Serial.printf("main on core: ");
    Serial.println(xPortGetCoreID());
}

void loop()
{
    delay(200);
    // 共用
    uint16_t year, time, pass;                   // 年份   读蓝牙链接时间    四位密码
    uint8_t month, day, hour, minute, sec, week; // 月  日  时  分  秒  星期
    // SW6306
    float sys_v, sys_a, sys_w, bat_v, bat_a, ic_temp, ntc_temp, bat_volume, bat_m; // 系统输入输出电压   ic温度   ntc电压   输入/输出电流   系统功率 电池温度  电池最大容量   电池当前容量
    uint8_t cyclecount, bat_per, sys_state, ac_state, smalla_state;                // 循环次数   电池百分比   系统充放电状态   4个接口状态AACL   小电流状态
    // BQ40Z50
    float bq_battemp, bq_batv, bq_bata, bq_batv4, bq_batv3, bq_batv2, bq_batv1;    // 电池温度   电池包总电压   电池电流  电池4电压   电池3电压  电池2电压  电池1电压
    uint8_t bq_batper;                                                             // 电池百分比
    uint16_t bq_cyclecount, bq_chargetime, bq_dischargetime, bq_batm, bq_capacity; // 循环次数  充满时间  放完电时间  剩余容量  充满容量

    uint8_t currentTime = EEPROM.read(5); // 睡眠时间
    unsigned long currentTime1;           // 程序执行计时
    while (currentTime >= 0)              // 屏幕睡眠时间 转次数
    {
        Serial.println("------------------------------------------------------------------BQ40Z50-----------loop------------------------");
        bq_battemp = (float)BQ_Temperature() / 100; // 电池 温度
        bq_batv = (float)BQ_Voltage() / 1000;       // 电池包 总电压
        bq_bata = (float)BQ_Current() / 1000;       // 电池 电流大小
        Serial.print("-----------------------");
        Serial.println(bq_bata);
        bq_batv4 = (float)BQ_CellVoltage4() / 1000; // 电压包 电池4电压
        bq_batv3 = (float)BQ_CellVoltage3() / 1000; // 电压包 电池3电压
        bq_batv2 = (float)BQ_CellVoltage2() / 1000; // 电压包 电池2电压
        bq_batv1 = (float)BQ_CellVoltage1() / 1000; // 电压包 电池1电压
        // bq_chargetime = BQ_AtRateTimeToFull();      // 电池组充满电的剩余时间
        bq_chargetime = BQ_AverageTimeToFull(); // 充满电时间
        // bq_dischargetime = BQ_AtRateTimeToEmpty();  // 电池组完全放电的剩余时间。
        bq_dischargetime = BQ_RunTimeToEmpty(); // 当前放电率返回预测的剩余电池容量    时间单位。

        bq_batper = BQ_RelativeStateOfCharge(); // 电池容量百分比 （根据充满电预测最大容量）
        bq_batm = BQ_RemainingCapacity();       // 预测的剩余电池容量
        bq_capacity = BQ_FullChargeCapacity();  // 充满电时预测的电池容量 (当前健康 最大电池的容量)
        bq_cyclecount = BQ_CycleCount();        // 经历的放电循环次数

        uint16_t bq_cbstatus = BQ_CBStatus();                 // 电池平衡时间信息
        uint16_t bq_health = BQ_Health();                     // 返回健康状态
        uint16_t bq_filteredcapacity = BQ_FilteredCapacity(); // 过滤后的容量和能量

        Serial.println("------------------------------------------------------------------SW6306-----------loop------------------------");
        sys_v = SYS_V();                                             // 系统电压
        sys_a = SYS_A();                                             // 系统 输入/输出 电流
        sys_w = sys_v * sys_a;                                       // 系统功率大小
        bat_v = Battery_V();                                         // 电池电压
        bat_a = Battery_A();                                         // 电池 输入/输出 电流
        ic_temp = IC_Temp();                                         // 芯片温度
        ntc_temp = NTC_Temp();                                       // NTC温度
        bat_m = Battery_Volume();                                    // 库仑计当前容量
        bat_per = Battery_Per();                                     // 电池电量百分比
        sys_state = SYS_State();                                     // 充放电状态   1 放电   2 充电
        ac_state = AC_State();                                       // 端口在线状态 C2是L口   0:空闲   1:C2   2:C1   3:C1C2   4:A2   5:A2C2   6:A2C1   7:A2C1C2   8:A1   9:A1C2   A:A1C1   B:A1C1C2   C:A1A2   D:A1A2C2   E:A1A2C1   F:A1A2C1C2
        smalla_state = Small_A_State();                              // 小电流状态   0: 关    1: 开
        EE_CycleCount(bat_per);                                      // 电池循环次数的判断
        cyclecount = EEPROM.read(2) / 2;                             // 判断之后读取  电池循环次数    /2减缓次数  20-80
        PrintTime(&year, &month, &day, &hour, &minute, &sec, &week); // 获取时间数据     年 月 日 时 分 秒 周

        Serial.print("MAC: ");
        Serial.println(ESP.getEfuseMac(), HEX);
       
        delay(20);
    beijing0:
        switch (EEPROM.read(4)) // 读取主题号
        // switch (5) // 调试主题用
        {
        case 1:
            if (EEPROM.read(13) == 1)
                BQScreenLayout1(bq_battemp, bq_batv, bq_bata, bq_batv4, bq_batv3, bq_batv2, bq_batv1, bq_chargetime, bq_dischargetime, bq_batper, bq_batm, bq_capacity, bq_cyclecount);
            else
                lcdlayout01(cyclecount, bat_per, bat_v, ic_temp, sys_v, sys_a, ntc_temp, sys_state, ac_state, bt_state); // 显示相关参数
            break;
        case 2:
            if (EEPROM.read(13) == 1)
                BQScreenLayout1(bq_battemp, bq_batv, bq_bata, bq_batv4, bq_batv3, bq_batv2, bq_batv1, bq_chargetime, bq_dischargetime, bq_batper, bq_batm, bq_capacity, bq_cyclecount);
            else
                BackgroundTime2(ac_state, bt_state, sys_v, sys_a, sys_w, ic_temp, ntc_temp, bat_per, cyclecount);
            break;
        case 3:
            if (EEPROM.read(13) == 1)
                BQScreenLayout1(bq_battemp, bq_batv, bq_bata, bq_batv4, bq_batv3, bq_batv2, bq_batv1, bq_chargetime, bq_dischargetime, bq_batper, bq_batm, bq_capacity, bq_cyclecount);
            else
                BackgroundTime3(week, bat_v, sys_v, sys_state, ac_state, sys_a, sys_w, bat_m, cyclecount, bat_per, bt_state);
            BackgroundTime3_2(month, day, ntc_temp, ic_temp, hour, minute, sec, smalla_state);
            break;
        case 4:
            if (EEPROM.read(13) == 1)
                BQScreenLayout1(bq_battemp, bq_batv, bq_bata, bq_batv4, bq_batv3, bq_batv2, bq_batv1, bq_chargetime, bq_dischargetime, bq_batper, bq_batm, bq_capacity, bq_cyclecount);
            else
                BackgroundTime4(bat_v, sys_v, sys_state, sys_a, sys_w, bat_per, bt_state, ic_temp, ntc_temp);
            break;
        case 5:
            if (EEPROM.read(13) == 1)
                BQScreenLayout1(bq_battemp, bq_batv, bq_bata, bq_batv4, bq_batv3, bq_batv2, bq_batv1, bq_chargetime, bq_dischargetime, bq_batper, bq_batm, bq_capacity, bq_cyclecount);
            else
                BackgroundTime5(bat_v, sys_v, sys_state, ac_state, sys_a, sys_w, bat_per, bat_m, bt_state, ic_temp, ntc_temp, month, day, hour, minute, sec, week);
            break;
        default:
            if (EEPROM.read(13) == 1)
                BQScreenLayout1(bq_battemp, bq_batv, bq_bata, bq_batv4, bq_batv3, bq_batv2, bq_batv1, bq_chargetime, bq_dischargetime, bq_batper, bq_batm, bq_capacity, bq_cyclecount);
            else
                lcdlayout01(cyclecount, bat_per, bat_v, ic_temp, sys_v, sys_a, ntc_temp, sys_state, ac_state, bt_state);
            break;
        }
        if (currentTime != 0)
            currentTime--; // 睡眠时间倒计时(循环次数，大概1s/fps)   到0退出循环
        if (currentTime == 1)
            break; // 跳出大循环  睡眠
        Serial.print("lcdSleepTime: ");
        Serial.println(currentTime);
        currentTime1 = millis();                     // 板运行当前程序的时间
        while (millis() - currentTime1 < 1000 - 200) // 延时 大概 1s 刷新 一次   以上执行时间165ms↔
        {
            if (digitalRead(4) == 0)
            {
                delay(200);              // 消抖
                if (digitalRead(4) == 0) // 此处进入眼睛动画
                {
                    delay(200);                // 延时
                    if (digitalRead(4) == LOW) // 拉低准备进入蓝牙
                    {
                        delay(1000);               // 延时
                        if (digitalRead(4) == LOW) // 进入蓝牙
                        {
                            // bleKeyboard.begin();                           // 打开蓝牙
                            // delay(1000);                                   // 给蓝牙启动缓冲
                            ticker1.once(EE_BLETimeRead(), esp_deep_sleep_start); // 小程序改的蓝牙打开时间  (150-3600,执行函数)
                            DynamicJsonDocument jsonBuffer1(512);
                            DynamicJsonDocument jsonBuffer2(512);
                            bt_state = 1;
                            switch (EEPROM.read(4))
                            {
                            case 1:
                                lcdlayout01(cyclecount, bat_per, bat_v, ic_temp, sys_v, sys_a, ntc_temp, sys_state, ac_state, bt_state); // 显示相关参数
                                break;
                            case 2:
                                BackgroundTime2(ac_state, bt_state, sys_v, sys_a, sys_w, ic_temp, ntc_temp, bat_per, cyclecount);
                                break;
                            case 3:
                                BackgroundTime3(week, bat_v, sys_v, sys_state, ac_state, sys_a, sys_w, bat_m, cyclecount, bat_per, bt_state);
                                BackgroundTime3_2(month, day, ntc_temp, ic_temp, hour, minute, sec, smalla_state);
                                break;
                            case 4:
                                BackgroundTime4(bat_v, sys_v, sys_state, sys_a, sys_w, bat_per, bt_state, ic_temp, ntc_temp);
                                break;
                            case 5:
                                BackgroundTime5(bat_v, sys_v, sys_state, ac_state, sys_a, sys_w, bat_per, bat_m, bt_state, ic_temp, ntc_temp, month, day, hour, minute, sec, week);
                                break;
                            default:
                                lcdlayout01(cyclecount, bat_per, bat_v, ic_temp, sys_v, sys_a, ntc_temp, sys_state, ac_state, bt_state); // 显示相关参数
                                break;
                            }
                            bleKeyboard.begin(); // 打开蓝牙
                            delay(1000);         // 给蓝牙启动缓冲
                            while (1)
                            {
                                lcdRotation();                                               // 实时屏幕上下
                                cyclecount = EEPROM.read(2) / 2;                             // 判断之后读取  电池循环次数
                                PrintTime(&year, &month, &day, &hour, &minute, &sec, &week); // 从DS1302获取时间数据     年 月 日 时 分 秒 周

                                sys_v = SYS_V();                    // 系统电压
                                sys_a = SYS_A();                    // 系统 输入/输出 电流
                                sys_w = sys_v * sys_a;              // 系统功率大小
                                bat_v = Battery_V();                // 电池电压
                                bat_a = Battery_A();                // 电池 输入/输出 电流
                                ic_temp = IC_Temp();                // 芯片温度
                                ntc_temp = NTC_Temp();              // NTC温度
                                bat_volume = Battery_Volume();      // 电池容量
                                bat_per = Battery_Per();            // 电池电量百分比
                                bat_m = bat_volume * bat_per / 100; // 电池当前容量判断
                                sys_state = SYS_State();            // 充放电状态   1 放电   2 充电
                                ac_state = AC_State();              // 端口在线状态 C2是L口   0:空闲   1:C2   2:C1   3:C1C2   4:A2   5:A2C2   6:A2C1   7:A2C1C2   8:A1   9:A1C2   A:A1C1   B:A1C1C2   C:A1A2   D:A1A2C2   E:A1A2C1   F:A1A2C1C2
                                smalla_state = Small_A_State();     // 小电流状态   0: 关    1: 开

                                topic = EEPROM.read(3);  // 读用户设置的值（1上3下）屏幕方向
                                time = EE_BLETimeRead(); // 读蓝牙链接时间

                                if (smalla_state == 0 && EEPROM.read(8) == 1) // smalla: OFF   //蓝牙给的设置 1 让打开小电流
                                {
                                    Small_A_ON();
                                }
                                else if (smalla_state == 1 && EEPROM.read(8) == 0)
                                {
                                    Small_A_OFF();
                                }

                                delay(5);
                                switch (EEPROM.read(4))
                                {
                                case 1:
                                    lcdlayout01(cyclecount, bat_per, bat_v, ic_temp, sys_v, sys_a, ntc_temp, sys_state, ac_state, bt_state); // 显示相关参数
                                    break;
                                case 2:
                                    BackgroundTime2(ac_state, bt_state, sys_v, sys_a, sys_w, ic_temp, ntc_temp, bat_per, cyclecount);
                                    break;
                                case 3:
                                    BackgroundTime3(week, bat_v, sys_v, sys_state, ac_state, bat_a, sys_w, bat_m, cyclecount, bat_per, bt_state);
                                    BackgroundTime3_2(month, day, ntc_temp, ic_temp, hour, minute, sec, smalla_state);
                                    break;
                                case 4:
                                    BackgroundTime4(bat_v, sys_v, sys_state, sys_a, sys_w, bat_per, bt_state, ic_temp, ntc_temp);
                                    break;
                                case 5:
                                    BackgroundTime5(bat_v, sys_v, sys_state, ac_state, sys_a, sys_w, bat_per, bat_m, bt_state, ic_temp, ntc_temp, month, day, hour, minute, sec, week);
                                    break;
                                default:
                                    lcdlayout01(cyclecount, bat_per, bat_v, ic_temp, sys_v, sys_a, ntc_temp, sys_state, ac_state, bt_state); // 显示相关参数
                                    break;
                                }
                                jsonBuffer1["efuseMac"] = String(ESP.getEfuseMac(), HEX);
                                jsonBuffer1["name"] = "AC1008";               // 设备名称
                                jsonBuffer1["software"] = "v4.0";             // 固件版本
                                jsonBuffer1["hardware"] = "v3.2";             // 硬件版本
                                jsonBuffer1["bat_cir"] = bq_cyclecount;       // 循环次数  bq_40z50
                                jsonBuffer1["bat_V"] = String(bat_v, 3);      // 电池电压
                                jsonBuffer1["bat_A"] = String(bat_a, 3);      // 电流
                                jsonBuffer1["A_C"] = ac_state;                // AC口状态
                                jsonBuffer1["ic_temp"] = String(ic_temp, 3);  // ic温度
                                jsonBuffer1["sys_outinv"] = String(sys_v, 3); // 系统充放电压
                                jsonBuffer1["sys_w"] = String(sys_w, 3);      // 系统功率
                                // jsonBuffer1["sys"] = sys_state;               // 充放电状态
                                jsonBuffer1["bat_m"] = String(bat_m, 3);      // 电池当前容量
                                jsonBuffer1["bat_per"] = bat_per;             // 百分比bat_per
                                jsonBuffer1["bat_ntc"] = String(ntc_temp, 3); // 电池温度

                                String output1;
                                serializeJson(jsonBuffer1, output1);
                                jsonBuffer1.clear();
                                Serial.println("---------------sendTx------1111------");
                                Serial.println(output1);
                                Serial.println("-------------------------------------");
                                bleKeyboard.sendTx(output1); // 第一次发送数据
                                output1 = "";
                                delay(100);
                                // jsonBuffer1["topic_Num"] = EEPROM.read(4); // 主题编号
                                //     jsonBuffer1["led-sleep"] = EEPROM.read(5); // 睡眠时间  最大存储255   小程序设置值  30  60  90  120  常亮
                                // jsonBuffer1["topic_dir"] = EEPROM.read(3);     // 屏幕方向  1  3
                                // jsonBuffer1["blt_Time"] = eepromread();        // 蓝牙打开及连接时间
                                // jsonBuffer1["small_A"] = Small_A_State();              // 读取最新的小电流状态  发送给小程序

                                // serializeJson(jsonBuffer1, output1);
                                // jsonBuffer1.clear();
                                // Serial.println("---------------sendTx1------2222------");
                                // Serial.println(output1);
                                // Serial.println("--------------------------------------");
                                // bleKeyboard.sendTx1(output1); // 第二次发送数据
                                // output1 = "";
                                // delay(100);
                                if (Rxdata.length() > 0) // 蓝牙接收数据
                                {
                                    Serial.println("------Rxdata: ----");
                                    Serial.println(Rxdata);
                                    Serial.println("------------------");
                                    // Rxdata = "{\"str\":\"welcome\",\"data1\":135,\"data2\":[48.756080,2.302038],\"object\":{\"key1\":-254}}";
                                    DeserializationError error = deserializeJson(jsonBuffer2, Rxdata);
                                    if (error)
                                    {
                                        Serial.print(F("deserializeJson() failed: ")); // F: Flash    内容存储到flash  节省RAM空间
                                        Serial.println(error.f_str());
                                        return;
                                    }
                                    // 解析JSON
                                    idlock = jsonBuffer2["btidlock"];       // ID锁
                                    year = jsonBuffer2["btyear"];           // 读取年
                                    month = jsonBuffer2["btmon"];           // 读取月
                                    day = jsonBuffer2["btday"];             // 读取天
                                    hour = jsonBuffer2["bthour"];           // 读取小时
                                    minute = jsonBuffer2["btmin"];          // 读取分
                                    sec = jsonBuffer2["btsec"];             // 读取秒
                                    week = jsonBuffer2["btweek"];           // 读取周几
                                    espthem = jsonBuffer2["btthem"];        // 读取主题
                                    sleeptime = jsonBuffer2["btsleeptime"]; // 读取睡眠时间
                                    topic = jsonBuffer2["bttopic"];         // 读用户设置的值（1上3下）	屏幕方向
                                    time = jsonBuffer2["bttime"];           // 读蓝牙开启状态的时间
                                    smalla = jsonBuffer2["btsmalla"];       // 读小电流开关设置
                                    ota = jsonBuffer2["btota"];             // OTA更新
                                    cyclecount = jsonBuffer2["btxunhuan"];  // 改写循环次数

                                    // 开始写入数据
                                    EEPROM.write(12, idlock); // 写1锁死ESP32 或 关闭所有输出口
                                    delay(5);
                                    if (sec != 0 || minute != 0 || hour != 0 || day != 0 || month != 0 || year)
                                        rtc.setTime(sec, minute, hour, day, month, year); // 更新彩屏时间
                                    delay(5);
                                    if (espthem != 0)
                                        EEPROM.write(4, espthem); // 写入主题编号
                                    delay(5);
                                    EEPROM.write(5, sleeptime); // 写入睡眠时间
                                    delay(5);
                                    if (topic != 0)
                                        EEPROM.write(3, topic); // 写入屏幕显示方向
                                    delay(5);
                                    if (time != 0)
                                        EE_BLETimeWrite(time); // 写入蓝牙开启时间
                                    delay(5);
                                    EEPROM.write(8, smalla); // 写入小电流设置
                                    delay(5);
                                    if (ota != 0)
                                        EEPROM.write(11, ota); // OTA更新  写1更新自动置零
                                    delay(5);
                                    if (cyclecount == 1)
                                        EEPROM.write(2, 0); // 循环次数  写1 清零
                                    else if (cyclecount != 0)
                                        EEPROM.write(2, cyclecount); // 循环次数
                                    delay(5);
                                    EEPROM.commit(); // 保存
                                    delay(5);
                                    Rxdata = ""; // 清空
                                    Serial.println("*************");
                                    Serial.println("RxEnd ! ! !");
                                    Serial.println("*************");
                                }
                                Serial.print("topic: ");
                                Serial.println(EEPROM.read(3));
                                Serial.print("them: ");
                                Serial.println(EEPROM.read(4));
                                Serial.print("sleepTime: ");
                                Serial.println(EEPROM.read(5));
                                Serial.print("BTtime: ");
                                Serial.println(EE_BLETimeRead());
                                Serial.print("SmallA: ");
                                Serial.println(EEPROM.read(8));
                                Serial.print("OTA: ");
                                Serial.println(EEPROM.read(11));
                                Serial.print("AC_OFF: ");
                                Serial.println(EEPROM.read(12));
                                Serial.print("cycles: ");
                                Serial.print(EEPROM.read(2));
                                Serial.println("/2");

                                unsigned long currentTime2;
                                currentTime2 = millis(); // 程序执行到此时间
                                delay(10);
                                while (millis() - currentTime2 < 1000) // while延时
                                {
                                    if (digitalRead(4) == 0)
                                    {
                                        delay(300);
                                        if (digitalRead(4) == LOW)
                                        {
                                            delay(1000);
                                            if (digitalRead(4) == LOW)
                                            {
                                                esp_deep_sleep_start();
                                                break;
                                            }
                                            continue;
                                        }
                                        bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN); // 音量减  用于拍照功能
                                        Serial.println("KEY_MEDIA_VOLUME_DOWN");
                                        delay(500);
                                    }
                                }
                            }
                        }
                    }
                    goto beijing0;
                }
            }
        }
    }
    // 屏幕进入睡眠
    if (digitalRead(27) == 1)
    {
        esp_deep_sleep_start(); // 睡眠
    }
    offscreen(); // 熄灭屏幕
    if (digitalRead(27) == 0)
    {
        while (1)
        {
            if (digitalRead(4) == 0) // 按 按钮
            {
                onscreen(); // 开启屏幕
                break;
            }
            if (digitalRead(27) == 1) // sw6306工作
            {
                onscreen(); // 开启屏幕
                break;
            }
        }
    }
}
void Task_OTA(void *pvParameters)
{
    // esp_task_wdt_add(NULL); // 给本任务添加看门口  NULL代表本任务
    Serial.print("Task_OTA on core: ");
    Serial.println(xPortGetCoreID()); // 所在核心
    while (1)
    {
        if (WiFi.status() == WL_CONNECTED) // WiFi连接成功
            ota_Page(a, b);                // lcd 显示进度
        else
            WiFi_Page(); // 正在连接WiFi...
        // ota_AP();     // lcd配网ip提示页面
        if (digitalRead(4) == LOW) // 按键单击退出更新
        {
            esp_deep_sleep_start();
            break;
        }
        vTaskDelay(600); // 慢一点循环，让OTA跑流畅点   //延时 退让资源同时喂狗
    }
}
void Task_AC_OFF(void *pvParameters)
{
    while (1)
    {
        Serial.print("Task_AC_OFF on core: ");
        Serial.println(xPortGetCoreID()); // 所在核心
        // AC_OFF();                         // 关闭AC口输出
        vTaskDelay(1000); // 延时  及喂狗
    }
}
void Task_IO4(void *pvParameters)
{
    while (1)
    {
        if (digitalRead(4) == LOW)
            EE_IO4();
        vTaskDelay(300); // 延时  及喂狗
    }
}
