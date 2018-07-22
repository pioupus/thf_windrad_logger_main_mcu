/*
 * rtc_stdlib.h
 *
 *  Created on: 28.03.2018
 *      Author: arne
 */

#ifndef MCU_INCLUDE_RTC_STDLIB_H_
#define MCU_INCLUDE_RTC_STDLIB_H_

#include <time.h>

extern RTC_HandleTypeDef hrtc;

void rtc_init(void);
void rtc_set_default_date(void);
void rtc_set_date_time(time_t now);
void rtc_goto_standby_with_wakup_after_period(int period_s);
void rtc_decativate_wakeup_timer(void);
uint32_t rtc_get_sub_seconds();
uint32_t rtc_get_sub_second_fraction();
#endif /* MCU_INCLUDE_RTC_STDLIB_H_ */
