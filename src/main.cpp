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
#include "rtc_time.h" //内置时钟
#include "AgentConfig.h"

#define BUTTON_PIN_BITMASK 0x0010 // GPIOs 4    io4 按钮

BleKeyboard bleKeyboard("AC1008", "OCRC", 50); // 蓝牙
Ticker ticker1;                                // 计时器
OneButton button(4, true);                     // IO4按键
ESP32Time rtc;                                 // offset in seconds GMT+1  // 内置时钟

// 子线程函数
void Task_OTA(void *pvParameters); // 子线程 OTA更新
// void Task_AC_OFF(void *pvParameters); // 子线程 关闭所有输出口
// void Task_IO4(void *pvParameters);    // 子线程 检测单击IO4  切换主题菜单

void setup()
{
    Serial.begin(115200);
    pinMode(4, INPUT_PULLUP);
    pinMode(27, INPUT_PULLUP);
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_27, 0);                              // 唤醒引脚配置 低电平唤醒
    esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK, ESP_EXT1_WAKEUP_ALL_LOW); // 唤醒引脚配置 低电平唤醒
    IICinit();                                                                 // 初始化 IIC 通讯
    I2C_Write_0_100();                                                         // 可写100内寄存器
    EEPROMinit();                                                              // 初始化 EEPROM 寄存器
    DisplayInit();                                                             // 显示初始化
    PowerLOGO(imgName);                                                        // 开机LOGO

    // 配置倒计时
    if (EEPROM.read(5) < 10 && EEPROM.read(5) != 0 || EEPROM.read(5) > 120) // 亮屏时间30-120s
        EEPROM.write(5, 30);                                                // 不在范围  设置为 30
    if (EE_BLETimeRead() < 150 || EE_BLETimeRead() > 3600)                  // 蓝牙休眠时间150-3600s
        EE_BLETimeWrite(150);                                               // 不在范围  设置为 150
    EEPROM.commit();                                                        // 保存

    // 丢失模式，关闭所有输出口
    if (EEPROM.read(12) == 1)
    {
        // xTaskCreatePinnedToCore(Task_AC_OFF,   // 具体实现的函数
        //                         "Task_AC_OFF", // 任务名称
        //                         1024,          // 堆栈大小
        //                         NULL,          // 输入参数
        //                         1,             // 任务优先级
        //                         NULL,          //
        //                         0              // 核心  0\1  不指定
        // );
        AC_OFF();         // 强制关闭输出，此bit不会自动清零，执行一次。    6208会自动清零，需持续关闭
        lost_Page();      // 丢失设备提示页
        vTaskDelay(5000); // 提示5s
    }
    else
        AC_ON(); // 解除，不会每次执行写操作，内部有判断

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
    // xTaskCreatePinnedToCore(Task_IO4,   // 具体实现的函数
    //                         "Task_IO4", // 任务名称
    //                         2048,       // 堆栈大小
    //                         NULL,       // 输入参数
    //                         1,          // 任务优先级
    //                         NULL,       //
    //                         0           // 核心  0\1 自动选择
    // );
    // CK22AT  2314
    if (keros_main() != 1) // CK22AT  2314
        esp_deep_sleep_start();
    // sw6306 配置初始化
    SW6306init(); // sw6306初始化   // 设置100W最大充放功率   // C2口配置为B/L口模式   // 空载时间设置min:8s

    Serial.printf("setup on core: ");
    Serial.println(xPortGetCoreID());
}

