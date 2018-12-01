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
#include "storage_calibration.h"
#include "task_key.h"
#include "task_display.h"
#include "task_adc.h"
#include <assert.h>
#include <string.h>

device_descriptor_t get_device_descriptor(void) {
    device_descriptor_t retval = {.name = "THF_LOGGER", .version = "0.1"};
    retval.githash = GITHASH;
    retval.gitDate_unix = GITUNIX;
    HAL_GetUID((uint32_t *)&retval.guid);
    return retval;
}

void acquire_sample_data(void) {
    extadc_start_acquire_sample_data();
}

uint8_t is_sample_data_complete(void) {
    return extadc_is_sample_data_complete();
}

void reset_sample_data_readpointer() {
    extadc_reset_sample_data_readpointer();
}

uint16_t get_sample_chunk_count() {
    return extadc_sample_chunk_count();
}

sampe_data_t get_sample_data(ext_adc_value_channel_t channel) {
    sampe_data_t result = {0};
    sub_unix_time_t sub_unix_time = rtc_get_sub_unix_time();
    result.unix_time = sub_unix_time.unix_time;
    result.sub_seconds = sub_unix_time.subseconds;
    extadc_get_sample_data(result.sample, channel);
    return result;
}

void set_calibration_data(calibration_t calibration_data_in[1]) {
    assert(sizeof(calibration_data_in->channel_neg) == sizeof(calibration_coefficients_t[ext_adc_value_COUNT]));
    calibration_t calibration_data = {.channel_neg = {{0}}, .channel_pos = {{0}}};
    memcpy(&calibration_data, calibration_data_in, sizeof(calibration_t));
    calib_store(&calibration_data);
}

calibration_t get_calibration_data(void) {
    calibration_t *calibration_data = calib_get(&calibration_data);
    return *calibration_data;
}

power_sensor_data_t get_power_sensor_data(void) {

    power_sensor_data_t result = {0};

    sub_unix_time_t sub_unix_time = rtc_get_sub_unix_time();
    result.unix_time = sub_unix_time.unix_time;
    result.sub_seconds = sub_unix_time.subseconds;

    int16_t i4[u_COUNT] = {0};
    uint16_t u4[4] = {0};
    uint16_t u3[3] = {0};

    extadc_get_voltages_avg(i4);
    result.voltage_l21_avg = i4[u_l21];
    result.voltage_l32_avg = i4[u_l32];
    result.voltage_l13_avg = i4[u_l13];
    result.voltage_aux = i4[u_aux];

    extadc_get_voltages_effective(u3);
    result.voltage_l21_eff = u3[0];
    result.voltage_l32_eff = u3[1];
    result.voltage_l13_eff = u3[2];

    extadc_get_voltages_max(u4);
    result.voltage_l21_max = u4[0];
    result.voltage_l32_max = u4[1];
    result.voltage_l13_max = u4[2];

    extadc_get_currents_avg(i4);
    result.current_l1_avg = i4[0];
    result.current_l2_avg = i4[1];
    result.current_l3_avg = i4[2];

    extadc_get_currents_max(u3);
    result.current_l1_max = u3[0];
    result.current_l2_max = u3[1];
    result.current_l3_max = u3[2];

    extadc_get_currents_effective(u3);
    result.current_l1_eff = u3[0];
    result.current_l2_eff = u3[1];
    result.current_l3_eff = u3[2];

    extadc_get_temperature_avg(u3);
    result.temperature_l1 = u3[0];
    result.temperature_l2 = u3[1];
    result.temperature_l3 = u3[2];

    result.power = extadc_get_power();
    result.frequency_Hz = extadc_get_frequency();

    uint16_t values[adsi_max] = {0};
    adc_get_values(values);

    result.external_current_sensor = values[adsi_curr_ext];
    result.supply_voltage = values[adsi_supply_sensse];
    result.cpu_temperature = values[adsi_temperature];
    result.coin_cell_mv = values[adsi_coin_cell];

    return result;
}

void set_unix_date_time(uint32_t unix_date_time) {
    rtc_set_date_time(unix_date_time);
}

uint32_t get_unix_date_time(void) {
    return time(NULL);
}

static key_event_t keystates[kid_KEYCOUNT];

static void key_handler(key_event_t event, key_id_t id) {
    keystates[id] = event;
}

void init_rpc_key_handler(void) {
    keyRegisterHandle(&key_handler);
}

uint8_t get_last_key_state(key_id_t key_id, uint8_t reset_key_memory) {
    uint8_t result = keystates[key_id];
    if (reset_key_memory) {
        memset(keystates, 0, sizeof(keystates));
    }
    return result;
}

static screen_id_t last_screen_id;

void display_return_to_normal_screen() {
    screen_id_t current_screen_id = display_get_current_screen();
    if (current_screen_id == screen_custom) {
        display_set_screen(last_screen_id);
    }
}

void display_write_text(uint8_t row, uint8_t text_in[20]) {
    display_custom_screen_write_text(row, text_in);
}

void display_clear_and_set_custom_screen(void) {
    display_custom_screen_clear();
    last_screen_id = display_get_current_screen();
    display_set_screen(screen_custom);
}

void display_set_sysstat_screen(uint8_t count_of_screens, uint8_t screen_index, uint8_t row, uint8_t text_in[20]) {
    display_set_sysstat_screen_(count_of_screens, screen_index, row, text_in);
}

void gpio_set_state(uint8_t gpio_index, uint8_t state) {
    switch (gpio_index) {
        case 0:
            if (state) {
                SET_GPIO0();
            } else {
                CLEAR_GPIO0();
            }
            break;

        case 1:
            if (state) {
                SET_GPIO1();
            } else {
                CLEAR_GPIO1();
            }
            break;

        case 2:
            if (state) {
                SET_GPIO2();
            } else {
                CLEAR_GPIO2();
            }
            break;
        case 3:
            if (state) {
                SET_GPIO3();
            } else {
                CLEAR_GPIO3();
            }
            break;
        case 4:
            if (state) {
                SET_GPIO4();
            } else {
                CLEAR_GPIO4();
            }
            break;
        case 5:
            if (state) {
                SET_WS2812();
            } else {
                CLEAR_WS2812();
            }
            break;
    }
}
