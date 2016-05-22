/*
 * eeprom_emulation.h
 *
 *  Created on: 07 апр. 2015 г.
 *      Author: vladimir
 */

#ifndef EEPROM_EMULATION_H_
#define EEPROM_EMULATION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define EEPROM_RESULT_SUCCESS (0)
#define EEPROM_RESULT_READ_FAILED (1)
#define EEPROM_RESULT_NO_EMPTY_PAGE (2)
#define EEPROM_RESULT_KEY_NOT_FOUND (3)
#define EEPROM_RESULT_NO_EMPTY_RECORD (4)
#define EEPROM_RESULT_INVALID_PARAMETERS (5)
#define EEPROM_RESULT_NEED_ERASE (6)
#define EEPROM_RESULT_UNCATCHED_FAIL (0xff)

typedef struct _t_eeprom_config {
    uint32_t words_on_page;
    uint32_t pages_count;
    uint32_t total_size;
    uint32_t active_page_index;
} t_eeprom_config;

int eeprom_init(
    uint32_t flash_address, t_eeprom_config *config);

int eeprom_read_value(
    uint32_t flash_address, t_eeprom_config *config,
    uint16_t key, uint16_t *value);

int eeprom_write_value(
    uint32_t flash_address, t_eeprom_config *config,
    uint16_t key, uint16_t value);

int eeprom_keys_count(
    uint32_t flash_address, t_eeprom_config *config,
    uint16_t *count);

int eeprom_read_by_index(
    uint32_t flash_address, t_eeprom_config *config,
    uint16_t index, uint16_t *key, uint16_t *value);

void eeprom_print_debug(
    uint32_t flash_address, t_eeprom_config *config,
    uint32_t address, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif /* EEPROM_EMULATION_H_ */
