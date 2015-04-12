/*
 * flash_abstraction.c
 *
 *  Created on: 07 апр. 2015 г.
 *      Author: vladimir
 */

#include "flash_abstraction.h"
#include <string.h>
#include <stdio.h>

#define FLASH_SIZE (0xffffffff)

int flash_read_word(uint32_t address, uint32_t *data)
{
    uint32_t *p;

    if (address >= FLASH_SIZE) {
        printf("address out of limits\n");
        return FLASH_RESULT_INVALID_ADDRESS;
    }

    p = (uint32_t *)address;
    *data = *p;

    return FLASH_RESULT_SUCCESS;

}

int flash_can_overwrite(uint32_t value_old, uint32_t value_new)
{
    uint32_t old_data, new_data;
    /*
     * forbid write 1 bit to 0 bit
     *
     * 1010111111000 old
     * 1111011000011 new
     * -x-x-------xx
     *
     * inv old
     * 0101000000111
     * xor new
     * 1010011000100
     * inv
     * 0101100111011
     * and new
     * 0101000000011
     */

    old_data = value_old;
    new_data = value_new;

    old_data = old_data ^ 0xffffffff; // inv old
    old_data = old_data ^ new_data; // xor new
    old_data = old_data ^ 0xffffffff; // inv
    old_data = old_data & new_data; // and new

    if (old_data != 0) {
        // can not be overwritten without erase
        return FLASH_RESULT_NEED_ERASE;
    }

    return FLASH_RESULT_SUCCESS;
}

int flash_write_word(uint32_t address, uint32_t data)
{
    uint32_t old_data;
    uint32_t *p;

    if (address >= FLASH_SIZE) {
        printf("address out of limits\n");
        return FLASH_RESULT_INVALID_ADDRESS;
    }

    p = (uint32_t *)address;
    old_data = *p;

    if (flash_can_overwrite(old_data, data) == FLASH_RESULT_NEED_ERASE) {
        printf("cannot write 1 (%08x) to 0 (%08x) without erase at address (%08x) !!!\n", data, old_data, address);
        return FLASH_RESULT_NEED_ERASE;
    }

    *p = data;

    return FLASH_RESULT_SUCCESS;
}

int flash_erase_page(uint32_t page_address, uint32_t words_on_page)
{
    uint32_t i;
    uint32_t *p;

    if (page_address + (words_on_page * sizeof(uint32_t)) >= FLASH_SIZE) {
        printf("address out of limits\n");
        return FLASH_RESULT_INVALID_ADDRESS;
    }

    p = (uint32_t *)page_address;

    for (i = 0; i < words_on_page; i++) {
        p[i] = 0xffffffff;
    }

    return FLASH_RESULT_SUCCESS;
}
