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
        if (flash_read(addr, 1, &value) == 0) {
            printf("cannot read value from address %08x", addr);
            return 0;
        }

        if (value == EEPROM_PAGE_USED) {
            *address = addr;
            return 1;
        }
    }

    printf("empty page not found!\n");
    *address = 0;
    return 0;
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
    if (flash_read(addr, 1, &value) == 0) {
        printf("cannot read value from address %08x", addr);
        return 0;
    }

    if (value == EEPROM_PAGE_EMPTY) {
        *address = addr;
        return 1;
    }

    printf("empty page not found!\n");
    *address = 0;
    return 0;
}

static int eeprom_find_key_from_start(uint32_t address_page, uint16_t key, uint16_t *value)
{
    uint32_t shift, stored;
    uint16_t stored_key, stored_value;

    for (shift = 0; shift < eeprom_info.words_on_page; shift++) {
        if (flash_read(address_page + shift, 1, &stored) == 0) {
            printf("cannot read value from address %08x", address_page + shift);
            return 0;
        }

        stored_key = (stored >> 16) & 0x0000ffff;
        stored_value = stored & 0x0000ffff;

        if (stored == EEPROM_PAGE_EMPTY) {
            *value = 0;
            return 0;
        }

        if (stored_key == key) {
            *value = stored_value;
            return 1;
        }
    }

    *value = 0;
    return 0;
}

static int eeprom_find_key_from_end(uint32_t address_page, uint16_t key, uint16_t *value)
{
    uint32_t shift, stored;
    uint16_t stored_key, stored_value;

    for (shift = address_page + eeprom_info.words_on_page - 1; shift >= address_page; shift--) {
        if (flash_read(address_page + shift, 1, &stored) == 0) {
            printf("cannot read value from address %08x", address_page + shift);
            return 0;
        }

        stored_key = (stored >> 16) & 0x0000ffff;
        stored_value = stored & 0x0000ffff;

        if (stored == EEPROM_PAGE_EMPTY) {
            continue;
        }

        if (stored_key == key) {
            *value = stored_value;
            return 1;
        }
    }

    *value = 0;
    return 0;
}

static int eeprom_store_value(uint32_t address_page, uint16_t key, uint16_t value)
{
    uint32_t shift, stored;

    for (shift = 0; shift < eeprom_info.words_on_page; shift++) {
        if (flash_read(address_page + shift, 1, &stored) == 0) {
            printf("cannot read value from address %08x", address_page + shift);
            return 0;
        }

        // if empty record was found, save key and value
        if (stored == EEPROM_PAGE_EMPTY) {
            stored = (key << 16) | value;
            flash_write(address_page + shift, 1, &stored);
            return 1;
        }
    }

    return 0;
}

static int eeprom_move_current_page()
{
    uint32_t active_page_address, next_page_address;
    uint32_t shift, stored;
    uint16_t stored_key, stored_value;

    active_page_address = eeprom_info.flash_active_page_address;

    if (eeprom_find_next_empty_page(&next_page_address) == 0) {
        printf("empty page not found!\n");
        return 0;
    }

    // from end to start
    for (shift = active_page_address + eeprom_info.words_on_page - 1; shift >= active_page_address; shift--) {
        if (flash_read(active_page_address + shift, 1, &stored) == 0) {
            printf("cannot read value from address %08x", active_page_address + shift);
            return 0;
        }

        // get key and value
        stored_key = (stored >> 16) & 0x0000ffff;
        //stored_value = stored & 0x0000ffff;

        if (stored == EEPROM_PAGE_EMPTY) {
            continue;
        }

        // find this key on the next page
        // if not found
        if (eeprom_find_key_from_end(next_page_address, stored_key, &stored_value) == 0) {
            // copy from active to next
            stored_value = stored & 0x0000ffff;
            eeprom_store_value(next_page_address, stored_key, stored_value);
        }

        // if found - skip
    }

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
