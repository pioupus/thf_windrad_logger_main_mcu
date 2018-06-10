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

typedef enum { screen_boot, screen_iu, screen_pbt, screen_sysstat, screen_custom, screen_COUNT } screen_id_t;

void display_set_screen(screen_id_t new_screen_id);
screen_id_t display_get_current_screen(void);

void display_custom_screen_write_text(uint8_t row, uint8_t *text);
void display_custom_screen_clear();

void display_set_sysstat_screen_(uint8_t count_of_screens, uint8_t screen_index, uint8_t row, uint8_t text[20]);
void taskDisplay(void *pvParameters);

#endif /* MCU_INCLUDE_TASK_DISPLAY_H_ */
