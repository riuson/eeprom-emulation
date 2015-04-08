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

static int eeprom_find_page_by_state(uint32_t state, uint32_t *address)
{
    uint32_t addr, page_header;

    for (addr = eeprom_info.flash_address; addr < eeprom_info.flash_address + eeprom_info.flash_size; addr += eeprom_info.words_on_page) {
        if (flash_read_word(addr, &page_header) != FLASH_RESULT_SUCCESS) {
            printf("cannot read value from address %08x", addr);
            return EEPROM_RESULT_READ_FAILED;
        }

        if ((page_header & EEPROM_KEY_MASK) == state) {
            *address = addr;
            return EEPROM_RESULT_SUCCESS;
        }
    }

    *address = 0;
    return EEPROM_RESULT_KEY_NOT_FOUND;
}

static uint32_t eeprom_calc_next_page_address(uint32_t current_address)
{
    uint32_t next_address = current_address + eeprom_info.words_on_page;

    if (next_address >= eeprom_info.flash_address + eeprom_info.flash_size) {
        next_address = eeprom_info.flash_address;
    }

    return next_address;
}

static int eeprom_find_next_empty_page(uint32_t *address)
{
    uint32_t addr, page_header;
    uint32_t addr_start = eeprom_info.flash_active_page_address;

    // address of the next page
    addr = eeprom_calc_next_page_address(addr_start);

    // read page header
    if (flash_read_word(addr, &page_header) != FLASH_RESULT_SUCCESS) {
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
        if (flash_read_word(address_page + shift, &stored) != FLASH_RESULT_SUCCESS) {
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
        if (flash_read_word(address_page + shift, &stored) != FLASH_RESULT_SUCCESS) {
            printf("cannot read value from address %08x", address_page + shift);
            return EEPROM_RESULT_READ_FAILED;
        }

        // if empty record was found, save key and value
        if ((stored & EEPROM_KEY_MASK) == EEPROM_PAGE_EMPTY) {
            flash_write_word(address_page + shift, (key << 16) | value);
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

    // current page to copy-out state
    flash_write_word(active_page_address + 0, EEPROM_PAGE_COPY_OUT);
    // next page to copy-in state
    flash_write_word(next_page_address + 0, EEPROM_PAGE_COPY_IN);

    // from end to start
    shift = eeprom_info.words_on_page - 1;

    // iterate records
    while (1) {
        if (flash_read_word(active_page_address + shift, &stored) != FLASH_RESULT_SUCCESS) {
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
    flash_write_word(next_page_address, EEPROM_PAGE_ACTIVE);
    // current page to empty state
    // flash_write(next_page_address, 1, state_active);

    eeprom_info.flash_active_page_address = next_page_address;
    flash_erase(active_page_address, eeprom_info.words_on_page);

    return EEPROM_RESULT_SUCCESS;
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

static int eeprom_check_state(void)
{
    /*
     * sequence:
     * 0. current: active, next: empty >>>
     * 1.     current: copy-out, next: empty >>>
     * 2.         current: copy-out, next: copy-in >>>
     * 3.             copy from current page to the next page >>>
     * 4.                 current: copy-out, next: active >>>
     * 5.                     current: empty (erased), next: active
     */

    uint32_t active_address, copy_in_address, copy_out_address;
    uint8_t active_exists, copy_in_exists, copy_out_exists;

    switch (eeprom_find_page_by_state(EEPROM_PAGE_ACTIVE, &active_address)) {
        case EEPROM_RESULT_SUCCESS: {
            active_exists = 1;
            break;
        }

        case EEPROM_RESULT_KEY_NOT_FOUND: {
            active_exists = 0;
            break;
        }

        case EEPROM_RESULT_READ_FAILED: {
            return EEPROM_RESULT_READ_FAILED;
        }
    }

    switch (eeprom_find_page_by_state(EEPROM_PAGE_COPY_IN, &copy_in_address)) {
        case EEPROM_RESULT_SUCCESS: {
            copy_in_exists = 1;
            break;
        }

        case EEPROM_RESULT_KEY_NOT_FOUND: {
            copy_in_exists = 0;
            break;
        }

        case EEPROM_RESULT_READ_FAILED: {
            return EEPROM_RESULT_READ_FAILED;
        }
    }

    switch (eeprom_find_page_by_state(EEPROM_PAGE_COPY_OUT, &copy_out_address)) {
        case EEPROM_RESULT_SUCCESS: {
            copy_out_exists = 1;
            break;
        }

        case EEPROM_RESULT_KEY_NOT_FOUND: {
            copy_out_exists = 0;
            break;
        }

        case EEPROM_RESULT_READ_FAILED: {
            return EEPROM_RESULT_READ_FAILED;
        }
    }

    if (!copy_out_exists && !copy_in_exists && active_exists) {
        // all ok
        return EEPROM_RESULT_SUCCESS;
    } else if (!copy_out_exists && !copy_in_exists && !active_exists) {
        // no one known page found

        // erase first page
        flash_erase(eeprom_info.flash_address, eeprom_info.words_on_page);

        // set it active
        flash_write_word(eeprom_info.flash_address, EEPROM_PAGE_ACTIVE);

        return EEPROM_RESULT_SUCCESS;
    } else if (copy_out_exists && !copy_in_exists && !active_exists) {
        // terminated between 1 and 2
        // copy-out - source page

        copy_in_address = eeprom_calc_next_page_address(copy_out_address);

        // erase next page
        flash_erase(copy_in_address, eeprom_info.words_on_page);

        // repeat copy
        eeprom_info.flash_active_page_address = copy_out_address;
        eeprom_move_current_page();

        return EEPROM_RESULT_SUCCESS;
    } else if (copy_out_exists && copy_in_exists && !active_exists) {
        // terminated between 2 and 4

        // erase next page
        flash_erase(copy_in_address, eeprom_info.words_on_page);

        // repeat copy
        eeprom_info.flash_active_page_address = copy_out_address;
        eeprom_move_current_page();

        return EEPROM_RESULT_SUCCESS;
    } else if (copy_out_exists && active_exists && !copy_in_exists) {
        // terminated between 4 and 5

        // erase previous page
        flash_erase(copy_out_address, eeprom_info.words_on_page);

        return EEPROM_RESULT_SUCCESS;
    }

    // unrecognized state, need format
    return 0;
}

int eeprom_init_debug(
    uint32_t flash_address,
    uint32_t flash_size,
    uint32_t words_on_page,
    uint32_t pages_count)
{
    uint32_t intermediate_page_address;
    uint32_t active_page_address;

    eeprom_info.flash_address = flash_address;
    eeprom_info.flash_size = flash_size;
    eeprom_info.words_on_page = words_on_page;
    eeprom_info.pages_count = pages_count;

    //flash_write_word(eeprom_info.flash_address + 0, EEPROM_PAGE_ACTIVE);

    //flash_write_word(eeprom_info.flash_address + 0, EEPROM_PAGE_COPY_OUT);
    //flash_write_word(eeprom_info.flash_address + eeprom_info.words_on_page, EEPROM_PAGE_EMPTY);

    //flash_write_word(eeprom_info.flash_address + 0, EEPROM_PAGE_COPY_OUT);
    //flash_write_word(eeprom_info.flash_address + eeprom_info.words_on_page, EEPROM_PAGE_COPY_IN);

    //flash_write_word(eeprom_info.flash_address + 0, EEPROM_PAGE_COPY_OUT);
    //flash_write_word(eeprom_info.flash_address + eeprom_info.words_on_page, EEPROM_PAGE_ACTIVE);

    eeprom_check_state();

    if (eeprom_find_page_by_state(EEPROM_PAGE_ACTIVE, &active_page_address) == EEPROM_RESULT_SUCCESS) {
        eeprom_info.flash_active_page_address = active_page_address;
    }
}
