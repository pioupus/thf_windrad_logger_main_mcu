#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#pragma RPC prefix RPC_TRANSMISSION_

typedef enum { rpcLEDStatus_none, rpcLEDStatus_off, rpcLEDStatus_on } rpcLEDStatus_t;

//#pragma RPC noanswer mcuSendDataNoAnswer
// void mcuSendDataNoAnswer(uint8_t data);

// uint16_t mcuSetLEDStatus(rpcLEDStatus_t ledStatus);

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

void get_sample_data(int16_t samples_out[128], adc_sample_channel_t channel);

void set_unix_date_time(uint32_t unix_date_time);
uint32_t get_unix_date_time(void);

device_descriptor_t get_device_descriptor(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
