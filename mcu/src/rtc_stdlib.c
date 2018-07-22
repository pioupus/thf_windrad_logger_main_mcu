/*
 * rtc_stdlib.c
 *
 *  Created on: 28.03.2018
 *      Author: arne
 */

#include "board.h"
#include "rtc_stdlib.h"
#include <sys/time.h>
#include "main.h" // for ISR_PRIORITY_RTC

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

uint32_t rtc_get_sub_seconds() {
    RTC_TimeTypeDef rtcTime = {0};
    HAL_RTC_GetTime(&hrtc, &rtcTime, RTC_FORMAT_BIN);
    return rtcTime.SubSeconds;
}

uint32_t rtc_get_sub_second_fraction() {
    RTC_TimeTypeDef rtcTime = {0};
    HAL_RTC_GetTime(&hrtc, &rtcTime, RTC_FORMAT_BIN);
    return rtcTime.SecondFraction;
}

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

void rtc_goto_standby_with_wakup_after_period(int period_s) {

    /*## Configure the Wake up timer ###########################################*/
    /*  RTC Wake-up Interrupt Generation:
        Wake-up Time Base = (RTC_WAKEUPCLOCK_RTCCLK_DIV /(LSI))
        Wake-up Time = Wake-up Time Base * WakeUpCounter
                    = (RTC_WAKEUPCLOCK_RTCCLK_DIV /(LSI)) * WakeUpCounter
        ==> WakeUpCounter = Wake-up Time / Wake-up Time Base

        To configure the wake up timer to 20s the WakeUpCounter is set to 0xA017:
          RTC_WAKEUPCLOCK_RTCCLK_DIV = RTCCLK_Div16 = 16
          Wake-up Time Base = 16 /(~32.768KHz) = ~0,488 ms
          Wake-up Time = ~20s = 0,488ms  * WakeUpCounter
          ==> WakeUpCounter = ~20s/0,488ms = 40983 = 0xA017 */
    /* Disable Wake-up timer */
    if (HAL_RTCEx_DeactivateWakeUpTimer(&hrtc) != HAL_OK) {
        /* Initialization Error */
        //  Error_Handler();
    }

    /*#### Clear all related wake-up flags ######################################*/
    /* Clear PWR wake up Flag */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

    /* Clear RTC Wake Up timer Flag */
    __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc, RTC_FLAG_WUTF);

    /*#### Setting the Wake up time ############################################*/
    //    HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 0xA017, RTC_WAKEUPCLOCK_RTCCLK_DIV16);

    HAL_NVIC_SetPriority(RTC_WKUP_IRQn, ISR_PRIORITY_RTC, 0);
    HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, period_s - 1, RTC_WAKEUPCLOCK_CK_SPRE_16BITS);

    /*#### Enter the Standby mode ##############################################*/
    /* Request to enter STANDBY mode  */
    // PWR_WakeUpPinCmd(PWR_WakeUpPin_2, ENABLE);
    // PWR_WakeUpPinCmd(PWR_WakeUpPin_1, ENABLE);

    HAL_PWR_EnterSTANDBYMode();
}

void rtc_decativate_wakeup_timer(void) {
    HAL_RTCEx_DeactivateWakeUpTimer(
        &hrtc); // System reset, as well as low-power modes (Sleep, Stop and Standby) have no influence on the wakeup timer.
}

void RTC_WKUP_IRQHandler(void) {
    HAL_RTCEx_WakeUpTimerIRQHandler(&hrtc);
}
