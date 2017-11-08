/*
 * rpc_func_arduino.c
 *
 *  Created on: 15.11.2015
 *      Author: arne
 */

#include "../../../include/rpc_transmission/server/app/qt2mcu.h"
#include "board.h"
#include "vc.h"

device_descriptor_t get_device_descriptor(void) {
    device_descriptor_t retval = {.name = "THF_LOGGER", .version = "0.1"};
    retval.githash = GITHASH;
    retval.gitDate_unix = GITUNIX;
    HAL_GetUID((uint32_t *)&retval.guid);
    return retval;
}

adc_values_t get_adc_values() {
    static uint32_t running_val_ = 0;
    adc_values_t retval = {0};
    retval.temperature_c = 45;
    retval.vcc_mv = 5500;
    retval.running_number = running_val_++;
    return retval;
}

void mcuSendDataNoAnswer(uint8_t data) {
}

uint16_t mcuSetLEDStatus(rpcLEDStatus_t ledStatus) {

    static uint16_t returnvalue = 0;
    returnvalue++;
    switch (ledStatus) {
        case rpcLEDStatus_off:
            CLEAR_LED_RED();
            break;
        case rpcLEDStatus_on:
            SET_LED_RED();
            break;
        case rpcLEDStatus_none:
            break;
    }

    return returnvalue;
}

void test_function_param(uint8_t param8, uint32_t param32) {
}

void test_function_struct(test_struct_t param8_inout[1], test_struct_t param32_inout[1]) {
    param32_inout->running_number++;
    param8_inout++;
}
