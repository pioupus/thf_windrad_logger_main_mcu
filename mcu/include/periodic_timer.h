/*
 * periodic_timer.h
 *
 *  Created on: 28.01.2018
 *      Author: arne
 */

#ifndef MCU_INCLUDE_PERIODIC_TIMER_H_
#define MCU_INCLUDE_PERIODIC_TIMER_H_

#include <stdint.h>

#define TIMEBASE_SUBTICK_DURATION_us 10

uint32_t timebase_get_subticks();

void timebase_delay_by_subticks(uint32_t delay_by_subtick);

#endif /* MCU_INCLUDE_PERIODIC_TIMER_H_ */
