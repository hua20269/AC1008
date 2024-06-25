#include <SW6208.h>
#include <Background.h>
#include <Ticker.h>
#include <ESP32bt.h>
#include <ck22.h>
#include <eeprom32.h>
#include <ArduinoJson.h>
#include <espds1302.h>
#include <OneButton.h>
#include "ota.h"
#include <WiFi.h>
#include <esp_task_wdt.h>

#define BUTTON_PIN_BITMASK 0x0010 // GPIOs 4    io4 按钮

BleKeyboard bleKeyboard("AC1008M", "OCRC", 50); // 蓝牙

uint8_t xunhuanh = 0, xunhuanl = 0, bt_icon = 0, sleeptime = 30, espthem = 4, yan = 0, otatishi = 1;
uint16_t xunhuan = 0;
Ticker time10;
OneButton button(4, true);

void Task_OTA(void *pvParameters);    // OTA更新 子线程
void Task_AC_OFF(void *pvParameters); // 关闭所有输出口 子线程

// void app2(void *pvParameters);     // cpu1函数2  按键
// void doubleclick();                // 双击
void time1_callback() // 定时函数   熄灭屏幕   进入深度睡眠
{
    esp_deep_sleep_start();
}

void setup()
{
    // esp_task_wdt_add(Task_OTA);
    Serial.begin(115200);
    pinMode(4, INPUT_PULLUP);
    pinMode(27, INPUT_PULLUP);
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_27, 0);                              // 唤醒引脚配置 低电平唤醒
    esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK, ESP_EXT1_WAKEUP_ALL_LOW); // 唤醒引脚配置 低电平唤醒
    IICinit();
    EEPROM.begin(EEPROM_SIZE); // 初始化 EEPROM 寄存器

    if (EEPROM.read(5) < 10 || EEPROM.read(5) > 120 && EEPROM.read(5) != 240) // 亮屏时间  30-120
    {
        EEPROM.write(5, sleeptime);
        EEPROM.commit();
    }

    if (I2C_Read(SW6208_address, 0x03) != 0x4F || I2C_Read(SW6208_address, 0x30) != 0x4 || I2C_Read(SW6208_address, 0x33) != 0x3) // 小电流预打开
    {
        I2C_Write(SW6208_address, 0x03, 0x4F); // 0100 1111  //1: 进入小电流充电模式   0: 在小电流充电和WLED都支持时，优先响应小电流充电模式    3: 仅显示电量
        I2C_Write(SW6208_address, 0x30, 0x4);  // 0100  // 轻载检测电流设置 VOUT<7.65V 或者 VOUT>7.65V 且 reg0x30[0]=0:  默认:55mA  此设置为:10mA
        I2C_Write(SW6208_address, 0x33, 0x3);  // 0011  // 小电流使能
    }
    // NTCLimit(); // 设置NTC温度上限保护
    DisplayInit(); // 显示初始化
    // PowerLOGO();   // 开机LOGO
    // delay(1000);
    if (EEPROM.read(12) == 1) // 锁死设备 关闭所有输出口
    {
        xTaskCreatePinnedToCore(Task_AC_OFF,   // 具体实现的函数
                                "Task_AC_OFF", // 任务名称
                                1024,          // 堆栈大小
                                NULL,          // 输入参数
                                1,             // 任务优先级
                                NULL,          //
                                0              // 核心  0\1  不指定
        );
        lost_Page();         // 丢失设备提示页
        vTaskDelay(30000);   // 提示30s
        EEPROM.write(5, 10); // 屏幕睡眠改10s
        eepromwrite(150);    // 蓝牙开启时间150s
        EEPROM.commit();
    }
    /**
     * OTA更新
     */
    if (EEPROM.read(11) == 1) // 小程序给更新确认
    {
        EEPROM.write(11, 0); // 写非1  更新完成
        EEPROM.commit();     // 保存
        Serial.printf("main on core: ");
        Serial.println(xPortGetCoreID());
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
    if (keros_main() != 1)
    {
        esp_deep_sleep_start();
    }
    button.reset(); // 清除按钮状态机的状态
    // button.attachDoubleClick(doubleclick);   // 注册双击
    // delay(600);                              // 600
    // xTaskCreatePinnedToCore(app2,            // 具体实现的函数
    //                         "APP2_CPU_LOOP", // 任务名称
    //                         8024,            // 堆栈大小
    //                         NULL,            // 输入参数
    //                         1,               // 任务优先级
    //                         NULL,            //
    //                         NULL             // 核心  0\1 自动选择
    // );
    Serial.println("ESP32 is restart now!");
}

