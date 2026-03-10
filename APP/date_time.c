/*
 * date_time.c
 *
 *  Created on: 2023年6月24日
 *      Author: wx030
 */

/* Includes ------------------------------------------------------*/

#include "HAL.h"
#include <time.h>

/********************************************************************
 * name          : TimeOffset
 * description   : 根据设置时间校准
 * Input         : none
 * Output        : none
 * Return        : none
 ********************************************************************/
void Time_offset(struct tm* stDateTime)
{
    time_t ctime = mktime(stDateTime);
    struct tm stTime;
    uint16_t year, mon, day, hour, min, sec;

    RTC_GetTime(&year, &mon, &day, &hour, &min, &sec);
    stTime.tm_year = year;
    stTime.tm_mday = day;
    stTime.tm_mon = mon;
    stTime.tm_hour = hour;
    stTime.tm_min = min;
    stTime.tm_sec = sec;
    // RTC_GetTime(&stTime.tm_year, &stTime.tm_mon, &stTime.tm_mday, &stTime.tm_hour, &stTime.tm_min, &stTime.tm_sec);
    time_t TimeStamp = mktime(&stTime);
    baseParameters.timeoffset = ctime - TimeStamp;
}
/********************************************************************
 * name          : TimeOffset
 * description   : 根据校准获得时间
 * Input         : none
 * Output        : none
 * Return        : none
 ********************************************************************/
struct tm gettimebyoffset(void)
{
    time_t ctimes = 0;
    struct tm stTime;
    uint16_t year, mon, day, hour, min, sec;

    RTC_GetTime(&year, &mon, &day, &hour, &min, &sec);
    stTime.tm_year = year;
    stTime.tm_mday = day;
    stTime.tm_mon = mon;
    stTime.tm_hour = hour;
    stTime.tm_min = min;
    stTime.tm_sec = sec;
    ctimes = mktime(&stTime) + baseParameters.timeoffset;
    struct tm* pstTime = localtime(&ctimes);
    return *pstTime;
}

void datetime_init(void)
{
    Time_offset(&baseParameters.sttime);
}
