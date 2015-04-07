/*
 * eeprom_emulation.h
 *
 *  Created on: 07 апр. 2015 г.
 *      Author: vladimir
 */

#ifndef EEPROM_EMULATION_H_
#define EEPROM_EMULATION_H_

#include <stdint.h>

int eeprom_init_debug(
    uint32_t flash_address,
    uint32_t flash_size,
    uint32_t words_on_page,
    uint32_t pages_on_block,
    uint32_t blocks_count);
int eeprom_read_value(uint16_t key, uint16_t *value);
int eeprom_write_value(uint16_t key, uint16_t value);

#endif /* EEPROM_EMULATION_H_ */
