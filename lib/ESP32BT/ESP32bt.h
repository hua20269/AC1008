// uncomment the following line to use NimBLE library
// #define USE_NIMBLE

#ifndef _ESP32BT_H
#define _ESP32BT_H
#include "sdkconfig.h"
#if defined(CONFIG_BT_ENABLED)

#if defined(USE_NIMBLE)

#include "NimBLECharacteristic.h"
#include "NimBLEHIDDevice.h"

#define BLEDevice NimBLEDevice
#define BLEServerCallbacks NimBLEServerCallbacks
#define BLECharacteristicCallbacks NimBLECharacteristicCallbacks
#define BLEHIDDevice NimBLEHIDDevice
#define BLECharacteristic NimBLECharacteristic
#define BLEAdvertising NimBLEAdvertising
#define BLEServer NimBLEServer
#define BLEService NimBLEService
#else

#include "BLEHIDDevice.h"
#include "BLECharacteristic.h"

#endif // USE_NIMBLE

#include "Print.h"

// #define SERVICE_UUID "A112750F-73C3-9381-5F7B-F0B89811CDA5"
// #define CHARACTERISTIC_UUID_RX "641FFEED-701E-E888-01B5-364CB5F77BB4"
// #define CHARACTERISTIC_UUID_TX "B84978D7-8BDE-3D7A-691E-DFDAEF5638B7"

#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"            // 串口服务UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"  // 串口接收UUID
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"  // 串口发送UUID
// #define CHARACTERISTIC_UUID_TX1 "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"  // 串口发送UUID
#define CHARACTERISTIC_UUID_TX1 "e8cf3944-f601-d5ed-2716-0360c42baa4a" // 串口发送UUID1
extern String Rxdata;                                                  // 串口接收数据

#define BLE_KEYBOARD_VERSION "0.0.4"
#define BLE_KEYBOARD_VERSION_MAJOR 0
#define BLE_KEYBOARD_VERSION_MINOR 0
#define BLE_KEYBOARD_VERSION_REVISION 4

const uint8_t KEY_LEFT_CTRL = 0x80;   // 键盘左CTRL
const uint8_t KEY_LEFT_SHIFT = 0x81;  // 键盘左SHIFT
const uint8_t KEY_LEFT_ALT = 0x82;    // 键盘左ALT
const uint8_t KEY_LEFT_GUI = 0x83;    // 键盘左GUI
const uint8_t KEY_RIGHT_CTRL = 0x84;  // 键盘右CTRL
const uint8_t KEY_RIGHT_SHIFT = 0x85; // 键盘右SHIFT
const uint8_t KEY_RIGHT_ALT = 0x86;   // 键盘右ALT
const uint8_t KEY_RIGHT_GUI = 0x87;   // 键盘右GUI

const uint8_t KEY_UP_ARROW = 0xDA;    // 键盘上箭头
const uint8_t KEY_DOWN_ARROW = 0xD9;  // 键盘下箭头
const uint8_t KEY_LEFT_ARROW = 0xD8;  // 键盘左箭头
const uint8_t KEY_RIGHT_ARROW = 0xD7; // 键盘右箭头
const uint8_t KEY_BACKSPACE = 0xB2;   // 键盘退格键
const uint8_t KEY_TAB = 0xB3;         // 键盘TAB键
const uint8_t KEY_RETURN = 0xB0;      // 键盘回车键
const uint8_t KEY_ESC = 0xB1;         // 键盘ESC键
const uint8_t KEY_INSERT = 0xD1;      // 键盘插入键
const uint8_t KEY_PRTSC = 0xCE;       // 键盘打印键
const uint8_t KEY_DELETE = 0xD4;      // 键盘删除键
const uint8_t KEY_PAGE_UP = 0xD3;     // 键盘上翻页键
const uint8_t KEY_PAGE_DOWN = 0xD6;   // 键盘下翻页键
const uint8_t KEY_HOME = 0xD2;        // 键盘HOME键
const uint8_t KEY_END = 0xD5;         // 键盘END键
const uint8_t KEY_CAPS_LOCK = 0xC1;   // 键盘大写锁定键
const uint8_t KEY_F1 = 0xC2;          // 键盘F1键
const uint8_t KEY_F2 = 0xC3;
const uint8_t KEY_F3 = 0xC4;
const uint8_t KEY_F4 = 0xC5;
const uint8_t KEY_F5 = 0xC6;
const uint8_t KEY_F6 = 0xC7;
const uint8_t KEY_F7 = 0xC8;
const uint8_t KEY_F8 = 0xC9;
const uint8_t KEY_F9 = 0xCA;
const uint8_t KEY_F10 = 0xCB;
const uint8_t KEY_F11 = 0xCC;
const uint8_t KEY_F12 = 0xCD;
const uint8_t KEY_F13 = 0xF0;
const uint8_t KEY_F14 = 0xF1;
const uint8_t KEY_F15 = 0xF2;
const uint8_t KEY_F16 = 0xF3;
const uint8_t KEY_F17 = 0xF4;
const uint8_t KEY_F18 = 0xF5;
const uint8_t KEY_F19 = 0xF6;
const uint8_t KEY_F20 = 0xF7;
const uint8_t KEY_F21 = 0xF8;
const uint8_t KEY_F22 = 0xF9;
const uint8_t KEY_F23 = 0xFA;
const uint8_t KEY_F24 = 0xFB; // 键盘F24键

