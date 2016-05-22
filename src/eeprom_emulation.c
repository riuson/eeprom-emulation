/*
 * eeprom_emulation.c
 *
 *  Created on: 07 апр. 2015 г.
 *      Author: vladimir
 */

#include "eeprom_emulation.h"
#include "flash_abstraction.h"
//#include <stdio.h>

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

static uint32_t get_page_address(
    uint32_t flash_address, t_eeprom_config *config,
    uint32_t page_index)
{
    uint32_t result = flash_address + (sizeof(uint32_t) * config->words_on_page) * page_index;
    return result;
}

static int eeprom_find_page_by_state(
    uint32_t flash_address, t_eeprom_config *config,
    uint32_t state, uint32_t *page_index)
{
    uint32_t addr, page_header, page;

    for (page = 0; page < config->pages_count; page++) {
        if (flash_read_word(get_page_address(flash_address, config, page), &page_header) != FLASH_RESULT_SUCCESS) {
            //printf("cannot read value from address %08x", get_page_address(flash_address, config, page));
            return EEPROM_RESULT_READ_FAILED;
        }

        if ((page_header & EEPROM_KEY_MASK) == state) {
            *page_index = page;
            return EEPROM_RESULT_SUCCESS;
        }
    }

    *page_index = 0;
    return EEPROM_RESULT_KEY_NOT_FOUND;
}

static uint32_t eeprom_calc_next_page_index(
    uint32_t flash_address, t_eeprom_config *config,
    uint32_t current_page_index)
{
    if ((current_page_index + 1) < config->pages_count) {
        return current_page_index + 1;
    }

    return 0;
}

static int eeprom_find_next_empty_page_index(
    uint32_t flash_address, t_eeprom_config *config,
    uint32_t *page_index)
{
    uint32_t next_page_index, page_header;
    uint32_t start_page_index = config->active_page_index;

    // index of the next page
    next_page_index = eeprom_calc_next_page_index(flash_address, config, start_page_index);

    // read page header
    if (flash_read_word(get_page_address(flash_address, config, next_page_index), &page_header) != FLASH_RESULT_SUCCESS) {
        //printf("cannot read value from address %08x", get_page_address(flash_address, config, next_page_index));
        return EEPROM_RESULT_READ_FAILED;
    }

    if ((page_header & EEPROM_KEY_MASK) == EEPROM_PAGE_EMPTY) {
        *page_index = next_page_index;
        return EEPROM_RESULT_SUCCESS;
    }

    //printf("empty page not found!123\n");
    //printf("flash: 0x%08x, w: %d, p: %d, api: 0x%08x, apa: 0x%08x\n",
    //       flash_address,
    //       config->words_on_page,
    //       config->pages_count,
    //       config->active_page_index,
    //       get_page_address(flash_address, config, config->active_page_index));
    *page_index = 0;
    return EEPROM_RESULT_NO_EMPTY_PAGE;
}

static int eeprom_can_compact(
    uint32_t flash_address, t_eeprom_config *config,
    uint32_t page_index)
{
    uint32_t shift1, shift2, stored, search_key;
    shift1 = config->words_on_page - 1;

    // Loop external from page end to start
    while (1) {
        if (flash_read_word(get_page_address(flash_address, config, page_index) + shift1 * sizeof(uint32_t), &stored) != FLASH_RESULT_SUCCESS) {
            //printf("cannot read value from address %08x", get_page_address(flash_address, config, page_index) + shift1 * sizeof(uint32_t));
            return EEPROM_RESULT_READ_FAILED;
        }

        if (shift1 <= 1) {
            break;
        }

        if ((stored & EEPROM_KEY_MASK) != EEPROM_PAGE_EMPTY) {
            search_key = (stored >> 16) & 0x0000ffff;
            shift2 = shift1 - 1;

            // Loop internal from external to start
            while (1) {
                if (flash_read_word(get_page_address(flash_address, config,  page_index) + shift2 * sizeof(uint32_t), &stored) != FLASH_RESULT_SUCCESS) {
                    //printf("cannot read value from address %08x", get_page_address(flash_address, config, page_index) + shift2 * sizeof(uint32_t));
                    return EEPROM_RESULT_READ_FAILED;
                }

                if (search_key == ((stored >> 16) & 0x0000ffff)) {
                    return EEPROM_RESULT_SUCCESS;
                }

                if (shift2 <= 1) {
                    break;
                }

                shift2--;
            }
        }

        shift1--;
    }

    return EEPROM_RESULT_NO_EMPTY_RECORD;
}

