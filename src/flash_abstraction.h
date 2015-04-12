/*
 * flash_abstraction.h
 *
 *  Created on: 07 апр. 2015 г.
 *      Author: vladimir
 */

#ifndef FLASH_ABSTRACTION_H_
#define FLASH_ABSTRACTION_H_

#include <stdint.h>

#define FLASH_RESULT_SUCCESS (0)
#define FLASH_RESULT_INVALID_ADDRESS (1)
#define FLASH_RESULT_NEED_ERASE (2)

int flash_read_word(uint32_t address, uint32_t *data);
int flash_can_overwrite(uint32_t value_old, uint32_t value_new);
int flash_write_word(uint32_t address, uint32_t data);
int flash_erase_page(uint32_t page_address, uint32_t words_on_page);

#endif /* FLASH_ABSTRACTION_H_ */
