#ifndef _OTA_H
#define _OTA_H

#define wifi_ssid "2024"            // WIFI名称，区分大小写，不要写错
#define wifi_password "12345678" // WIFI密码

extern int a, b;

// 固件链接，在巴法云控制台复制、粘贴到这里即可
// #define upUrl = "http://bin.bemfa.com/b/3BcYWMzNDRjODg3OGFiNWQwMWNlNDFkMjNiMmRjZjRmNzQ=AC1008.bin";

void updateBin(); // wifiManager配网      httpUpdate OTA更新



#endif