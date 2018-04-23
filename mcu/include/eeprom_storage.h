/*
 * eeprom_storage.h
 *
 *  Created on: 22.04.2018
 *      Author: arne
 */

#ifndef MCU_INCLUDE_EEPROM_STORAGE_H_
#define MCU_INCLUDE_EEPROM_STORAGE_H_

#include <stdbool.h>
#include <stdint.h>

void eeprom_init();
void eeprom_write_buffer(uint16_t address, const uint16_t *data, uint16_t size_in_half_word);
void eeprom_read_buffer(uint16_t address, uint16_t *data, uint16_t size_in_half_word);

#endif /* MCU_INCLUDE_EEPROM_STORAGE_H_ */