const uint8_t KEY_NUM_0 = 0xEA; // 键盘小键盘0
const uint8_t KEY_NUM_1 = 0xE1;
const uint8_t KEY_NUM_2 = 0xE2;
const uint8_t KEY_NUM_3 = 0xE3;
const uint8_t KEY_NUM_4 = 0xE4;
const uint8_t KEY_NUM_5 = 0xE5;
const uint8_t KEY_NUM_6 = 0xE6;
const uint8_t KEY_NUM_7 = 0xE7;
const uint8_t KEY_NUM_8 = 0xE8;
const uint8_t KEY_NUM_9 = 0xE9;        // 键盘小键盘9
const uint8_t KEY_NUM_SLASH = 0xDC;    // 键盘小键盘/
const uint8_t KEY_NUM_ASTERISK = 0xDD; // 键盘小键盘*
const uint8_t KEY_NUM_MINUS = 0xDE;    // 键盘小键盘-
const uint8_t KEY_NUM_PLUS = 0xDF;     // 键盘小键盘+
const uint8_t KEY_NUM_ENTER = 0xE0;    // 键盘小键盘回车
const uint8_t KEY_NUM_PERIOD = 0xEB;   // 键盘小键盘.

typedef uint8_t MediaKeyReport[2]; // 媒体键报告

const MediaKeyReport KEY_MEDIA_NEXT_TRACK = {1, 0};                      // 媒体下一曲
const MediaKeyReport KEY_MEDIA_PREVIOUS_TRACK = {2, 0};                  // 媒体上一曲
const MediaKeyReport KEY_MEDIA_STOP = {4, 0};                            // 媒体停止
const MediaKeyReport KEY_MEDIA_PLAY_PAUSE = {8, 0};                      // 媒体播放/暂停
const MediaKeyReport KEY_MEDIA_MUTE = {16, 0};                           // 媒体静音
const MediaKeyReport KEY_MEDIA_VOLUME_UP = {32, 0};                      // 媒体音量+
const MediaKeyReport KEY_MEDIA_VOLUME_DOWN = {64, 0};                    // 媒体音量-
const MediaKeyReport KEY_MEDIA_WWW_HOME = {128, 0};                      // 媒体主页
const MediaKeyReport KEY_MEDIA_LOCAL_MACHINE_BROWSER = {0, 1};           // 打开本地机器的浏览器
const MediaKeyReport KEY_MEDIA_CALCULATOR = {0, 2};                      // 打开计算器
const MediaKeyReport KEY_MEDIA_WWW_BOOKMARKS = {0, 4};                   // 打开书签
const MediaKeyReport KEY_MEDIA_WWW_SEARCH = {0, 8};                      // 打开搜索
const MediaKeyReport KEY_MEDIA_WWW_STOP = {0, 16};                       // 停止搜索
const MediaKeyReport KEY_MEDIA_WWW_BACK = {0, 32};                       // 后退
const MediaKeyReport KEY_MEDIA_CONSUMER_CONTROL_CONFIGURATION = {0, 64}; // Media Selection
const MediaKeyReport KEY_MEDIA_EMAIL_READER = {0, 128};                  // 打开邮件阅读器

