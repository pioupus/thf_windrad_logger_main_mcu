#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#pragma RPC prefix RPC_TRANSMISSION_

typedef enum { rpcLEDStatus_none, rpcLEDStatus_off, rpcLEDStatus_on } rpcLEDStatus_t;

#pragma RPC noanswer mcuSendDataNoAnswer
void mcuSendDataNoAnswer(uint8_t data);

uint16_t mcuSetLEDStatus(rpcLEDStatus_t ledStatus);

typedef struct {
    uint32_t temperature_c;
    uint32_t vcc_mv;
    uint32_t running_number;
} adc_values_t;

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

adc_values_t get_adc_values();

device_descriptor_t get_device_descriptor(void);

typedef struct {
    uint32_t running_number;
    char name[15];
    char version[8];
} test_struct_t;

void test_function_param(uint8_t param8, uint32_t param32);

void test_function_struct(test_struct_t param8_inout[1], test_struct_t param32_inout[1]);

#ifdef __cplusplus
}
#endif /* __cplusplus */
