/*
 * rtc_stdlib.c
 *
 *  Created on: 28.03.2018
 *      Author: arne
 */

#include "board.h"
#include "rtc_stdlib.h"
#include <sys/time.h>

RTC_HandleTypeDef hrtc;

void rtc_init(void) {
    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv = 127;
    hrtc.Init.SynchPrediv = 255;
    hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    HAL_RTC_Init(&hrtc);
}

void rtc_set_default_date(void) {
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};

    /**Initialize RTC and set the Time and Date
    */

    sTime.Hours = 0x0;
    sTime.Minutes = 0x0;
    sTime.Seconds = 0x0;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;
    HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);

    sDate.WeekDay = RTC_WEEKDAY_MONDAY;
    sDate.Month = RTC_MONTH_JANUARY;
    sDate.Date = 0x1;
    sDate.Year = 0x0;

    HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
}

void rtc_set_date_time(time_t now) {

    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};

    struct tm time_tm = {0};

    time_tm = *(localtime(&now));

    sTime.Hours = (uint8_t)time_tm.tm_hour;
    sTime.Minutes = (uint8_t)time_tm.tm_min;
    sTime.Seconds = (uint8_t)time_tm.tm_sec;
    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) {
        // _Error_Handler(__FILE__, __LINE__);
    }

    if (time_tm.tm_wday == 0) {
        time_tm.tm_wday = 7;
    } // the chip goes mon tue wed thu fri sat sun
    sDate.WeekDay = (uint8_t)time_tm.tm_wday;
    sDate.Month = (uint8_t)time_tm.tm_mon + 1; // momth 1-12. This is why date math is frustrating.
    sDate.Date = (uint8_t)time_tm.tm_mday;
    sDate.Year = (uint16_t)(time_tm.tm_year + 1900 - 2000); // time.h is years since 1900, chip is years since 2000

    /*
     * update the RTC
     */
    if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) {
        // _Error_Handler(__FILE__, __LINE__);
    }

    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, 0x32F2); // lock it in with the backup registers
}

#if 0
void example_print_date_time(){
    time_t t = time(0);
    struct tm printTm = *(localtime(&t));
    char buffer[20] = {0};
    strftime(buffer, 20, "%m/%d/%y %H:%M:%S", &printTm);
}
#endif

static time_t rtc_get_date_time() {
    RTC_DateTypeDef rtcDate = {0};
    RTC_TimeTypeDef rtcTime = {0};

    HAL_RTC_GetTime(&hrtc, &rtcTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &rtcDate, RTC_FORMAT_BIN);

    uint8_t hh = rtcTime.Hours;
    uint8_t mm = rtcTime.Minutes;
    uint8_t ss = rtcTime.Seconds;
    uint8_t d = rtcDate.Date;
    uint8_t m = rtcDate.Month;
    uint16_t y = rtcDate.Year;
    uint16_t yr = (uint16_t)(y + 2000 - 1900);

    struct tm tim = {0};
    tim.tm_year = yr;
    tim.tm_mon = m - 1;
    tim.tm_mday = d;
    tim.tm_hour = hh;
    tim.tm_min = mm;
    tim.tm_sec = ss;

    return mktime(&tim);
}

int _gettimeofday(struct timeval *tv, void *tzvp) {
    tv->tv_sec = rtc_get_date_time(); // convert to seconds
    tv->tv_usec = 0;                  // get remaining microseconds
    return 0;                         // return non-zero for error
}
