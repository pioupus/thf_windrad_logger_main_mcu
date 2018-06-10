#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#ifndef QTMCU_H
#define QTMCU_H
#pragma RPC prefix RPC_TRANSMISSION_

//#include "board.h"

//#pragma RPC noanswer mcuSendDataNoAnswer
// void mcuSendDataNoAnswer(uint8_t data);

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

typedef enum { kid_none, kid_key3, kid_key2, kid_key1, kid_onoff, kid_KEYCOUNT } key_id_t;

typedef struct {
    uint32_t githash;
    uint32_t gitDate_unix;
    uint32_t serialnumber;
    uint16_t deviceID;
    uint8_t guid[12];
    uint8_t boardRevision;
    char name[15];
    char version[8];
} device_descriptor_t;

typedef struct {
    int16_t current_l1_avg;
    int16_t current_l2_avg;
    int16_t current_l3_avg;

    uint16_t current_l1_eff;
    uint16_t current_l2_eff;
    uint16_t current_l3_eff;

    int16_t current_l1_max;
    int16_t current_l2_max;
    int16_t current_l3_max;

    int16_t voltage_l12_avg;
    int16_t voltage_l23_avg;
    int16_t voltage_l31_avg;

    uint16_t voltage_l12_eff;
    uint16_t voltage_l23_eff;
    uint16_t voltage_l31_eff;

    uint16_t voltage_l12_max;
    uint16_t voltage_l23_max;
    uint16_t voltage_l31_max;

    uint16_t temperature_l1;
    uint16_t temperature_l2;
    uint16_t temperature_l3;

    int16_t voltage_aux;

    uint16_t frequency_Hz;
    uint16_t power;

    uint16_t external_current_sensor;
    uint16_t supply_voltage;

    int8_t cpu_temperature;
    uint16_t coin_cell_mv;

} power_sensor_data_t;

typedef struct {
    // y = c0 + c1*x + c2*x*x
    int32_t c0_over_65536;
    int32_t c1_over_65536;
    int32_t c2_over_65536;
} calibration_coefficients_t;

#define ADC_EXTERNAL_VALUE_COUNT 11
#define ADC_MCU_VALUE_COUNT 4

typedef struct {
    calibration_coefficients_t channel_pos[11]; // must be the same as ADC_EXTERNAL_VALUE_COUNT, but RPC will not generate code
    calibration_coefficients_t channel_neg[11]; // must be the same as ADC_EXTERNAL_VALUE_COUNT, but RPC will not generate code
    calibration_coefficients_t cpu_channels[4]; // must be the same as ADC_MCU_VALUE_COUNT, but RPC will not generate code
} calibration_t;

void set_calibration_data(calibration_t calibration_data_in[1]);

calibration_t get_calibration_data(void);

void acquire_sample_data(void);
uint8_t is_sample_data_complete(void);
void get_sample_data(int16_t samples_out[128], ext_adc_value_channel_t channel);

power_sensor_data_t get_power_sensor_data(void);

void set_unix_date_time(uint32_t unix_date_time);
uint32_t get_unix_date_time(void);

device_descriptor_t get_device_descriptor(void);

uint8_t get_last_key_state(key_id_t key_id, uint8_t reset_key_memory);

void display_return_to_normal_screen(void);
void display_write_text(uint8_t row, uint8_t text_in[20]);
void display_clear_and_set_custom_screen(void);

void display_set_sysstat_screen(uint8_t count_of_screens, uint8_t screen_index, uint8_t row, uint8_t text_in[20]);

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */
