#ifndef _Background_H
#define _Background_H
void DisplayInit(); // 初始化函数
void LcdRotation(); // 连接蓝牙实时刷新屏幕方向

void RefreshDisplay(); // 刷新画面
void sys_init(uint16_t timenum);
void PowerLOGO(char imgName);

//---------------主题-----------
// 经典主题  HUA
void Theme1(float bat_v, float sys_v, float sys_a, float ic_temp, float ntc_temp, float bat_m,
            uint8_t bat_per, uint8_t sys_state, uint8_t ac_state, uint8_t sinkProtocol, uint8_t sourceProtocol, uint8_t smalla, uint8_t ble_state, uint8_t cycle);
// 闪极风格 HUA
void Theme2(float bat_v, float sys_v, float sys_a, float ic_temp, float ntc_temp, float bat_m,
            uint8_t bat_per, uint8_t sys_state, uint8_t ac_state, uint8_t sinkProtocol, uint8_t sourceProtocol, uint8_t smalla, uint8_t ble_state, uint8_t cycle);
// win10风格  小东
void Theme3(float bat_v, float sys_v, float sys_a, float ic_temp, float ntc_temp, float bat_m,
            uint8_t bat_per, uint8_t sys_state, uint8_t ac_state, uint8_t sinkProtocol, uint8_t sourceProtocol, uint8_t smalla, uint8_t ble_state, uint8_t cycle,
            uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t sec, uint8_t week);
// 极简主题  貹
void Theme4(float bat_v, float sys_v, float sys_a, float ic_temp, float ntc_temp, float bat_m,
            uint8_t bat_per, uint8_t sys_state, uint8_t ac_state, uint8_t sinkProtocol, uint8_t sourceProtocol, uint8_t smalla, uint8_t ble_state, uint8_t cycle);
// 手机风格  HUA
void Theme5(float bat_v, float sys_v, float sys_a, float ic_temp, float ntc_temp, float bat_m,
            uint8_t bat_per, uint8_t sys_state, uint8_t ac_state, uint8_t sinkProtocol, uint8_t sourceProtocol, uint8_t smalla, uint8_t ble_state, uint8_t cycle,
            uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t sec, uint8_t week);
// 新简风  HUA
void Theme6(float bat_v, float sys_v, float sys_a, float ic_temp, float ntc_temp, float bat_m,
            uint8_t bat_per, uint8_t sys_state, uint8_t ac_state, uint8_t sinkProtocol, uint8_t sourceProtocol, uint8_t smalla, uint8_t ble_state, uint8_t cycle);
// 全功能主题  HUA
void Theme7(float bat_v, float sys_v, float sys_a, float ic_temp, float ntc_temp, float bat_m,
            uint8_t bat_per, uint8_t sys_state, uint8_t ac_state, uint8_t sinkProtocol, uint8_t sourceProtocol, uint8_t smalla, uint8_t ble_state, uint8_t cycle);
//------------------

void offscreen();
void onscreen();

void lost_Page(); // 非正当获得设备采取措施

void WiFi_Page();            // 等待连接WiFi页面
void ota_AP();               // 配网信息提示
void ota_Page(int a, int b); // 更新页面

#endif
