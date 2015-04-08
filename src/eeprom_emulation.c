/*
 * eeprom_emulation.c
 *
 *  Created on: 07 апр. 2015 г.
 *      Author: vladimir
 */

#include "eeprom_emulation.h"
#include "flash_abstraction.h"
#include <stdio.h>

/*
 * page status changes:
 * 1. empty -> active
 * 2.
 *    active -> copy-out -> empty  -> empty    -> empty
 *    empty  -> copy-in  -> active -> copy-out -> empty
 *    empty  -> empty    -> empty  -> copy-in  -> active
 */
#define EEPROM_KEY_MASK      (0xffff0000)
#define EEPROM_PAGE_EMPTY    (0xffff0000)
#define EEPROM_PAGE_COPY_IN  (0xfffe0000)
#define EEPROM_PAGE_ACTIVE   (0xfffc0000)
#define EEPROM_PAGE_COPY_OUT (0xfff80000)

typedef struct _t_eeprom_info {
    uint32_t flash_address;
    uint32_t flash_size;
    uint32_t words_on_page;
    uint32_t pages_count;
    uint32_t flash_active_page_address;
} t_eeprom_info;

static t_eeprom_info eeprom_info;

static int eeprom_find_used_page(uint32_t *address)
{
    uint32_t addr, page_header;

    for (addr = eeprom_info.flash_address; addr < eeprom_info.flash_address + eeprom_info.flash_size; addr += eeprom_info.words_on_page) {
        if (flash_read(addr, 1, &page_header) != FLASH_RESULT_SUCCESS) {
            printf("cannot read value from address %08x", addr);
            return EEPROM_RESULT_READ_FAILED;
        }

        if ((page_header & EEPROM_KEY_MASK) == EEPROM_PAGE_ACTIVE) {
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
    uint32_t addr, page_header;
    uint32_t addr_start = eeprom_info.flash_active_page_address;

    // address of the next page
    addr = addr_start + eeprom_info.words_on_page;

    if (addr >= eeprom_info.flash_address + eeprom_info.flash_size) {
        addr = eeprom_info.flash_address;
    }

    // read page header
    if (flash_read(addr, 1, &page_header) != FLASH_RESULT_SUCCESS) {
        printf("cannot read value from address %08x", addr);
        return EEPROM_RESULT_READ_FAILED;
    }

    if ((page_header & EEPROM_KEY_MASK) == EEPROM_PAGE_EMPTY) {
        *address = addr;
        return EEPROM_RESULT_SUCCESS;
    }

    printf("empty page not found!\n");
    *address = 0;
    return EEPROM_RESULT_NO_EMPTY_PAGE;
}

static int eeprom_find_key_from_end(uint32_t address_page, uint16_t key, uint16_t *value)
{
    uint32_t shift, stored, search_key;

    shift = eeprom_info.words_on_page - 1;
    search_key = key << 16;

    while (1) {
        if (flash_read(address_page + shift, 1, &stored) != FLASH_RESULT_SUCCESS) {
            printf("cannot read value from address %08x", address_page + shift);
            return EEPROM_RESULT_READ_FAILED;
        }

        if ((stored & EEPROM_KEY_MASK) == search_key) {
            *value = (stored & 0x0000ffff);
            return EEPROM_RESULT_SUCCESS;
        }

        if (shift <= 1) {
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

    for (shift = 1; shift < eeprom_info.words_on_page; shift++) {
        if (flash_read(address_page + shift, 1, &stored) != FLASH_RESULT_SUCCESS) {
            printf("cannot read value from address %08x", address_page + shift);
            return EEPROM_RESULT_READ_FAILED;
        }

        // if empty record was found, save key and value
        if ((stored & EEPROM_KEY_MASK) == EEPROM_PAGE_EMPTY) {
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
    uint32_t state_copy_in = EEPROM_PAGE_COPY_IN;
    uint32_t state_copy_out = EEPROM_PAGE_COPY_OUT;
    uint32_t state_active = EEPROM_PAGE_ACTIVE;

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

    // current page to copy-out state
    flash_write(active_page_address + 0, 1, &state_copy_out);
    // next page to copy-in state
    flash_write(next_page_address + 0, 1, &state_copy_in);

    // from end to start
    shift = eeprom_info.words_on_page - 1;

    // iterate records
    while (1) {
        if (flash_read(active_page_address + shift, 1, &stored) != FLASH_RESULT_SUCCESS) {
            printf("cannot read value from address %08x", active_page_address + shift);
            return EEPROM_RESULT_READ_FAILED;
        }

        if ((stored & EEPROM_KEY_MASK) != EEPROM_PAGE_EMPTY) {
            // get key and value
            stored_key = (stored >> 16) & 0x0000ffff;

            // find this key on the next page
            // if not found
            if (eeprom_find_key_from_end(next_page_address, stored_key, &stored_value) == EEPROM_RESULT_KEY_NOT_FOUND) {
                // copy from active to next
                stored_value = stored & 0x0000ffff;
                eeprom_store_value(next_page_address, stored_key, stored_value);
            }

            // if found - skip
        }

        if (shift <= 1) {
            break;
        }

        shift--;

    }

    // next page to active state
    flash_write(next_page_address, 1, &state_active);
    // current page to empty state
    // flash_write(next_page_address, 1, state_active);

    eeprom_info.flash_active_page_address = next_page_address;
    flash_erase(active_page_address, eeprom_info.words_on_page);

    return EEPROM_RESULT_SUCCESS;
}

int eeprom_init_debug(
    uint32_t flash_address,
    uint32_t flash_size,
    uint32_t words_on_page,
    uint32_t pages_count)
{
    eeprom_info.flash_address = flash_address;
    eeprom_info.flash_size = flash_size;
    eeprom_info.words_on_page = words_on_page;
    eeprom_info.pages_count = pages_count;
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
