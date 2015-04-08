/*
 * flash_abstraction.h
 *
 *  Created on: 07 апр. 2015 г.
 *      Author: vladimir
 */

#ifndef FLASH_ABSTRACTION_H_
#define FLASH_ABSTRACTION_H_

#include <stdint.h>

// size of one page in words
#define FLASH_WORDS_ON_PAGE (512 / 4)
// number of pages
#define FLASH_PAGES_COUNT (16)

#define FLASH_RESULT_SUCCESS (0)
#define FLASH_RESULT_INVALID_ADDRESS (1)
#define FLASH_RESULT_NEED_ERASE (2)

void flash_init_debug();
int flash_read(uint32_t address, uint32_t size, uint32_t *data);
int flash_can_overwrite(uint32_t value_old, uint32_t value_new);
int flash_write(uint32_t address, uint32_t size, const uint32_t *data);
int flash_erase(uint32_t address, uint32_t size);
void flash_print_debug(uint32_t address, uint32_t size);

#endif /* FLASH_ABSTRACTION_H_ */
