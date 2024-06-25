#include <espds1302.h>
DS1302 rtc(12, 15, 13); // 对应rst dat clk

/**
 * 初始化ds1302 并设置年月日时分秒星期
 */
void initRTCtime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t sec, uint8_t week)
{
    rtc.writeProtect(false);
    rtc.halt(false);
    Time t(year, month, day, hour, minute, sec, week);
    rtc.time(t);
}

/**
 * 输出1302的时间数据
 */

void printTime(uint16_t *year, uint8_t *month, uint8_t *day, uint8_t *hour, uint8_t *minute, uint8_t *sec, uint8_t *week)
{
    Time tim = rtc.time(); // 从1302获取时间数据
    *year = tim.yr;
    *month = tim.mon;
    *day = tim.date;
    *hour = tim.hr;
    *minute = tim.min;
    *sec = tim.sec;
    *week = tim.day;
}
