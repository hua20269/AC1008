#ifndef _ESP32GFX_H
#define _ESP32GFX_H
#define LGFX_USE_V1
#include <LovyanGFX.hpp>

#define LCD_BL 21
#define LCD_RES 22
#define LCD_DC 19
#define LCD_SDA 23
#define LCD_SCK 18
#define LCD_CS 5


class LGFX : public lgfx::LGFX_Device
{
    lgfx::Panel_ST7789 _panel_instance; //指定使用型号 (ST7789)
    lgfx::Bus_SPI _bus_instance; //SPI设定
    lgfx::Light_PWM _light_instance;//背光设定

public:
    LGFX(void)
    {
        {
            auto cfg = _bus_instance.config(); // バス設定用の構造体を取得します。
            // SPIバスの設定
            cfg.spi_host = VSPI_HOST; // 使用するSPIを選択  ESP32-S2,C3 : SPI2_HOST or SPI3_HOST / ESP32 : VSPI_HOST or HSPI_HOST
            // ※ ESP-IDFバージョンアップに伴い、VSPI_HOST , HSPI_HOSTの記述は非推奨になるため、エラーが出る場合は代わりにSPI2_HOST , SPI3_HOSTを使用してください。
            cfg.spi_mode = 3;          // SPI通信モードを設定 (0 ~ 3)
            cfg.freq_write = 27000000; // 送信時のSPIクロック (最大80MHz, 80MHzを整数で割った値に丸められます)
            cfg.freq_read = 20000000;  // 受信時のSPIクロック
            cfg.spi_3wire = true;      // 受信をMOSIピンで行う場合はtrueを設定
            cfg.use_lock = true;       // トランザクションロックを使用する場合はtrueを設定
            cfg.dma_channel = 0;       // 使用するDMAチャンネルを設定 (0=DMA不使用 / 1=1ch / 2=ch / SPI_DMA_CH_AUTO=自動設定)
            // ※ ESP-IDFバージョンアップに伴い、DMAチャンネルはSPI_DMA_CH_AUTO(自動設定)が推奨になりました。1ch,2chの指定は非推奨になります。
            cfg.pin_sclk = LCD_SCK;                 // SPIのSCLKピン番号を設定
            cfg.pin_mosi = LCD_SDA;                 // SPIのMOSIピン番号を設定
            cfg.pin_miso = -1;                      // SPIのMISOピン番号を設定 (-1 = disable)
            cfg.pin_dc = LCD_DC;                    // SPIのD/Cピン番号を設定  (-1 = disable)
            _bus_instance.config(cfg);              // 設定値をバスに反映します。
            _panel_instance.setBus(&_bus_instance); // バスをパネルにセットします。
        }
        {                                        // 表示パネル制御の設定を行います。
            auto cfg = _panel_instance.config(); // 表示パネル設定用の構造体を取得します。
            cfg.pin_cs = LCD_CS;                 // CSが接続されているピン番号   (-1 = disable)
            cfg.pin_rst = LCD_RES;               // RSTが接続されているピン番号  (-1 = disable)
            cfg.pin_busy = -1;                   // BUSYが接続されているピン番号 (-1 = disable)
            // ※ 以下の設定値はパネル毎に一般的な初期値が設定されていますので、不明な項目はコメントアウトして試してみてください。
            cfg.panel_width = 135;    // 実際に表示可能な幅
            cfg.panel_height = 240;   // 実際に表示可能な高さ
            cfg.offset_x = 52;        // パネルのX方向オフセット量
            cfg.offset_y = 40;        // パネルのY方向オフセット量
            cfg.offset_rotation = 0;  // 回転方向の値のオフセット 0~7 (4~7は上下反転)
            cfg.dummy_read_pixel = 8; // ピクセル読出し前のダミーリードのビット数
            cfg.dummy_read_bits = 1;  // ピクセル以外のデータ読出し前のダミーリードのビット数
            cfg.readable = true;      // データ読出しが可能な場合 trueに設定
            cfg.invert = true;        // パネルの明暗が反転してしまう場合 trueに設定
            cfg.rgb_order = false;    // パネルの赤と青が入れ替わってしまう場合 trueに設定
            cfg.dlen_16bit = false;   // 16bitパラレルやSPIでデータ長を16bit単位で送信するパネルの場合 trueに設定
            cfg.bus_shared = true;    // SDカードとバスを共有している場合 trueに設定(drawJpgFile等でバス制御を行います)
            _panel_instance.config(cfg);
        }
        //*
        {                                        // バックライト制御の設定を行います。（必要なければ削除）
            auto cfg = _light_instance.config(); // バックライト設定用の構造体を取得します。

            cfg.pin_bl = LCD_BL; // バックライトが接続されているピン番号
            cfg.invert = true;   // バックライトの輝度を反転させる場合 true
            cfg.freq = 44100;    // バックライトのPWM周波数
            cfg.pwm_channel = 1; // 使用するPWMのチャンネル番号
            _light_instance.config(cfg);
            _panel_instance.setLight(&_light_instance); // バックライトをパネルにセットします。
        }
        setPanel(&_panel_instance); // 使用するパネルをセットします。
    }
};

void DisplayInit();    // 初始化函数
void RefreshDisplay(); // 刷新画面
//void Background_1();   // 背景主题1
//void ShowParameter_1(float batv, float bata, uint16_t ACstate, float temp, float sysv, float sysw, uint16_t workp, float batvolume, int xnum, uint16_t batper, float battemp);

#endif