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

#ifndef MCU_INCLUDE_TASK_EXTERNAL_ADC_H_
#define MCU_INCLUDE_TASK_EXTERNAL_ADC_H_

#include "board.h"

#define ADC_RECORDS_TO_BUFFER 128

typedef enum {
    ext_adc_value_curr_l1,
    ext_adc_value_curr_l2,
    ext_adc_value_curr_l3,

    ext_adc_value_vref,

    ext_adc_value_volt_l12,
    ext_adc_value_volt_l23,
    ext_adc_value_volt_l31,

    ext_adc_value_aux_volt,

    ext_adc_value_temp_l1,
    ext_adc_value_temp_l2,
    ext_adc_value_temp_l3,
    ext_adc_value_COUNT
} ext_adc_value_channel_t;

typedef struct { int16_t l1, l2, l3; } ext_adc_temperatures_t;

typedef enum { i_l1, i_l2, i_l3, i_COUNT } current_index_t;
typedef enum { u_l12, u_l23, u_l31, u_aux, u_COUNT } voltage_index_t;

void extadc_get_voltages_avg(int16_t avg[u_COUNT]);
void extadc_get_voltages_effective(uint16_t eff[3]);

void extadc_get_currents_avg(int16_t avg[i_COUNT]);
void extadc_get_currents_effective(uint16_t eff[i_COUNT]);

void extadc_get_sample_data(int16_t samples[ADC_RECORDS_TO_BUFFER], const ext_adc_value_channel_t channel);

void taskExternalADC(void *pvParameters);
#endif /* MCU_INCLUDE_TASK_ADC_H_ */