static int eeprom_find_key_from_end(
    uint32_t flash_address, t_eeprom_config *config,
    uint32_t page_index, uint16_t key, uint16_t *value, uint32_t *valueShift)
{
    uint32_t shift, stored, search_key;

    shift = config->words_on_page - 1;
    search_key = key << 16;

    while (1) {
        if (flash_read_word(get_page_address(flash_address, config, page_index) + shift * sizeof(uint32_t), &stored) != FLASH_RESULT_SUCCESS) {
            //printf("cannot read value from address %08x", get_page_address(flash_address, config, page_index) + shift * sizeof(uint32_t));
            return EEPROM_RESULT_READ_FAILED;
        }

        if ((stored & EEPROM_KEY_MASK) == search_key) {
            *value = (stored & 0x0000ffff);
            *valueShift = shift;
            return EEPROM_RESULT_SUCCESS;
        }

        if (shift <= 1) {
            break;
        }

        shift--;
    }

    *value = 0;
    *valueShift = 0;
    return EEPROM_RESULT_KEY_NOT_FOUND;
}

static int eeprom_store_value(
    uint32_t flash_address, t_eeprom_config *config,
    uint32_t page_index, uint16_t key, uint16_t value)
{
    uint32_t shift = config->words_on_page - 1, stored, same = 0xffffffff, same_shift = 0xffffffff;
    uint32_t to_store = (key << 16) | value;

    // check for overwrite possibility on existing same record
    switch (eeprom_find_key_from_end(flash_address, config, page_index, key, &stored, &same_shift)) {
        case EEPROM_RESULT_SUCCESS: {
            same = (key << 16) | stored;

            // if same value, nothing to do
            if (same == to_store) {
                return EEPROM_RESULT_SUCCESS;
            }

            // overwrite if can
            if (flash_can_overwrite(same, to_store) == FLASH_RESULT_SUCCESS) {
                flash_write_word(get_page_address(flash_address, config, page_index) + same_shift * sizeof(uint32_t), to_store);
                return EEPROM_RESULT_SUCCESS;
            }

            break;
        }

        case EEPROM_RESULT_READ_FAILED: {
            return EEPROM_RESULT_READ_FAILED;
        }

        case EEPROM_RESULT_KEY_NOT_FOUND: {
            break;
        }
    }

    for (shift = 1; shift < config->words_on_page; shift++) {
        if (flash_read_word(get_page_address(flash_address, config, page_index) + shift * sizeof(uint32_t), &stored) != FLASH_RESULT_SUCCESS) {
            //printf("cannot read value from address %08x", get_page_address(flash_address, config, page_index) + shift * sizeof(uint32_t));
            return EEPROM_RESULT_READ_FAILED;
        }

        // if empty record was found, save key and value
        if ((stored & EEPROM_KEY_MASK) == EEPROM_PAGE_EMPTY) {
            flash_write_word(get_page_address(flash_address, config, page_index) + shift * sizeof(uint32_t), to_store);
            return EEPROM_RESULT_SUCCESS;
        }
    }

    return EEPROM_RESULT_NO_EMPTY_RECORD;
}

static int eeprom_move_current_page(
    uint32_t flash_address, t_eeprom_config *config)
{
    uint32_t active_page_index, next_page_index;
    uint32_t shift, stored, stored_shift;
    uint16_t stored_key, stored_value;

    active_page_index = config->active_page_index;

    // where no reason to move page, what cannot be compacted;
    // so check possibility
    switch (eeprom_can_compact(flash_address, config, active_page_index)) {
        case EEPROM_RESULT_NO_EMPTY_RECORD: {
            return EEPROM_RESULT_NO_EMPTY_RECORD;
        }

        case EEPROM_RESULT_READ_FAILED: {
            //printf("read failed\n");
            return EEPROM_RESULT_READ_FAILED;
        }

        case EEPROM_RESULT_SUCCESS: {
            break;
        }
    }

    // find room to new page
    switch (eeprom_find_next_empty_page_index(flash_address, config, &next_page_index)) {
        case EEPROM_RESULT_NO_EMPTY_PAGE: {
            //printf("empty page not found!\n");
            return EEPROM_RESULT_NO_EMPTY_PAGE;
        }

        case EEPROM_RESULT_READ_FAILED: {
            //printf("read failed\n");
            return EEPROM_RESULT_READ_FAILED;
        }

        case EEPROM_RESULT_SUCCESS: {
            break;
        }
    }

    // current page to copy-out state
    flash_write_word(get_page_address(flash_address, config, active_page_index) + 0, EEPROM_PAGE_COPY_OUT);
    // next page to copy-in state
    flash_write_word(get_page_address(flash_address, config, next_page_index) + 0, EEPROM_PAGE_COPY_IN);

    // from end to start
    shift = config->words_on_page - 1;

    // iterate records
    while (1) {
        if (flash_read_word(get_page_address(flash_address, config, active_page_index) + shift * sizeof(uint32_t), &stored) != FLASH_RESULT_SUCCESS) {
            //printf("cannot read value from address %08x", get_page_address(flash_address, config, active_page_index) + shift * sizeof(uint32_t));
            return EEPROM_RESULT_READ_FAILED;
        }

        if ((stored & EEPROM_KEY_MASK) != EEPROM_PAGE_EMPTY) {
            // get key and value
            stored_key = (stored >> 16) & 0x0000ffff;

            // find this key on the next page
            // if not found
            if (eeprom_find_key_from_end(flash_address, config, next_page_index, stored_key, &stored_value, &stored_shift) == EEPROM_RESULT_KEY_NOT_FOUND) {
                // copy from active to next
                stored_value = stored & 0x0000ffff;
                eeprom_store_value(flash_address, config, next_page_index, stored_key, stored_value);
            }

            // if found - skip
        }

        if (shift <= 1) {
            break;
        }

        shift--;

    }

    // next page to active state
    flash_write_word(get_page_address(flash_address, config, next_page_index), EEPROM_PAGE_ACTIVE);
    // current page to empty state
    // flash_write(next_page_address, 1, state_active);

    config->active_page_index = next_page_index;
    flash_erase_page(get_page_address(flash_address, config, active_page_index), config->words_on_page);

    return EEPROM_RESULT_SUCCESS;
}

