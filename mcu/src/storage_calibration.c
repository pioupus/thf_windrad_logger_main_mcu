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
#define STORAGE_EEPROM_CALIBRATION_LENGTH_IN_HALF_WORD 0x80

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

void calib_init() {
    assert(sizeof(calibration_storage_t) < STORAGE_EEPROM_CALIBRATION_LENGTH_IN_HALF_WORD * 2);
    uint16_t buffer[STORAGE_EEPROM_CALIBRATION_LENGTH_IN_HALF_WORD] = {0};
    eeprom_read_buffer(STORAGE_EEPROM_CALIBRATION_START, buffer, STORAGE_EEPROM_CALIBRATION_LENGTH);
    memcpy(&calibration_storage, buffer, sizeof(calibration_storage_t));
    uint16_t taken_crc = calibration_storage.crc16;
    calibration_storage.crc16 = 0;
    memcpy(buffer, &calibration_storage, sizeof(calibration_storage_t));
    uint16_t calced_crc = calc_crc((uint8_t *)buffer, sizeof(calibration_storage_t));
    if (calced_crc == taken_crc) {
        // super

        calibration_storage.crc16 = taken_crc;

    } else {
        // load default
        // not super
    }
}

void calib_store(const calibration_t *calibration_data) {
    uint16_t buffer[STORAGE_EEPROM_CALIBRATION_LENGTH_IN_HALF_WORD] = {0};
    memcpy(&calibration_storage.calibration_data, &calibration_data, sizeof(calibration_t));
    calibration_storage.crc16 = 0;
    memcpy(buffer, &calibration_storage, sizeof(calibration_storage_t));
    uint16_t calced_crc = calc_crc((uint8_t *)buffer, sizeof(calibration_storage_t));
    calibration_storage.crc16 = calced_crc;
    memcpy(buffer, &calibration_storage, sizeof(calibration_storage_t));
    eeprom_write_buffer(STORAGE_EEPROM_CALIBRATION_START, buffer, STORAGE_EEPROM_CALIBRATION_LENGTH_IN_HALF_WORD);
}

calibration_t *calib_get() {
    return &calibration_storage.calibration_data;
}
