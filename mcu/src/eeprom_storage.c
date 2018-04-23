/*
 * eeprom_storage.c
 *
 *  Created on: 22.04.2018
 *      Author: arne
 */

#include "eeprom_storage.h"
#include "eeprom_emulation.h"
#include <assert.h>

#define STORAGE_CHUNK_SIZE 16

typedef struct {
    uint16_t address_start;
    uint16_t address_end;
    bool valid;
} found_storage_chunk_t;

static found_storage_chunk_t found_storage_chunks[STORAGE_CHUNK_SIZE];

static void check_for_overlaps(uint16_t start_address, uint16_t size) {
    uint16_t end_address = start_address + size;
    bool chunk_already_known = false;
    uint last_chunk = 0;
    for (uint i = 0; i < STORAGE_CHUNK_SIZE; i++) {
        last_chunk = i;
        if (found_storage_chunks[i].valid) {
            if ((found_storage_chunks[i].address_start == start_address) && (found_storage_chunks[i].address_end == end_address)) {
                chunk_already_known = true;
            }
        } else {
            break;
        }
    }
    if (!chunk_already_known) {
        assert(STORAGE_CHUNK_SIZE > last_chunk);
        found_storage_chunks[last_chunk].valid = true;
        found_storage_chunks[last_chunk].address_start = start_address;
        found_storage_chunks[last_chunk].address_end = end_address;
    }

    for (uint i = 0; i < STORAGE_CHUNK_SIZE; i++) {
        if (found_storage_chunks[i].valid) {
            uint16_t address_start = found_storage_chunks[i].address_start;
            uint16_t address_end = found_storage_chunks[i].address_end;

            for (uint n = i; n < STORAGE_CHUNK_SIZE; n++) {
                if (found_storage_chunks[n].valid) {

                    if (((address_start > found_storage_chunks[n].address_start) && (found_storage_chunks[n].address_start > address_end)) ||
                        ((address_start > found_storage_chunks[n].address_end) && (found_storage_chunks[n].address_end > address_end))) {
                        assert(0); // there is an overlap of eeprom memory areas
                    }
                } else {
                    break;
                }
            }

        } else {
            break;
        }
    }
}

void eeprom_init() {
    EE_Init();
    for (uint i = 0; i < STORAGE_CHUNK_SIZE; i++) {
        found_storage_chunks[i].valid = false;
    }
}

void eeprom_write_buffer(uint16_t address, const uint16_t *data, uint16_t size_in_half_word) {
    uint i = 0;
    check_for_overlaps(address, size_in_half_word);
    for (uint virt_addre = address; virt_addre < address + size_in_half_word; virt_addre++) {
        uint16_t probe_val = 0;
        EE_ReadVariable(virt_addre, &probe_val);
        if (probe_val != data[i]) {
            EE_WriteVariable(virt_addre, data[i]);
        }
        i++;
    }
}

void eeprom_read_buffer(uint16_t address, uint16_t *data, uint16_t size_in_half_word) {
    uint i = 0;
    check_for_overlaps(address, size_in_half_word);
    for (uint virt_addre = address; virt_addre < address + size_in_half_word; virt_addre++) {
        uint16_t probe_val = 0;
        EE_ReadVariable(virt_addre, &data[i]);
        i++;
    }
}