int eeprom_read_value(
    uint32_t flash_address, t_eeprom_config *config,
    uint16_t key, uint16_t *value)
{
    uint32_t shift;
    return (eeprom_find_key_from_end(flash_address, config, config->active_page_index, key, value, &shift));
}

int eeprom_write_value(
    uint32_t flash_address, t_eeprom_config *config,
    uint16_t key, uint16_t value)
{
    // try store valye
    switch (eeprom_store_value(flash_address, config, config->active_page_index, key, value)) {
        case EEPROM_RESULT_READ_FAILED: {
            return EEPROM_RESULT_READ_FAILED;
        }

        case EEPROM_RESULT_NO_EMPTY_RECORD: {
            // move page
            switch (eeprom_move_current_page(flash_address, config)) {
                case EEPROM_RESULT_NO_EMPTY_PAGE: {
                    return EEPROM_RESULT_NO_EMPTY_PAGE;
                }

                case EEPROM_RESULT_NO_EMPTY_RECORD: {
                    return EEPROM_RESULT_NO_EMPTY_RECORD;
                }

                case EEPROM_RESULT_READ_FAILED: {
                    return EEPROM_RESULT_READ_FAILED;
                }

                case EEPROM_RESULT_SUCCESS: {
                    // try store value again
                    switch (eeprom_store_value(flash_address, config, config->active_page_index, key, value)) {
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

int eeprom_keys_count(
    uint32_t flash_address, t_eeprom_config *config,
    uint16_t *count)
{
    uint32_t shift1, shift2, stored, search_key;
    const uint32_t page_index = config->active_page_index;
    shift1 = config->words_on_page - 1;
    *count = 0;

    // Loop external from page end to start
    while (1) {
        if (flash_read_word(get_page_address(flash_address, config, page_index) + shift1 * sizeof(uint32_t), &stored) != FLASH_RESULT_SUCCESS) {
            //printf("cannot read value from address %08x", get_page_address(flash_address, config, page_index) + shift1 * sizeof(uint32_t));
            return EEPROM_RESULT_READ_FAILED;
        }


        if ((stored & EEPROM_KEY_MASK) != EEPROM_PAGE_EMPTY) {
            (*count)++;
            search_key = (stored >> 16) & 0x0000ffff;
            shift2 = shift1 - 1;

            // Loop internal from external to start
            while (shift1 > 1) {
                if (flash_read_word(get_page_address(flash_address, config, page_index) + shift2 * sizeof(uint32_t), &stored) != FLASH_RESULT_SUCCESS) {
                    //printf("cannot read value from address %08x", get_page_address(flash_address, config, page_index) + shift2 * sizeof(uint32_t));
                    return EEPROM_RESULT_READ_FAILED;
                }

                if (search_key == ((stored >> 16) & 0x0000ffff)) {
                    (*count)--;
                    break;
                }

                if (shift2 <= 1) {
                    break;
                }

                shift2--;
            }
        }

        if (shift1 <= 1) {
            break;
        }

        shift1--;
    }

    return EEPROM_RESULT_SUCCESS;
}

int eeprom_read_by_index(
    uint32_t flash_address, t_eeprom_config *config,
    uint16_t index, uint16_t *key, uint16_t *value)
{
    uint32_t shift1, shift2, stored, search_key;
    const uint32_t page_index = config->active_page_index;
    uint16_t count = 0;
    shift1 = config->words_on_page - 1;

    // Loop external from page end to start
    while (1) {
        if (flash_read_word(get_page_address(flash_address, config, page_index) + shift1 * sizeof(uint32_t), &stored) != FLASH_RESULT_SUCCESS) {
            //printf("cannot read value from address %08x", get_page_address(flash_address, config, page_index) + shift1 * sizeof(uint32_t));
            return EEPROM_RESULT_READ_FAILED;
        }

        if ((stored & EEPROM_KEY_MASK) != EEPROM_PAGE_EMPTY) {
            count++;
            search_key = (stored >> 16) & 0x0000ffff;
            shift2 = shift1 - 1;

            if (count == index + 1) {
                *key = (stored >> 16) & 0x0000ffff;
                *value = stored & 0x0000ffff;
                return EEPROM_RESULT_SUCCESS;
            }

            // Loop internal from external to start
            while (shift1 > 1) {
                if (flash_read_word(get_page_address(flash_address, config, page_index) + shift2 * sizeof(uint32_t), &stored) != FLASH_RESULT_SUCCESS) {
                    //printf("cannot read value from address %08x", get_page_address(flash_address, config, page_index) + shift2 * sizeof(uint32_t));
                    return EEPROM_RESULT_READ_FAILED;
                }

                if (search_key == ((stored >> 16) & 0x0000ffff)) {
                    count--;
                    break;
                }

                if (shift2 <= 1) {
                    break;
                }

                shift2--;
            }
        }

        if (shift1 <= 1) {
            break;
        }

        shift1--;
    }

    return EEPROM_RESULT_KEY_NOT_FOUND;
}

static int eeprom_format(
    uint32_t flash_address, t_eeprom_config *config)
{
    uint32_t shift, value, page_index;

    // erase
    for (page_index = 0; page_index < config->pages_count; page_index++) {
        flash_erase_page(get_page_address(flash_address, config, page_index), config->words_on_page);
    }

    // check
    for (shift = 0; shift < config->pages_count * config->words_on_page; shift++) {

        if (flash_read_word(flash_address + shift * sizeof(uint32_t), &value) == FLASH_RESULT_INVALID_ADDRESS) {
            return EEPROM_RESULT_INVALID_PARAMETERS;
        }

        if (value != 0xffffffff) {
            return EEPROM_RESULT_NEED_ERASE;
        }
    }

    // set first page active
    switch (flash_write_word(flash_address, EEPROM_PAGE_ACTIVE)) {
        case FLASH_RESULT_SUCCESS: {
            return EEPROM_RESULT_SUCCESS;
        }

        case FLASH_RESULT_INVALID_ADDRESS: {
            return EEPROM_RESULT_INVALID_PARAMETERS;
        }

        case FLASH_RESULT_NEED_ERASE: {
            return EEPROM_RESULT_NEED_ERASE;
        }

        default: {
            return EEPROM_RESULT_UNCATCHED_FAIL;
        }
    }
}

static int eeprom_check_state(
    uint32_t flash_address, t_eeprom_config *config)
{
    /*
     * sequence:
     * #
     * 0. current: active, next: empty >>>
     * 1.     current: copy-out, next: empty >>>
     * 2.         current: copy-out, next: copy-in >>>
     * 3.             copy from current page to the next page >>>
     * 4.                 current: copy-out, next: active >>>
     * 5.                     current: empty (erased), next: active
     *
     *
     *
     * seq#  copy_out_exists    copy_in_exists   active_exists   state#  handled
     *             -                  -                -           0        *
     * 1.          *                  -                -           1        *
     *             -                  *                -           2        -
     * 2.          *                  *                -           3        *
     * 0.          -                  -                *           4        *
     * 4.          *                  -                *           5        *
     *             -                  *                *           6        -
     *             *                  *                *           7        -
     */

    uint32_t active_index, copy_in_index, copy_out_index;
    uint8_t active_exists, copy_in_exists, copy_out_exists;

    switch (eeprom_find_page_by_state(flash_address, config, EEPROM_PAGE_ACTIVE, &active_index)) {
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

        default: {
            return EEPROM_RESULT_UNCATCHED_FAIL;
        }
    }

    switch (eeprom_find_page_by_state(flash_address, config, EEPROM_PAGE_COPY_IN, &copy_in_index)) {
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

        default: {
            return EEPROM_RESULT_UNCATCHED_FAIL;
        }
    }

    switch (eeprom_find_page_by_state(flash_address, config, EEPROM_PAGE_COPY_OUT, &copy_out_index)) {
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

        default: {
            return EEPROM_RESULT_UNCATCHED_FAIL;
        }
    }

    if (!copy_out_exists && !copy_in_exists && active_exists) {// state #4
        // all ok
        return EEPROM_RESULT_SUCCESS;
    } else if (!copy_out_exists && !copy_in_exists && !active_exists) { // state #0
        // no one known page found

        // format all
        eeprom_format(flash_address, config);

        // set it active
        switch (flash_write_word(flash_address, EEPROM_PAGE_ACTIVE)) {
            case FLASH_RESULT_SUCCESS: {
                return EEPROM_RESULT_SUCCESS;
            }

            case FLASH_RESULT_INVALID_ADDRESS: {
                return EEPROM_RESULT_INVALID_PARAMETERS;
            }

            case FLASH_RESULT_NEED_ERASE: {
                return EEPROM_RESULT_NEED_ERASE;
            }

            default: {
                return EEPROM_RESULT_UNCATCHED_FAIL;
            }
        }
    } else if (copy_out_exists && !active_exists) { // state #1, #3
        // !copy_in_exists = terminated between seq #1 and seq #2, copy-out is a source page;
        // copy_in_exists = terminated between seq #2 and seq #4;

        copy_in_index = eeprom_calc_next_page_index(flash_address, config, copy_out_index);

        // erase next page
        switch (flash_erase_page(get_page_address(flash_address, config, copy_in_index), config->words_on_page)) {
            case FLASH_RESULT_SUCCESS: {
                break;
            }

            case FLASH_RESULT_INVALID_ADDRESS: {
                return EEPROM_RESULT_INVALID_PARAMETERS;
            }

            default: {
                return EEPROM_RESULT_UNCATCHED_FAIL;
            }
        }

        // repeat copy
        config->active_page_index = copy_out_index;

        switch (eeprom_move_current_page(flash_address, config)) {
            case EEPROM_RESULT_SUCCESS: {
                return EEPROM_RESULT_SUCCESS;
            }

            case EEPROM_RESULT_READ_FAILED: {
                return EEPROM_RESULT_READ_FAILED;
            }

            case EEPROM_RESULT_NO_EMPTY_PAGE: {
                return EEPROM_RESULT_NO_EMPTY_PAGE;
            }

            default: {
                return EEPROM_RESULT_UNCATCHED_FAIL;
            }
        }
    } else if (copy_out_exists && active_exists && !copy_in_exists) { // state #5
        // terminated between 4 and 5

        // erase previous page
        switch (flash_erase_page(get_page_address(flash_address, config, copy_out_index), config->words_on_page)) {
            case FLASH_RESULT_SUCCESS: {
                return EEPROM_RESULT_SUCCESS;
            }

            case FLASH_RESULT_INVALID_ADDRESS: {
                return EEPROM_RESULT_INVALID_PARAMETERS;
            }

            default: {
                return EEPROM_RESULT_UNCATCHED_FAIL;
            }
        }
    }

    // unrecognized state, need format
    return EEPROM_RESULT_UNCATCHED_FAIL;
}

//void eeprom_print_debug(
//    uint32_t flash_address, t_eeprom_config *config,
//    uint32_t address, uint32_t size)
//{
//    uint32_t i, j, k;
//    uint8_t *data = (uint8_t *)address;
//
//    for (i = 0; i < size; i += 256) {
//        printf("Block at 0x%08x\n", address + i);
//
//        for (j = 0; j < 256; j += 16) {
//            printf("0x%08x: ", address + i + j);
//
//            for (k = 0; k < 16; k++) {
//                printf(" %02x", data[i + j + k]);
//            }
//
//            printf("\n");
//        }
//    }
//}

int eeprom_init(
    uint32_t flash_address, t_eeprom_config *config)
{
    uint32_t active_page_index;

    config->total_size = config->pages_count * config->words_on_page * sizeof(uint32_t);

    if (eeprom_check_state(flash_address, config) != EEPROM_RESULT_SUCCESS) {
        eeprom_format(flash_address, config);
    }

    if (eeprom_find_page_by_state(flash_address, config, EEPROM_PAGE_ACTIVE, &active_page_index) == EEPROM_RESULT_SUCCESS) {
        config->active_page_index = active_page_index;
        return EEPROM_RESULT_SUCCESS;
    }

    return EEPROM_RESULT_UNCATCHED_FAIL;
}
