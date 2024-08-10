#ifndef _Background_H
#define _Background_H
void DisplayInit(); // 初始化函数
void lcdRotation(); // 连接蓝牙实时刷新屏幕方向

void RefreshDisplay(); // 刷新画面
void sys_init(uint16_t timenum);
void PowerLOGO();

//-------------------sw6306

void lcdlayout01(uint16_t bat_circ, uint8_t bat_per, float battery_V, float ic_temp, float sys_outinv, float battery_A, float bat_ntc, uint8_t sys, uint8_t A_C, uint8_t bt_icon);

void BackgroundTime2(uint8_t A_C, uint8_t bt_icon, float sys_outinv, float battery_A, float sys_w, float ic_temp,
                     float bat_ntc, uint8_t bat_per, uint16_t xunhuan);
void BackgroundTime3(uint8_t week, float batv, float sysv, uint16_t workp, uint16_t ACstate,
                     float bata, float sysw, float batvolume, int xnum, uint16_t batper, uint8_t bt_icon);
void BackgroundTime3_2(uint8_t month, uint8_t day, float battemp, float temp, uint8_t hour, uint8_t minute, uint8_t sec, uint8_t xdl);

void BackgroundTime4(float battery_V, float sys_outinv, uint8_t sys, float battery_A, float sys_w, uint8_t bat_per,
                     uint8_t bt_icon, float ic_temp, float bat_ntc);
void BackgroundTime5(float battery_V, float sys_outinv, uint8_t sys, uint8_t A_C, float battery_A, float sys_w, uint8_t bat_per, float bat_m, uint8_t bt_icon,
                     float ic_temp, float bat_ntc, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t sec, uint8_t week);

//------------------BQ40Z50

void BQScreenLayout1(float bq_battemp, float bq_batv, float bq_bata, float bq_batv4, float bq_batv3, float bq_batv2, float bq_batv1, uint16_t bq_chargetime, uint16_t bq_dischargetime,
                     uint8_t bq_batper, uint16_t bq_batm, uint16_t bq_capacity, uint16_t bq_cyclecount);

void offscreen();
void onscreen();

void lost_Page(); // 非正当获得设备采取措施

void WiFi_Page();            // 等待连接WiFi页面
void ota_AP();               // 配网信息提示
void ota_Page(int a, int b); // 更新页面

#endif
