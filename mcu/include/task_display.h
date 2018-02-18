/*
 * task_display.h
 *
 *  Created on: 18.02.2018
 *      Author: arne
 */

#ifndef MCU_INCLUDE_TASK_DISPLAY_H_
#define MCU_INCLUDE_TASK_DISPLAY_H_

#include "FreeRTOS.h"
#include "task.h"
#include "board.h"

typedef enum { screen_boot, screen_iup, screen_COUNT } screen_id_t;
void display_set_screen(screen_id_t new_screen_id);
void taskDisplay(void *pvParameters);

#endif /* MCU_INCLUDE_TASK_DISPLAY_H_ */
