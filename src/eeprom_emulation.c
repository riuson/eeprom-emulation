/*
 * eeprom_emulation.c
 *
 *  Created on: 07 апр. 2015 г.
 *      Author: vladimir
 */

#include "eeprom_emulation.h"
#include "flash_abstraction.h"
#include <stdio.h>


#define EEPROM_PAGE_EMPTY (0xffffffff)
#define EEPROM_PAGE_USED (0x00000000)

typedef struct _t_eeprom_info {
    uint32_t flash_address;
    uint32_t flash_size;
    uint32_t words_on_page;
    uint32_t pages_on_block;
    uint32_t blocks_count;
    uint32_t flash_active_page_address;
} t_eeprom_info;

static t_eeprom_info eeprom_info;

static int eeprom_find_used_page(uint32_t *address)
{
    uint32_t addr, value;

    for (addr = eeprom_info.flash_address; addr < eeprom_info.flash_address + eeprom_info.flash_size; addr += eeprom_info.words_on_page) {
        if (flash_read(addr, 1, &value) != FLASH_RESULT_SUCCESS) {
            printf("cannot read value from address %08x", addr);
            return EEPROM_RESULT_READ_FAILED;
        }

        if (value == EEPROM_PAGE_USED) {
            *address = addr;
            return EEPROM_RESULT_SUCCESS;
        }
    }

    printf("empty page not found!\n");
    *address = 0;
    return EEPROM_RESULT_NO_EMPTY_PAGE;
}

static int eeprom_find_next_empty_page(uint32_t *address)
{
    uint32_t addr, value;
    uint32_t addr_start = eeprom_info.flash_active_page_address;

    // address of the next page
    addr = addr_start + eeprom_info.words_on_page;

    if (addr >= eeprom_info.flash_address + eeprom_info.flash_size) {
        addr = eeprom_info.flash_address;
    }

    // read start word
    if (flash_read(addr, 1, &value) != FLASH_RESULT_SUCCESS) {
        printf("cannot read value from address %08x", addr);
        return EEPROM_RESULT_READ_FAILED;
    }

    if (value == EEPROM_PAGE_EMPTY) {
        *address = addr;
        return EEPROM_RESULT_SUCCESS;
    }

    printf("empty page not found!\n");
    *address = 0;
    return EEPROM_RESULT_NO_EMPTY_PAGE;
}

static int eeprom_find_key_from_start(uint32_t address_page, uint16_t key, uint16_t *value)
{
    uint32_t shift, stored;
    uint16_t stored_key, stored_value;

    for (shift = 0; shift < eeprom_info.words_on_page; shift++) {
        if (flash_read(address_page + shift, 1, &stored) != FLASH_RESULT_SUCCESS) {
            printf("cannot read value from address %08x", address_page + shift);
            return EEPROM_RESULT_READ_FAILED;
        }

        stored_key = (stored >> 16) & 0x0000ffff;
        stored_value = stored & 0x0000ffff;

        if (stored == EEPROM_PAGE_EMPTY) {
            *value = 0;
            return EEPROM_RESULT_KEY_NOT_FOUND;
        }

        if (stored_key == key) {
            *value = stored_value;
            return EEPROM_RESULT_SUCCESS;
        }
    }

    *value = 0;
    return EEPROM_RESULT_KEY_NOT_FOUND;
}

static int eeprom_find_key_from_end(uint32_t address_page, uint16_t key, uint16_t *value)
{
    uint32_t shift, stored;
    uint16_t stored_key, stored_value;

    shift = eeprom_info.words_on_page - 1;

    while (1) {
        if (flash_read(address_page + shift, 1, &stored) != FLASH_RESULT_SUCCESS) {
            printf("cannot read value from address %08x", address_page + shift);
            return EEPROM_RESULT_READ_FAILED;
        }

        stored_key = (stored >> 16) & 0x0000ffff;
        stored_value = stored & 0x0000ffff;

        if (stored_key == key) {
            *value = stored_value;
            return EEPROM_RESULT_SUCCESS;
        }

        if (shift == 0) {
            break;
        }

        shift--;
    }

    *value = 0;
    return EEPROM_RESULT_KEY_NOT_FOUND;
}

