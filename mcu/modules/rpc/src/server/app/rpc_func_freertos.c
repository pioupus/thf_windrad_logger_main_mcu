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

device_descriptor_t get_device_descriptor(void) {
    device_descriptor_t retval = {.name = "THF_LOGGER", .version = "0.1"};
    retval.githash = GITHASH;
    retval.gitDate_unix = GITUNIX;
    HAL_GetUID((uint32_t *)&retval.guid);
    return retval;
}

void get_sample_data(int16_t samples_out[128], adc_sample_channel_t channel) {
    extadc_get_sample_data(samples_out, channel);
}

void set_unix_date_time(uint32_t unix_date_time) {
    rtc_set_date_time(unix_date_time);
}

uint32_t get_unix_date_time(void) {
    return time(NULL);
}
