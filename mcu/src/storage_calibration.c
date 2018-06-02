/*
 * storage_calibration.c
 *
 *  Created on: 22.04.2018
 *      Author: arne
 */

#include "storage_calibration.h"
#include "eeprom_storage.h"
#include <assert.h>
#include <string.h>

#define STORAGE_EEPROM_CALIBRATION_START 0
#define STORAGE_EEPROM_CALIBRATION_LENGTH_IN_HALF_WORD 0xA0

typedef struct {
    calibration_t calibration_data;
    uint16_t crc16;
} calibration_storage_t;

static calibration_storage_t calibration_storage;

static uint16_t calc_crc(const uint8_t *data_p, uint16_t length) {
    uint8_t x;
    uint16_t crc = 0x1021;

    while (length--) {
        x = crc >> 8 ^ *data_p++;
        x ^= x >> 4;
        crc = (crc << 8) ^ ((unsigned short)(x << 12)) ^ ((unsigned short)(x << 5)) ^ ((unsigned short)x);
    }
    return crc;
}

static void calib_set_crc() {
    uint16_t buffer[STORAGE_EEPROM_CALIBRATION_LENGTH_IN_HALF_WORD] = {0};
    calibration_storage.crc16 = 0;
    memcpy(buffer, &calibration_storage, sizeof(calibration_storage_t));
    uint16_t calced_crc = calc_crc((uint8_t *)buffer, sizeof(calibration_storage_t));
    calibration_storage.crc16 = calced_crc;
}

static bool calib_check_crc() {
    uint16_t taken_crc = calibration_storage.crc16;
    calibration_storage.crc16 = 0;
    uint16_t buffer[STORAGE_EEPROM_CALIBRATION_LENGTH_IN_HALF_WORD] = {0};
    memcpy(buffer, &calibration_storage, sizeof(calibration_storage_t));
    uint16_t calced_crc = calc_crc((uint8_t *)buffer, sizeof(calibration_storage_t));
    calibration_storage.crc16 = taken_crc;
    return calced_crc == taken_crc;
}

static HAL_StatusTypeDef calib_load() {
    assert(sizeof(calibration_storage_t) < (STORAGE_EEPROM_CALIBRATION_LENGTH_IN_HALF_WORD * 2));
    assert(sizeof(calibration_storage.calibration_data.channel_neg) == sizeof(calibration_coefficients_t[ext_adc_value_COUNT]));
    uint16_t buffer[STORAGE_EEPROM_CALIBRATION_LENGTH_IN_HALF_WORD] = {0};
    HAL_StatusTypeDef result = eeprom_read_buffer(STORAGE_EEPROM_CALIBRATION_START, buffer, (STORAGE_EEPROM_CALIBRATION_LENGTH_IN_HALF_WORD));
    memcpy(&calibration_storage, buffer, sizeof(calibration_storage_t));
    return result;
}

static void calib_load_default() {
    calibration_t calibration_data = {.channel_neg = {{0}}, .channel_pos = {{0}}};
    for (int i = 0; i < ext_adc_value_COUNT; i++) {
        calibration_data.channel_neg[i].c1_over_65536 = 65536;
        calibration_data.channel_pos[i].c1_over_65536 = 65536;
    }

    calib_store(&calibration_data);
}

void calib_init() {
    HAL_StatusTypeDef load_result = calib_load();

    if (calib_check_crc() && (load_result == HAL_OK)) {
        // super

    } else {
        // not super
        calib_load_default();
    }
}

void calib_store(const calibration_t *calibration_data) {
    memcpy(&calibration_storage.calibration_data, calibration_data, sizeof(calibration_t));
    calib_set_crc();
    uint16_t buffer[STORAGE_EEPROM_CALIBRATION_LENGTH_IN_HALF_WORD] = {0};
    memcpy(buffer, &calibration_storage, sizeof(calibration_storage_t));
    eeprom_write_buffer(STORAGE_EEPROM_CALIBRATION_START, buffer, STORAGE_EEPROM_CALIBRATION_LENGTH_IN_HALF_WORD);
}

calibration_t *calib_get() {
    return &calibration_storage.calibration_data;
}
