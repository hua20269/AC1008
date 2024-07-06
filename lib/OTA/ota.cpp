// #include <Arduino.h>
// #include <WiFiManager.h>
// #include <esp_AutoWifi.h>
#include <HTTPUpdate.h>
#include <ota.h>
// #include <esp_task_wdt.h>
// int cur, total;
int a, b;
// 固件链接，在巴法云控制台复制、粘贴到这里即可
String upUrl = "http://bin.bemfa.com/b/3BcYWMzNDRjODg3OGFiNWQwMWNlNDFkMjNiMmRjZjRmNzQ=AC1008.bin";

// 当升级开始时，打印日志
void update_started()
{
    Serial.println("CALLBACK:  HTTP update process started");
}

// 当升级中，打印日志
void update_progress(int cur, int total)
{

    a = cur;
    b = total;
    Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes... %.1f %%\n", cur, total, cur * 100.0 / total);
    // Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes[%.1f%%]...\n", cur, total, cur * 100.0 / total);
}

// 当升级结束时，打印日志
void update_finished()
{
    Serial.println("CALLBACK:  HTTP update process finished");
}

// 当升级失败时，打印日志
void update_error(int err)
{
    Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
}

/**
 * 固件升级函数
 * 在需要升级的地方，加上这个函数即可，例如setup中加的updateBin();
 * 原理：通过http请求获取远程固件，实现升级
 */
void updateBin()
{
    // WiFiManager wifiManager; // 初始化一个自动配网对象
    // wifiManager.autoConnect("AP_Page"); // wifiManager库 配网

    // AutoWifi autowifi;   // 初始化一个自动配网对象
    // autowifi.webBegin(); // esp_AutoWifi库 配网

    WiFi.begin(wifi_ssid, wifi_password); // 连接wifi
    vTaskDelay(1000);                     // 给WiFi启动缓冲
    while (WiFi.status() != WL_CONNECTED)
    { // 等待连接wifi
        delay(300);
        Serial.print(".");
    }
    vTaskDelay(1000);
    WiFiClient UpdateClient; // 获取远程更新
    vTaskDelay(1000);
    httpUpdate.onStart(update_started);     // 当升级开始时
    httpUpdate.onEnd(update_finished);      // 当升级结束时
    httpUpdate.onProgress(update_progress); // 当升级中
    httpUpdate.onError(update_error);       // 当升级失败时

    t_httpUpdate_return ret = httpUpdate.update(UpdateClient, upUrl);
    switch (ret)
    {
    case HTTP_UPDATE_FAILED: // 当升级失败
        Serial.println("[update] Update failed.");
        break;
    case HTTP_UPDATE_NO_UPDATES: // 当无升级
        Serial.println("[update] Update no Update.");
        break;
    case HTTP_UPDATE_OK: // 当升级成功
        Serial.println("[update] Update ok.");
        break;
    }
}

// #include "nvs_flash.h"

// #include "esp_wifi.h"
// // void sys_ota()
// // {

// //     //  默认会存在 NVS 里，不会因为掉电而消失，您也可以通过 esp_wifi_set_storage() 设置，此时分为两种情况： 如果想要实现掉电保存 Wi-Fi SSID 和 PSAAWORD，可通过调用 esp_wifi_set_storage(WIFI_STORAGE_FLASH) 将 Wi-Fi 信息存储在 flash 内。 如果想要实现掉电不保存 Wi-Fi SSID 和 PASSWORD 的操作，可通过调用 esp_wifi_set_storage(WIFI_STORAGE_RAM) 将 Wi-Fi 信息存储在 RAM 内。
// //     WiFi.begin(wifi_name, wifi_password); // 连接wifi
// //     // esp_wifi_set_storage(WIFI_STORAGE_FLASH);
// //     while (WiFi.status() != WL_CONNECTED)
// //     { // 等待连接wifi
// //         delay(300);
// //         Serial.print(".");
// //     }

// //     updateBin(); // 开始升级
// // }

// // 保存wifi配置参数结构体变量wifi_config到nvs------------------------------------------------
// static void saveWifiConfig(wifi_config_t *wifi_config)
// {
//     nvs_handle my_handle;
//     nvs_open("WIFI_CONFIG", NVS_READWRITE, &my_handle); // 打开

//     ESP_ERROR_CHECK(nvs_set_blob(my_handle, "wifi_config", wifi_config, sizeof(wifi_config_t)));

//     ESP_ERROR_CHECK(nvs_commit(my_handle)); // 提交
//     nvs_close(my_handle);                   // 退出
// }

// // 从nvs中读取wifi配置到给定的sta_config结构体变量
// static esp_err_t readWifiConfig(wifi_config_t *sta_config)
// {
//     nvs_handle my_handle;
//     nvs_open("WIFI_CONFIG", NVS_READWRITE, &my_handle); // 打开

//     // sta_config = {
//     //     .sta = {
//     //         .ssid = "ssid",
//     //         .password = "password",
//     //         .bssid_set = false}}

//     uint32_t len = sizeof(wifi_config_t);
//     esp_err_t err = nvs_get_blob(my_handle, "wifi_config", sta_config, &len);

//     nvs_close(my_handle); // 退出

//     return err;
// }

// // esp_wifi_get_confi;
