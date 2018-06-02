/*
 * storage_calibration.h
 *
 *  Created on: 22.04.2018
 *      Author: arne
 */

#ifndef MCU_INCLUDE_STORAGE_CALIBRATION_H_
#define MCU_INCLUDE_STORAGE_CALIBRATION_H_

#include <stdbool.h>
#include <stdint.h>

#include "rpc_transmission/server/app/qt2mcu.h"

extern const uint16_t STORAGE_EEPROM_CALIBRATION_START;
extern const uint16_t STORAGE_EEPROM_CALIBRATION_LENGTH;

void calib_init();
void calib_store(const calibration_t *calibration_data);
calibration_t *calib_get();

int32_t calib_apply_calibration(int32_t in_value, const calibration_coefficients_t *coeffs);

#endif /* MCU_INCLUDE_STORAGE_CALIBRATION_H_ */