static int eeprom_store_value(uint32_t address_page, uint16_t key, uint16_t value)
{
    uint32_t shift, stored;

    for (shift = 0; shift < eeprom_info.words_on_page; shift++) {
        if (flash_read(address_page + shift, 1, &stored) != FLASH_RESULT_SUCCESS) {
            printf("cannot read value from address %08x", address_page + shift);
            return EEPROM_RESULT_READ_FAILED;
        }

        // if empty record was found, save key and value
        if (stored == EEPROM_PAGE_EMPTY) {
            stored = (key << 16) | value;
            flash_write(address_page + shift, 1, &stored);
            return EEPROM_RESULT_SUCCESS;
        }
    }

    return EEPROM_RESULT_NO_EMPTY_RECORD;
}

static int eeprom_move_current_page()
{
    uint32_t active_page_address, next_page_address;
    uint32_t shift, stored;
    uint16_t stored_key, stored_value;

    active_page_address = eeprom_info.flash_active_page_address;

    switch (eeprom_find_next_empty_page(&next_page_address)) {
        case EEPROM_RESULT_NO_EMPTY_PAGE: {
            printf("empty page not found!\n");
            return EEPROM_RESULT_NO_EMPTY_PAGE;
        }

        case EEPROM_RESULT_READ_FAILED: {
            printf("read failed\n");
            return EEPROM_RESULT_READ_FAILED;
        }
    }

    // from end to start
    shift = eeprom_info.words_on_page - 1;

    while (1) {
        if (flash_read(active_page_address + shift, 1, &stored) != FLASH_RESULT_SUCCESS) {
            printf("cannot read value from address %08x", active_page_address + shift);
            return EEPROM_RESULT_READ_FAILED;
        }

        // get key and value
        stored_key = (stored >> 16) & 0x0000ffff;

        if (stored == EEPROM_PAGE_EMPTY) {
            continue;
        }

        // find this key on the next page
        // if not found
        if (eeprom_find_key_from_end(next_page_address, stored_key, &stored_value) == EEPROM_RESULT_KEY_NOT_FOUND) {
            // copy from active to next
            stored_value = stored & 0x0000ffff;
            eeprom_store_value(next_page_address, stored_key, stored_value);
        }

        // if found - skip

        if (shift == 0) {
            break;
        }

        shift--;

    }

    return EEPROM_RESULT_SUCCESS;
}

int eeprom_init_debug(
    uint32_t flash_address,
    uint32_t flash_size,
    uint32_t words_on_page,
    uint32_t pages_on_block,
    uint32_t blocks_count)
{
    eeprom_info.flash_address = flash_address;
    eeprom_info.flash_size = flash_size;
    eeprom_info.words_on_page = words_on_page;
    eeprom_info.pages_on_block = pages_on_block;
    eeprom_info.blocks_count = blocks_count;
}

int eeprom_read_value(uint16_t key, uint16_t *value)
{
    return (eeprom_find_key_from_end(eeprom_info.flash_active_page_address, key, value));
}

int eeprom_write_value(uint16_t key, uint16_t value)
{
    // try store valye
    switch (eeprom_store_value(eeprom_info.flash_active_page_address, key, value)) {
        case EEPROM_RESULT_READ_FAILED : {
            return EEPROM_RESULT_READ_FAILED;
        }

        case EEPROM_RESULT_NO_EMPTY_RECORD: {
            // move page
            switch (eeprom_move_current_page()) {
                case EEPROM_RESULT_NO_EMPTY_PAGE: {
                    return EEPROM_RESULT_NO_EMPTY_PAGE;
                }

                case EEPROM_RESULT_READ_FAILED: {
                    return EEPROM_RESULT_READ_FAILED;
                }

                case EEPROM_RESULT_SUCCESS: {
                    // try store value again
                    switch (eeprom_store_value(eeprom_info.flash_active_page_address, key, value)) {
                        case EEPROM_RESULT_READ_FAILED: {
                            return EEPROM_RESULT_READ_FAILED;
                        }

                        case EEPROM_RESULT_NO_EMPTY_RECORD: {
                            return EEPROM_RESULT_NO_EMPTY_RECORD;
                        }
                    }

                    break;
                }
            }

            break;
        }
    }

    return EEPROM_RESULT_SUCCESS;
}