void loop()
{
    Serial.printf("loop on core: ");
    Serial.println(xPortGetCoreID());
    float bat_v, bat_a, sys_v, sys_a, ic_temp, ntc_temp, bat_m;         // 电池电压  电池电流  系统电压  系统电流   ic温度    电池温度  电池实时容量
    uint8_t bat_per, sys_state, ac_state, sinkProtocol, sourceProtocol; // 电池百分比   系统充放电状态   系统输出口状态   快充协议  快放协议
    uint8_t smalla;                                                     // 小电流  // 共用 ble set 蓝牙设置

    // 时间
    uint16_t year;                               // 年份
    uint8_t month, day, hour, minute, sec, week; //  月  日  时  分  秒  星期

    // main 当前文件用
    uint16_t cycle = 0;    // uint8_t 循环次数     暂用uint16_t预留
    uint8_t ble_state = 0; // 蓝牙开关状态

    uint8_t currentTime = EEPROM.read(5); // 睡眠时间
    unsigned long currentTime1;           // 程序执行计时
    while (currentTime >= 0)              // 屏幕睡眠时间 转次数
    {
        Serial.println("------------------------------------------------------------------SW6306-----------loop------------------------");
        sys_v = SYS_V();                                             // 系统电压
        sys_a = SYS_A();                                             // 系统 输入/输出 电流
        bat_v = Battery_V();                                         // 电池电压
        bat_a = Battery_A();                                         // 电池 输入/输出 电流
        ic_temp = IC_Temp();                                         // 芯片温度
        ntc_temp = NTC_Temp();                                       // NTC温度
        bat_m = Battery_Volume();                                    // 库仑计当前容量
        bat_per = Battery_Per();                                     // 电池电量百分比
        sys_state = SYS_State();                                     // 充放电状态   1 放电   2 充电
        ac_state = AC_State();                                       // 端口在线状态 C2是L口   0:空闲   1:C2   2:C1   3:C1C2   4:A2   5:A2C2   6:A2C1   7:A2C1C2   8:A1   9:A1C2   A:A1C1   B:A1C1C2   C:A1A2   D:A1A2C2   E:A1A2C1   F:A1A2C1C2
        EE_CycleCount(bat_per);                                      // 电池循环次数的判断
        cycle = EEPROM.read(2) / 2;                                  // 判断之后读取  电池循环次数    /2减缓次数  20-80
        PrintTime(&year, &month, &day, &hour, &minute, &sec, &week); // 获取时间数据     年 月 日 时 分 秒 周
        sinkProtocol = Sink_Protocol();                              // 充电协议
        // sourceProtocol = Source_Protocol(); // 放电协议
        smalla = Small_A_State(); // 小电流状态   0: 关    1: 开

        Serial.print("EfuseMac: ");
        Serial.println(ESP.getEfuseMac(), HEX);

        delay(20);
    beijing0:
        switch (EEPROM.read(4)) // 读取主题号
        {
        case 1:
            Theme1(bat_v, sys_v, sys_a, ic_temp, ntc_temp, bat_m, bat_per, sys_state, ac_state, sinkProtocol, sourceProtocol, smalla, ble_state, cycle); // 14个
            break;
        case 2:
            Theme2(bat_v, sys_v, sys_a, ic_temp, ntc_temp, bat_m, bat_per, sys_state, ac_state, sinkProtocol, sourceProtocol, smalla, ble_state, cycle);
            break;
        case 3:
            Theme3(bat_v, sys_v, sys_a, ic_temp, ntc_temp, bat_m, bat_per, sys_state, ac_state, sinkProtocol, sourceProtocol, smalla, ble_state, cycle, year, month, day, hour, minute, sec, week);
            break;
        case 4:
            Theme4(bat_v, sys_v, sys_a, ic_temp, ntc_temp, bat_m, bat_per, sys_state, ac_state, sinkProtocol, sourceProtocol, smalla, ble_state, cycle);
            break;
        case 5:
            Theme5(bat_v, sys_v, sys_a, ic_temp, ntc_temp, bat_m, bat_per, sys_state, ac_state, sinkProtocol, sourceProtocol, smalla, ble_state, cycle, year, month, day, hour, minute, sec, week);
            break;
        case 6:
            Theme6(bat_v, sys_v, sys_a, ic_temp, ntc_temp, bat_m, bat_per, sys_state, ac_state, sinkProtocol, sourceProtocol, smalla, ble_state, cycle);
            break;
        case 7:
            Theme7(bat_v, sys_v, sys_a, ic_temp, ntc_temp, bat_m, bat_per, sys_state, ac_state, sinkProtocol, sourceProtocol, smalla, ble_state, cycle);
            break;
        default:
            Theme1(bat_v, sys_v, sys_a, ic_temp, ntc_temp, bat_m, bat_per, sys_state, ac_state, sinkProtocol, sourceProtocol, smalla, ble_state, cycle);
            break;
        }
        if (currentTime != 0)
            currentTime--; // 睡眠时间倒计时(循环次数，大概1s/fps)   到1退出循环
        if (currentTime == 1)
            break; // 跳出大循环  睡眠
        Serial.print("LCDTime: ");
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
                            ble_state = 1;
                            switch (EEPROM.read(4)) // 读取主题号
                            {
                            case 1:
                                Theme1(bat_v, sys_v, sys_a, ic_temp, ntc_temp, bat_m, bat_per, sys_state, ac_state, sinkProtocol, sourceProtocol, smalla, ble_state, cycle); // 14个
                                break;
                            case 2:
                                Theme2(bat_v, sys_v, sys_a, ic_temp, ntc_temp, bat_m, bat_per, sys_state, ac_state, sinkProtocol, sourceProtocol, smalla, ble_state, cycle);
                                break;
                            case 3:
                                Theme3(bat_v, sys_v, sys_a, ic_temp, ntc_temp, bat_m, bat_per, sys_state, ac_state, sinkProtocol, sourceProtocol, smalla, ble_state, cycle, year, month, day, hour, minute, sec, week);
                                break;
                            case 4:
                                Theme4(bat_v, sys_v, sys_a, ic_temp, ntc_temp, bat_m, bat_per, sys_state, ac_state, sinkProtocol, sourceProtocol, smalla, ble_state, cycle);
                                break;
                            case 5:
                                Theme5(bat_v, sys_v, sys_a, ic_temp, ntc_temp, bat_m, bat_per, sys_state, ac_state, sinkProtocol, sourceProtocol, smalla, ble_state, cycle, year, month, day, hour, minute, sec, week);
                                break;
                            case 6:
                                Theme6(bat_v, sys_v, sys_a, ic_temp, ntc_temp, bat_m, bat_per, sys_state, ac_state, sinkProtocol, sourceProtocol, smalla, ble_state, cycle);
                                break;
                            case 7:
                                Theme7(bat_v, sys_v, sys_a, ic_temp, ntc_temp, bat_m, bat_per, sys_state, ac_state, sinkProtocol, sourceProtocol, smalla, ble_state, cycle);
                                break;
                            default:
                                Theme1(bat_v, sys_v, sys_a, ic_temp, ntc_temp, bat_m, bat_per, sys_state, ac_state, sinkProtocol, sourceProtocol, smalla, ble_state, cycle);
                                break;
                            }
                            bleKeyboard.begin(); // 打开蓝牙
                            delay(1000);         // 给蓝牙启动缓冲
                            while (1)
                            {
                                LcdRotation(); // 实时屏幕上下

                                sys_v = SYS_V();                                             // 系统电压
                                sys_a = SYS_A();                                             // 系统 输入/输出 电流
                                bat_v = Battery_V();                                         // 电池电压
                                bat_a = Battery_A();                                         // 电池 输入/输出 电流
                                ic_temp = IC_Temp();                                         // 芯片温度
                                ntc_temp = NTC_Temp();                                       // NTC温度
                                bat_m = Battery_Volume();                                    // 库仑计当前容量
                                bat_per = Battery_Per();                                     // 电池电量百分比
                                sys_state = SYS_State();                                     // 充放电状态   1 放电   2 充电
                                ac_state = AC_State();                                       // 端口在线状态 C2是L口   0:空闲   1:C2   2:C1   3:C1C2   4:A2   5:A2C2   6:A2C1   7:A2C1C2   8:A1   9:A1C2   A:A1C1   B:A1C1C2   C:A1A2   D:A1A2C2   E:A1A2C1   F:A1A2C1C2
                                EE_CycleCount(bat_per);                                      // 电池循环次数的判断
                                cycle = EEPROM.read(2) / 2;                                  // 判断之后读取  电池循环次数    /2减缓次数  20-80
                                PrintTime(&year, &month, &day, &hour, &minute, &sec, &week); // 获取时间数据     年 月 日 时 分 秒 周
                                sinkProtocol = Sink_Protocol();                              // 充电协议
                                // sourceProtocol = Source_Protocol(); // 放电协议
                                smalla = Small_A_State(); // 小电流状态   0: 关    1: 开

                                // 打开或关闭小电流
                                if ((smalla == 0 && EEPROM.read(8) == 1) || (smalla == 1 && EEPROM.read(8) == 0)) // smalla状态和蓝牙给的设置不一样     注意：eeprom默认255,故不能用 != 判断
                                    Small_A_ON_or_OFF();                                                          // 小电流开关

                                delay(5);
                                switch (EEPROM.read(4)) // 读取主题号
                                {
                                case 1:
                                    Theme1(bat_v, sys_v, sys_a, ic_temp, ntc_temp, bat_m, bat_per, sys_state, ac_state, sinkProtocol, sourceProtocol, smalla, ble_state, cycle); // 14个
                                    break;
                                case 2:
                                    Theme2(bat_v, sys_v, sys_a, ic_temp, ntc_temp, bat_m, bat_per, sys_state, ac_state, sinkProtocol, sourceProtocol, smalla, ble_state, cycle);
                                    break;
                                case 3:
                                    Theme3(bat_v, sys_v, sys_a, ic_temp, ntc_temp, bat_m, bat_per, sys_state, ac_state, sinkProtocol, sourceProtocol, smalla, ble_state, cycle, year, month, day, hour, minute, sec, week);
                                    break;
                                case 4:
                                    Theme4(bat_v, sys_v, sys_a, ic_temp, ntc_temp, bat_m, bat_per, sys_state, ac_state, sinkProtocol, sourceProtocol, smalla, ble_state, cycle);
                                    break;
                                case 5:
                                    Theme5(bat_v, sys_v, sys_a, ic_temp, ntc_temp, bat_m, bat_per, sys_state, ac_state, sinkProtocol, sourceProtocol, smalla, ble_state, cycle, year, month, day, hour, minute, sec, week);
                                    break;
                                case 6:
                                    Theme6(bat_v, sys_v, sys_a, ic_temp, ntc_temp, bat_m, bat_per, sys_state, ac_state, sinkProtocol, sourceProtocol, smalla, ble_state, cycle);
                                    break;
                                case 7:
                                    Theme7(bat_v, sys_v, sys_a, ic_temp, ntc_temp, bat_m, bat_per, sys_state, ac_state, sinkProtocol, sourceProtocol, smalla, ble_state, cycle);
                                    break;
                                default:
                                    Theme1(bat_v, sys_v, sys_a, ic_temp, ntc_temp, bat_m, bat_per, sys_state, ac_state, sinkProtocol, sourceProtocol, smalla, ble_state, cycle);
                                    break;
                                }
                                // 整理第 1 次发送数据
                                jsonBuffer1["agent"] = agent; // 代理人
                                // jsonBuffer1["efuseMac"] = String(ESP.getEfuseMac(), HEX);
                                jsonBuffer1["name"] = "AC1008";                  // 设备名称
                                jsonBuffer1["software"] = software;              // 固件版本
                                jsonBuffer1["hardware"] = hardware;              // 硬件版本
                                jsonBuffer1["ic_temp"] = String(ic_temp, 2);     // ic温度
                                jsonBuffer1["bat_ntc"] = String(ntc_temp, 2);    // 电池温度
                                jsonBuffer1["bat_V"] = String(bat_v, 2);         // 电池电压
                                jsonBuffer1["sys_outinv"] = String(sys_v, 2);    // 充放电压
                                jsonBuffer1["bat_A"] = String(sys_a, 2);         // 电流
                                jsonBuffer1["sys_w"] = String(sys_v * sys_a, 2); // 功率          后续小程序计算，或分段传输保留
                                jsonBuffer1["A_C"] = ac_state;                   // AC口状态
                                // jsonBuffer1["sys"] = sys_state;                  // 充放电状态       小程序无作用，后续小程序更新使用
                                jsonBuffer1["bat_m"] = String(bat_m, 2); // 电池实时容量
                                jsonBuffer1["bat_per"] = bat_per;        // 百分比bat_per
                                jsonBuffer1["bat_cir"] = cycle;          // 循环次数
                                // jsonBuffer1["sinkProtocol"] = sinkProtocol;      // 充电协议       后续小程序更新分段传值
                                // jsonBuffer1["sourceProtocol"] = sourceProtocol;  // 放电协议       后续小程序更新分段传值
                                // jsonBuffer1["smalla"] = smalla;                  // 小电流状态      后续小程序更新分段传值

                                String output1;
                                serializeJson(jsonBuffer1, output1);
                                jsonBuffer1.clear();
                                Serial.println("----------------------SendTx: 1");
                                Serial.println(output1);
                                Serial.println("----------------------");
                                bleKeyboard.sendTx(output1); // 第一次发送数据
                                output1 = "";
                                delay(50);

                                // 整理第 2 次发送数据
                                // jsonBuffer1["topic_dir"] = EEPROM.read(3);  // 屏幕方向  1  3
                                // jsonBuffer1["topic_Num"] = EEPROM.read(4);  // 主题编号
                                // jsonBuffer1["led-sleep"] = EEPROM.read(5);  // 睡眠时间  最大存储255   小程序设置值  30  60  90  120  常亮
                                // jsonBuffer1["blt_Time"] = EE_BLETimeRead(); // 蓝牙时间 eeprom: 6  7

                                // serializeJson(jsonBuffer1, output1);
                                // jsonBuffer1.clear();
                                // Serial.println("----------------------SendTx: 2");
                                // Serial.println(output1);
                                // Serial.println("----------------------");
                                // bleKeyboard.sendTx1(output1); // 第二次发送数据
                                // output1 = "";
                                // delay(50);
                                if (Rxdata.length() > 0) // 蓝牙接收数据
                                {
                                    Serial.println("----------------------Rxdata: ");
                                    Serial.println(Rxdata);
                                    DeserializationError error = deserializeJson(jsonBuffer2, Rxdata);
                                    if (error)
                                    {
                                        Serial.print(F("deserializeJson() failed: ")); // F: Flash    内容存储到flash  节省RAM空间
                                        Serial.println(error.f_str());
                                        return;
                                    }
                                    // ble set   蓝牙设置
                                    uint16_t bletime;                           // 蓝牙时间
                                    uint8_t lcdtime, theme, topic, idlock, ota; // 亮屏时间    主题    屏幕方向1上3下     OTA    ID锁
                                    // 解析JSON
                                    bletime = jsonBuffer2["bttime"];      // 读蓝牙开启状态的时间
                                    lcdtime = jsonBuffer2["btsleeptime"]; // 亮屏时间
                                    theme = jsonBuffer2["btthem"];        // 主题
                                    topic = jsonBuffer2["bttopic"];       // 屏幕方向（1上3下）	屏幕方向
                                    idlock = jsonBuffer2["btidlock"];     // ID锁
                                    ota = jsonBuffer2["btota"];           // OTA更新
                                    // 以下数据类型共用uint定义
                                    year = jsonBuffer2["btyear"];     // 年
                                    month = jsonBuffer2["btmon"];     // 月
                                    day = jsonBuffer2["btday"];       // 日
                                    hour = jsonBuffer2["bthour"];     // 时
                                    minute = jsonBuffer2["btmin"];    // 分
                                    sec = jsonBuffer2["btsec"];       // 秒
                                    week = jsonBuffer2["btweek"];     // 周
                                    smalla = jsonBuffer2["btsmalla"]; // 读小电流开关设置
                                    cycle = jsonBuffer2["btcycle"];   // 改写循环次数

                                    // 开始写入数据
                                    if (bletime != 0)
                                        EE_BLETimeWrite(bletime); // 写入蓝牙时间
                                    EEPROM.write(5, lcdtime);     // 亮屏时间    (0 / 1)
                                    if (theme != 0)
                                        EEPROM.write(4, theme); // 写入主题编号
                                    if (topic != 0)
                                        EEPROM.write(3, topic); // 写入屏幕显示方向
                                    if (idlock != 0)
                                        EEPROM.write(12, idlock); // 写1 关闭所有输出口(丢失模式)
                                    if (ota != 0)
                                        EEPROM.write(11, ota); // OTA更新  写1更新  更新处自动置零
                                    if (sec != 0 || minute != 0 || hour != 0 || day != 0 || month != 0 || year)
                                        rtc.setTime(sec, minute, hour, day, month, year); // 更新彩屏时间
                                    EEPROM.write(8, smalla);                              // 写入小电流设置    (0 / 1)
                                    if (cycle == 1)
                                    {
                                        EEPROM.write(0, 0); // 循环判断一并清零
                                        EEPROM.write(1, 0); //
                                        EEPROM.write(2, 0); // 改写循环次数  写1清零
                                    }
                                    else if (cycle != 0)
                                        EEPROM.write(2, cycle); // 改写循环次数   (实际值 = 写入的值 / 2)
                                    EEPROM.commit();            // 保存
                                    vTaskDelay(10);
                                    Rxdata = ""; // 清空
                                    Serial.println("----------------------");
                                }
                                Serial.print("Topic: ");
                                Serial.println(EEPROM.read(3));
                                Serial.print("Theme: ");
                                Serial.println(EEPROM.read(4));
                                Serial.print("LCDTime: ");
                                Serial.println(EEPROM.read(5));
                                Serial.print("BTtime: ");
                                Serial.println(EE_BLETimeRead());
                                Serial.print("SmallA: ");
                                Serial.println(EEPROM.read(8));
                                Serial.print("OTA: ");
                                Serial.println(EEPROM.read(11));
                                Serial.print("AC_OFF: ");
                                Serial.println(EEPROM.read(12));
                                Serial.print("Cycles: ");
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
