/*
 *    Copyright (C) 2015  Crystal-Photonics GmbH
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef MCU_INCLUDE_TASK_ADC_H_
#define MCU_INCLUDE_TASK_ADC_H_

#include "board.h"

typedef enum { adsi_temperature, adsi_curr_ext, adsi_supply_sensse, adsi_coin_cell, adsi_ref, adsi_max } adc_sequence_index_t;

void ADC1_IRQHandler(void);

void taskADC(void *pvParameters);
void adc_get_values(uint16_t values[adsi_max]);
bool adc_values_valid();

#endif /* MCU_INCLUDE_TASK_ADC_H_ */
