/*
 * flash_abstraction.c
 *
 *  Created on: 07 апр. 2015 г.
 *      Author: vladimir
 */

#include "flash_abstraction.h"
#include <string.h>
#include <stdio.h>

#define FLASH_SIZE (FLASH_BLOCKS_COUNT * FLASH_PAGES_IN_BLOCK * FLASH_WORDS_ON_PAGE)

typedef struct _t_flash_memory_data {
    uint32_t data_array[FLASH_SIZE];
} t_flash_memory_data;

static t_flash_memory_data flash_memory_data;

void flash_init_debug()
{
    uint32_t i;

    for (i = 0; i < FLASH_SIZE; i++) {
        flash_memory_data.data_array[i] = 0xffffffff;
    }
}

int flash_read(uint32_t address, uint32_t size, uint32_t *data)
{
    uint32_t i;

    if (address + size >= FLASH_SIZE) {
        printf("address out of limits\n");
        return 0;
    }

    for (i = 0; i < size; i++) {
        data[i] = flash_memory_data.data_array[address + i];
    }

    return 1;

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
        return 0;
    }

    return 1;
}

int flash_write(uint32_t address, uint32_t size, const uint32_t *data)
{
    uint32_t i;
    uint32_t old_data, new_data;

    if (address + size >= FLASH_SIZE) {
        printf("address out of limits\n");
        return 0;
    }

    for (i = 0; i < size; i++) {
        old_data = flash_memory_data.data_array[address + i];
        new_data = data[i];

        if (flash_can_overwrite(old_data, new_data) == 0) {
            printf("cannot write 1 (%08x) to 0 (%08x) without erase at address (%08x) !!!\n", new_data, old_data, address);
            return 0;
        }

        flash_memory_data.data_array[address + i] = new_data;
    }

    return 1;
}

int flash_erase(uint32_t address, uint32_t size)
{
    uint32_t i;
    uint32_t boundary = address / FLASH_WORDS_ON_PAGE;
    boundary *= FLASH_WORDS_ON_PAGE;

    if (address + size >= FLASH_SIZE) {
        printf("address out of limits\n");
        return 0;
    }

    if (size % FLASH_WORDS_ON_PAGE != 0) {
        printf("size must be multiplicity of page size (%d)\n", FLASH_WORDS_ON_PAGE);
        return 0;
    }

    if (boundary != address) {
        printf("address %08x must be equals on page boundary %08x\n", address, boundary);
        return 0;
    }

    for (i = 0; i < size; i++) {
        flash_memory_data.data_array[address + i] = 0xffffffff;
    }

    return 1;
}

void flash_print_debug(uint32_t address, uint32_t size)
{
    uint32_t i, j, k;
    uint8_t *data = (uint8_t *) &flash_memory_data.data_array[0];

    for (i = 0; i < size; i += 256) {
        printf("Block at 0x%08x\n", address + i);

        for (j = 0; j < 256; j += 16) {
            printf("0x%08x: ", address + i + j);

            for (k = 0; k < 16; k++) {
                printf(" %02x", data[address + i + j + k]);
            }

            printf("\n");
        }
    }
}