//  Low level key report: up to 6 keys and shift, ctrl etc at once
typedef struct
{
    uint8_t modifiers;
    uint8_t reserved;
    uint8_t keys[6];
} KeyReport;

/**
 * @brief // 键盘类
 *
 */
class BleKeyboard : public Print, public BLEServerCallbacks, public BLECharacteristicCallbacks
{
private:
    BLEServer *pServer;                    // BLE服务
    BLEHIDDevice *hid;                     // HID设备
    BLEService *pService;                  // BLE服务
    BLECharacteristic *inputKeyboard;      // 键盘输入特征
    BLECharacteristic *outputKeyboard;     // 键盘输出特征
    BLECharacteristic *inputMediaKeys;     // 媒体键输入特征
    BLECharacteristic *pTxCharacteristic;  // 串口发送特征
    BLECharacteristic *pTxCharacteristic1; // 串口发送特征
    BLECharacteristic *pRxCharacteristic;  // 串口接收特征
    BLEAdvertising *advertising;           // BLE广播
    KeyReport _keyReport;                  // 键盘报告
    MediaKeyReport _mediaKeyReport;        // 媒体键报告
    std::string deviceName;                // 设备名称
    std::string deviceManufacturer;        // 设备制造商
    uint8_t batteryLevel;                  // 电池电量
    bool connected = false;                // 是否连接
    uint32_t _delay_ms = 7;                // 延时时间
    void delay_ms(uint64_t ms);            // 延时函数

    uint16_t vid = 0x05ac;     // 苹果公司
    uint16_t pid = 0x820a;     // 键盘
    uint16_t version = 0x0210; // 键盘版本

public:
    BleKeyboard(std::string deviceName = "ESP32 Keyboard", std::string deviceManufacturer = "Espressif", uint8_t batteryLevel = 100); // 构造函数
    void begin(void);                                                                                                                 // 开始
    void end(void);                                                                                                                   // 结束
    void sendTx(String output1);                                                                                                      // 串口发送数据
    void sendTx1(String output1);                                                                                                     // 串口发送数据
    void sendReport(KeyReport *keys);                                                                                                 // 发送键盘报告
    void sendReport(MediaKeyReport *keys);                                                                                            // 发送媒体键报告
    size_t press(uint8_t k);                                                                                                          // 按下按键
    size_t press(const MediaKeyReport k);                                                                                             // 按下媒体键
    size_t release(uint8_t k);                                                                                                        // 释放按键
    size_t release(const MediaKeyReport k);                                                                                           // 释放媒体键
    size_t write(uint8_t c);                                                                                                          // 写入按键
    size_t write(const MediaKeyReport c);                                                                                             // 写入媒体键
    size_t write(const uint8_t *buffer, size_t size);                                                                                 // 写入数据
    void releaseAll(void);                                                                                                            // 释放所有按键
    bool isConnected(void);                                                                                                           // 是否连接
    void setBatteryLevel(uint8_t level);                                                                                              // 设置电池电量
    void setName(std::string deviceName);                                                                                             // 设置设备名称
    void setDelay(uint32_t ms);                                                                                                       // 设置延时时间

    void set_vendor_id(uint16_t vid);   // 设置厂商ID
    void set_product_id(uint16_t pid);  // 设置产品ID
    void set_version(uint16_t version); // 设置版本号

protected:
    virtual void onStarted(BLEServer *pServer) {};          // 开始
    virtual void onConnect(BLEServer *pServer) override;    // 连接
    virtual void onDisconnect(BLEServer *pServer) override; // 断开连接
    virtual void onWrite(BLECharacteristic *me) override;   // 写入
};

#endif // CONFIG_BT_ENABLED
#endif // ESP32_BLE_KEYBOARD_H
