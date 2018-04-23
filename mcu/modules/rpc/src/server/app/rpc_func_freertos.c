/*
 * rpc_func_arduino.c
 *
 *  Created on: 15.11.2015
 *      Author: arne
 */

#include "../../../include/rpc_transmission/server/app/qt2mcu.h"
#include "board.h"
#include "vc.h"
#include "task_external_adc.h"
#include "rtc_stdlib.h"
#include <assert.h>

device_descriptor_t get_device_descriptor(void) {
    device_descriptor_t retval = {.name = "THF_LOGGER", .version = "0.1"};
    retval.githash = GITHASH;
    retval.gitDate_unix = GITUNIX;
    HAL_GetUID((uint32_t *)&retval.guid);
    return retval;
}

void get_sample_data(int16_t samples_out[128], adc_sample_channel_t channel) {
    extadc_get_sample_data(samples_out, channel);
#if 0
    static int16_t val = 0;
    for (int i = 0; i < 128; i++) {
        samples_out[i] = val;
    }
    val++;
#endif
}

void set_calibration_data(calibration_coefficients_t calibration_data_pos_in[], calibration_coefficients_t calibration_data_neg_in[]) {
    assert(sizeof(calibration_data_pos_in) == sizeof(calibration_coefficients_t[ext_adc_value_COUNT]));
    assert(sizeof(calibration_data_neg_in) == sizeof(calibration_coefficients_t[ext_adc_value_COUNT]));
}

power_sensor_data_t get_power_sensor_data(void) {

    power_sensor_data_t result = {0};

    int16_t i4[u_COUNT] = {0};
    uint16_t u3[3] = {0};

    extadc_get_voltages_avg(i4);
    result.voltage_l12_avg = i4[u_l12];
    result.voltage_l23_avg = i4[u_l23];
    result.voltage_l31_avg = i4[u_l31];
    result.voltage_aux = i4[u_aux];

    extadc_get_voltages_effective(u3);
    result.voltage_l12_eff = u3[0];
    result.voltage_l23_eff = u3[1];
    result.voltage_l31_eff = u3[2];

    extadc_get_currents_avg(i4);
    result.current_l1_avg = i4[0];
    result.current_l2_avg = i4[1];
    result.current_l3_avg = i4[2];

    extadc_get_currents_effective(u3);
    result.current_l1_eff = u3[0];
    result.current_l2_eff = u3[1];
    result.current_l3_eff = u3[2];

    extadc_get_temperature_avg(u3);
    result.temperature_l1 = u3[0];
    result.temperature_l2 = u3[1];
    result.temperature_l3 = u3[2];

    return result;
}

void set_unix_date_time(uint32_t unix_date_time) {
    rtc_set_date_time(unix_date_time);
}

uint32_t get_unix_date_time(void) {
    return time(NULL);
}