void loop()
{
    delay(200);
    float battery_V, sys_outinv, ic_temp, ntc_v, battery_A, sys_w, bat_m, bat_ntc; // 变量电池电压   系统输入输出电压   ic温度   ntc电压   输入/输出电流   系统功率   电池容量   电池温度
    uint16_t year, time, pass;                                                     // 年份   读蓝牙链接时间    四位密码
    uint8_t bat_per, sys, A_C, month, day, hour, minute, sec, week;                // 电池百分比   系统充放电状态   5个接口状态    月  日  时  分  秒  星期
    uint8_t smalla, a1, c1, topic, ota, idlock;                                    // 小电流开关   A1口状态1开0关     C1口状态1开0关      屏幕方向1上3下     OTA    ID锁
    uint8_t currentTime = EEPROM.read(5);                                          // 睡眠时间
    unsigned long currentTime1;

    while (currentTime > 1) // lcdsleeptime
    {
        battery_V = batteryV(); // 电池电压
        delay(5);
        sysstate(&sys, &A_C, &battery_A); // 冲放电状态及输出电流的判断   1A  4C  5AC
        delay(5);
        sys_outinv = battery_outinV(); // 输入输出电压
        ic_temp = battery_ictemp();    // ic温度
        delay(5);
        sys_w = sys_outinv * battery_A; // 系统功率大小
        bat_per = battery_per();        // 电池百分比
        delay(5);
        bat_m = battery_volume() * bat_per / 100; // 电池容量判断  实时容量    总容量x电池百分比
        bat_ntc = battery_ntcV();                 // 电池温度
        delay(5);
        battime(bat_per);             // 电池循环次数的判断
        xunhuan = EEPROM.read(2) / 2; // 判断之后读取  电池循环次数
        delay(5);
        printTime(&year, &month, &day, &hour, &minute, &sec, &week); // 从DS1302获取时间数据     年 月 日 时 分 秒 周
        delay(5);
        smalla = xdlzt(); // 小电流状态

        Serial.println(ESP.getEfuseMac() & 0X0000FFFFFFFFFFFF, HEX); // chipID  //MAC

    beijing0:
        switch (EEPROM.read(4)) // 读取主题号
        // switch (5)
        {
        case 1:
            if (yan == 1)
            {
                for (uint16_t i = 1; i < 20; i++)
                {
                    Backgroundyan(i);
                    delay(50);
                    if (digitalRead(4) == 0)
                    {
                        yan ^= 1;
                        goto beijing1;
                        break;
                    }
                }
            }
            else
            beijing1:
                Background1(battery_V, battery_A, A_C, ic_temp, sys_outinv, sys_w, sys, bat_m, xunhuan, bat_per, bat_ntc, bt_icon); // 显示相关参数
            break;
        case 2:
            if (yan == 1)
            {
                for (uint16_t i = 1; i < 20; i++)
                {
                    Backgroundyan(i);
                    delay(50);
                    if (digitalRead(4) == 0)
                    {
                        yan ^= 1;
                        goto beijing2;
                        break;
                    }
                }
            }
            else
            beijing2:
                BackgroundTime2(A_C, bt_icon, sys_outinv, battery_A, sys_w, ic_temp, bat_ntc, bat_per, xunhuan);
            break;
        case 3:
            if (yan == 1)
            {
                for (uint16_t i = 1; i < 20; i++)
                {
                    Backgroundyan(i);
                    delay(50);
                    if (digitalRead(4) == 0)
                    {
                        yan ^= 1;
                        goto beijing3;
                        break;
                    }
                }
            }
            else
            {
            beijing3:
                BackgroundTime3(week, battery_V, sys_outinv, sys, A_C, battery_A, sys_w, bat_m, xunhuan, bat_per, bt_icon);
                BackgroundTime3_2(month, day, bat_ntc, ic_temp, hour, minute, sec, smalla);
            }
            break;
        case 4:
            if (yan == 1)
            {
                for (uint16_t i = 1; i < 20; i++)
                {
                    Backgroundyan(i);
                    delay(50);
                    if (digitalRead(4) == 0)
                    {
                        yan ^= 1;
                        goto beijing4;
                        break;
                    }
                }
            }
            else
            {
            beijing4:
                BackgroundTime4(battery_V, sys_outinv, sys, battery_A, sys_w, bat_per, bt_icon, ic_temp, bat_ntc);
            }
            break;
        case 5:
            if (yan == 1)
            {
                for (uint16_t i = 1; i < 20; i++)
                {
                    Backgroundyan(i);
                    delay(50);
                    if (digitalRead(4) == 0)
                    {
                        yan ^= 1;
                        goto beijing5;
                        break;
                    }
                }
            }
            else
            {
            beijing5:
                BackgroundTime5(battery_V, sys_outinv, sys, A_C, battery_A, sys_w, bat_per, bat_m, bt_icon, ic_temp, bat_ntc, month, day, hour, minute, sec, week);
            }
            break;
        default:
            if (yan == 1)
            {
                for (uint16_t i = 1; i < 20; i++)
                {
                    Backgroundyan(i);
                    delay(100);
                    if (digitalRead(4) == 0)
                    {
                        yan ^= 1;
                        goto beijing10;
                        break;
                    }
                }
            }
            else
            {
            beijing10:
                BackgroundTime5(battery_V, sys_outinv, sys, A_C, battery_A, sys_w, bat_per, bat_m, bt_icon, ic_temp, bat_ntc, month, day, hour, minute, sec, week);
            }
            break;
        }
        if (EEPROM.read(5) < 240)
            currentTime--; // 睡眠时间倒计时   到0退出循环
        Serial.print("lcdSleepTime: ");
        Serial.println(currentTime);
        // else EEPROM.read(5) == 255 不倒计时   屏幕常亮
        currentTime1 = millis();                     // 板运行当前程序的时间
        while (millis() - currentTime1 < 1000 - 165) // 延时 大概 1s 刷新 一次   以上执行时间165ms↔
        {
            if (digitalRead(4) == 0)
            {
                delay(150);              // 消抖
                if (digitalRead(4) == 0) // 此处进入眼睛动画
                {
                    delay(300);                // 延时
                    if (digitalRead(4) == LOW) // 拉低准备进入蓝牙
                    {
                        delay(1000);               // 延时
                        if (digitalRead(4) == LOW) // 进入蓝牙
                        {
                            // bleKeyboard.begin();                           // 打开蓝牙
                            // delay(1000);                                   // 给蓝牙启动缓冲
                            if (eepromread() < 150 || eepromread() > 3600) // 蓝牙休眠时间  150s
                            {
                                time10.once(150, time1_callback); // 定时休眠
                            }
                            else
                                time10.once(eepromread(), time1_callback); // 小程序改的蓝牙打开时间  150-3600
                            DynamicJsonDocument jsonBuffer1(512);
                            DynamicJsonDocument jsonBuffer2(512);
                            bt_icon = 1;
                            switch (EEPROM.read(4))
                            {
                            case 1:
                                Background1(battery_V, battery_A, A_C, ic_temp, sys_outinv, sys_w, sys, bat_m, xunhuan, bat_per, bat_ntc, bt_icon); // 显示相关参数
                                break;
                            case 2:
                                BackgroundTime2(A_C, bt_icon, sys_outinv, battery_A, sys_w, ic_temp, bat_ntc, bat_per, xunhuan);
                                break;
                            case 3:
                                BackgroundTime3(week, battery_V, sys_outinv, sys, A_C, battery_A, sys_w, bat_m, xunhuan, bat_per, bt_icon);
                                BackgroundTime3_2(month, day, bat_ntc, ic_temp, hour, minute, sec, smalla);
                                break;
                            case 4:
                                BackgroundTime4(battery_V, sys_outinv, sys, battery_A, sys_w, bat_per, bt_icon, ic_temp, bat_ntc);
                                break;
                            case 5:
                                BackgroundTime5(battery_V, sys_outinv, sys, A_C, battery_A, sys_w, bat_per, bat_m, bt_icon, ic_temp, bat_ntc, month, day, hour, minute, sec, week);
                                break;
                            default:
                                BackgroundTime5(battery_V, sys_outinv, sys, A_C, battery_A, sys_w, bat_per, bat_m, bt_icon, ic_temp, bat_ntc, month, day, hour, minute, sec, week);
                                break;
                            }
                            bleKeyboard.begin(); // 打开蓝牙
                            // delay(1000);                                   // 给蓝牙启动缓冲
                            delay(1000);
                            while (1)
                            {
                                lcdRotation();                                               // 实时屏幕上下
                                xunhuan = EEPROM.read(2) / 2;                                // 判断之后读取  电池循环次数
                                printTime(&year, &month, &day, &hour, &minute, &sec, &week); // 从DS1302获取时间数据     年 月 日 时 分 秒 周
                                battery_V = batteryV();                                      // 电池电压
                                delay(5);
                                sysstate(&sys, &A_C, &battery_A); // 冲放电状态及输出电流的判断
                                delay(5);
                                sys_outinv = battery_outinV(); // 输入输出电压
                                ic_temp = battery_ictemp();    // ic温度
                                delay(5);
                                sys_w = sys_outinv * battery_A;           // 系统功率大小
                                bat_per = battery_per();                  // 电池百分比
                                bat_m = battery_volume() * bat_per / 100; // 电池容量判断
                                bat_ntc = battery_ntcV();                 // 电池温度

                                topic = EEPROM.read(3); // 读用户设置的值（1上3下）屏幕方向
                                time = eepromread();    // 读蓝牙链接时间

                                smalla = xdlzt(); // 读取小电流状态
                                Serial.print("smalla: ");
                                Serial.println(smalla);

                                if (smalla == 0 && EEPROM.read(8) == 1) // smalla: OFF   //蓝牙给的设置 1 让打开小电流
                                {
                                    A2_ON();
                                    kqxdl();          // 写1打开小电流
                                    A2_OFF();         // 开启小电流之后关闭A2口   本设备没有A2口，故不受影响
                                    smalla = xdlzt(); // 读取小电流状态
                                    Serial.print("smallaON: ok ");
                                    Serial.println(smalla);
                                    delay(1000);
                                    Serial.println(xdlzt());
                                }
                                else if (smalla == 1 && EEPROM.read(8) == 0)
                                {
                                    A2_ON();
                                    kqxdl(); // 再次写1关闭小电流
                                    A2_OFF();
                                    smalla = xdlzt(); // 读取小电流状态
                                    Serial.print("smallaOFF: ok ");
                                    Serial.println(smalla);
                                    delay(1000);
                                    Serial.println(xdlzt());
                                }

                                delay(5);
                                switch (EEPROM.read(4))
                                {
                                case 1:
                                    Background1(battery_V, battery_A, A_C, ic_temp, sys_outinv, sys_w, sys, bat_m, xunhuan, bat_per, bat_ntc, bt_icon); // 显示相关参数
                                    break;
                                case 2:
                                    BackgroundTime2(A_C, bt_icon, sys_outinv, battery_A, sys_w, ic_temp, bat_ntc, bat_per, xunhuan);
                                    break;
                                case 3:
                                    BackgroundTime3(week, battery_V, sys_outinv, sys, A_C, battery_A, sys_w, bat_m, xunhuan, bat_per, bt_icon);
                                    BackgroundTime3_2(month, day, bat_ntc, ic_temp, hour, minute, sec, smalla);
                                    break;
                                case 4:
                                    BackgroundTime4(battery_V, sys_outinv, sys, battery_A, sys_w, bat_per, bt_icon, ic_temp, bat_ntc);
                                    break;
                                case 5:
                                    BackgroundTime5(battery_V, sys_outinv, sys, A_C, battery_A, sys_w, bat_per, bat_m, bt_icon, ic_temp, bat_ntc, month, day, hour, minute, sec, week);
                                    break;
                                default:
                                    BackgroundTime5(battery_V, sys_outinv, sys, A_C, battery_A, sys_w, bat_per, bat_m, bt_icon, ic_temp, bat_ntc, month, day, hour, minute, sec, week);
                                    break;
                                }
                                jsonBuffer1["chipid"] = ESP.getEfuseMac() & 0X0000FFFFFFFFFFFF;
                                jsonBuffer1["name"] = "AC1008M";                     // 设备名称
                                jsonBuffer1["software"] = "v3.0";                  // 固件版本
                                jsonBuffer1["hardware"] = "v3.1";                  // 硬件版本
                                jsonBuffer1["bat_cir"] = xunhuan;                  // 循环次数
                                jsonBuffer1["bat_V"] = String(battery_V, 3);       // 电池电压
                                jsonBuffer1["bat_A"] = String(battery_A, 3);       // 电流
                                jsonBuffer1["A_C"] = A_C;                          // AC口状态
                                jsonBuffer1["ic_temp"] = String(ic_temp, 3);       // ic温度
                                jsonBuffer1["sys_outinv"] = String(sys_outinv, 3); // 系统充放电压
                                jsonBuffer1["sys_w"] = String(sys_w, 3);           // 系统功率
                                jsonBuffer1["sys"] = sys;                          // 充放电状态
                                jsonBuffer1["bat_m"] = String(bat_m, 3);           // 电池容量
                                jsonBuffer1["bat_per"] = bat_per;                  // 百分比bat_per
                                jsonBuffer1["bat_ntc"] = String(bat_ntc, 3);       // 电池温度

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
                                // if (EEPROM.read(5) == 240)                 // 240 常亮   小程序接收0
                                //     jsonBuffer1["led-sleep"] = 0;
                                // else
                                //     jsonBuffer1["led-sleep"] = EEPROM.read(5); // 睡眠时间  最大存储255   小程序设置值  30  60  90  120  常亮
                                // jsonBuffer1["topic_dir"] = EEPROM.read(3);     // 屏幕方向  1  3
                                // jsonBuffer1["blt_Time"] = eepromread();        // 蓝牙打开及连接时间
                                // jsonBuffer1["small_A"] = xdlzt();              // 读取最新的小电流状态  发送给小程序

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
                                    xunhuan = jsonBuffer2["btxunhuan"];     // 改写循环次数

                                    // 开始写入数据
                                    EEPROM.write(12, idlock); // 写1锁死ESP32 或 关闭所有输出口
                                    delay(5);
                                    initRTCtime(year, month, day, hour, minute, sec + 2, week); // 更新彩屏时间
                                    delay(5);
                                    if (espthem != 0)
                                        EEPROM.write(4, espthem); // 写入主题编号
                                    delay(5);
                                    if (sleeptime == 0)
                                        sleeptime = 240;        // 常亮
                                    EEPROM.write(5, sleeptime); // 写入睡眠时间
                                    delay(5);
                                    if (topic != 0)
                                        EEPROM.write(3, topic); // 写入屏幕显示方向
                                    delay(5);
                                    if (time != 0)
                                        eepromwrite(time); // 写入蓝牙开启时间
                                    delay(5);
                                    EEPROM.write(8, smalla); // 写入小电流设置
                                    delay(5);
                                    if (ota != 0)
                                        EEPROM.write(11, ota); // OTA更新  写1更新自动置零
                                    delay(5);
                                    if (xunhuan == 1)
                                        EEPROM.write(2, 0); // 改写循环次数
                                    else if (xunhuan != 0)
                                        EEPROM.write(2, xunhuan); // 改写循环次数
                                    delay(5);
                                    EEPROM.commit(); // 保存
                                    delay(5);
                                    Rxdata = ""; // 清空
                                    Serial.println("*************");
                                    Serial.println("RxEnd ! ! !");
                                    Serial.println("*************");
                                }
                                Serial.print("屏幕方向：");
                                Serial.println(EEPROM.read(3));
                                Serial.print("主题：");
                                Serial.println(EEPROM.read(4));
                                Serial.print("息屏时间：");
                                Serial.println(EEPROM.read(5));
                                Serial.print("蓝牙时间：");
                                Serial.println(eepromread());
                                Serial.print("小电流：");
                                Serial.println(EEPROM.read(8));
                                Serial.print("OTA: ");
                                Serial.println(EEPROM.read(11));
                                Serial.print("AC_OFF: ");
                                Serial.println(EEPROM.read(12));
                                Serial.print("循环次数: ");
                                Serial.println(EEPROM.read(2));

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
                        // continue;
                    }
                    yan ^= 1;
                    delay(100);
                    goto beijing0;
                }
            }
        }
    }
    // 屏幕进入睡眠
    if (digitalRead(27) == 1)
    {
        esp_deep_sleep_start(); // 重启
    }
    offscreen(); // 熄灭屏幕
    if (digitalRead(27) == 0)
    {
        while (1)
        {
            if (digitalRead(4) == 0)
            {
                yan == 1;
                onscreen(); // 开启屏幕
                break;
            }
            if (digitalRead(27) == 1)
            {
                onscreen();
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
        AC_OFF();                         // 关闭AC口输出
        vTaskDelay(1000);                 // 延时  及喂狗
    }
}