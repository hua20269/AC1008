#ifndef _RTC_Time_H
#define _RTC_Time_H
#include <Arduino.h>
#include "ESP32Time.h" //内置时钟


void PrintTime(uint16_t *year, uint8_t *month, uint8_t *day, uint8_t *hour, uint8_t *minute, uint8_t *sec, uint8_t *week);


#endif