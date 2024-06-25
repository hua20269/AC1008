#ifndef _ESPDS1302_H
#define _ESPDS1302_H
#include <Arduino.h>
#include <DS1302.h>
void initRTCtime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t sec, uint8_t week);
void printTime(uint16_t *year, uint8_t *month, uint8_t *day, uint8_t *hour, uint8_t *minute, uint8_t *sec, uint8_t *week);


#endif