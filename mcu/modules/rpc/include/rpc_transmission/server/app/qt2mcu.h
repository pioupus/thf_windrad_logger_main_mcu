#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#ifndef QTMCU_H
#define QTMCU_H
#pragma RPC prefix RPC_TRANSMISSION_

#include "board.h"

//#pragma RPC noanswer mcuSendDataNoAnswer
// void mcuSendDataNoAnswer(uint8_t data);

typedef enum {
    adc_sample_channel_curr_l1,
    adc_sample_channel_curr_l2,
    adc_sample_channel_curr_l3,

    adc_sample_channel_vref,

    adc_sample_channel_volt_l12,
    adc_sample_channel_volt_l23,
    adc_sample_channel_volt_l31,

    adc_sample_channel_aux_volt,

    adc_sample_channel_temp_l1,
    adc_sample_channel_temp_l2,
    adc_sample_channel_temp_l3
} adc_sample_channel_t;

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

    int16_t voltage_l12_avg;
    int16_t voltage_l23_avg;
    int16_t voltage_l31_avg;

    uint16_t voltage_l12_eff;
    uint16_t voltage_l23_eff;
    uint16_t voltage_l31_eff;

    uint16_t temperature_l1;
    uint16_t temperature_l2;
    uint16_t temperature_l3;

    int16_t voltage_aux;

} power_sensor_data_t;

typedef struct {
    // y = c0 + c1*x + c2*x*x
    int32_t c0_over_65536;
    int32_t c1_over_65536;
    int32_t c2_over_65536;
} calibration_coefficients_t;

typedef struct {
    calibration_coefficients_t channel_pos[ext_adc_value_COUNT];
    calibration_coefficients_t channel_neg[ext_adc_value_COUNT];
} calibration_t;

void set_calibration_data(calibration_coefficients_t calibration_data_pos_in[11], calibration_coefficients_t calibration_data_neg_in[11]);

void get_sample_data(int16_t samples_out[128], adc_sample_channel_t channel);
power_sensor_data_t get_power_sensor_data(void);

void set_unix_date_time(uint32_t unix_date_time);
uint32_t get_unix_date_time(void);

device_descriptor_t get_device_descriptor(void);

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */
