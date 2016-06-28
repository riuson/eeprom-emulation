/*
 * eeprom_emulation.c
 *
 *  Created on: 07 апр. 2015 г.
 *      Author: vladimir
 */

#include "eeprom_emulation.h"
#include "flash_abstraction.h"

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
    uint32_t result = flash_address + page_index * config->words_on_page * sizeof(uint32_t);
    return result;
}

static int convert_flash_result(uint32_t source)
{
    switch (source) {
        case FLASH_RESULT_SUCCESS: {
            return EEPROM_RESULT_SUCCESS;
        }

        case FLASH_RESULT_NEED_ERASE: {
            return EEPROM_RESULT_NEED_ERASE;
        }

        case FLASH_RESULT_INVALID_ADDRESS: {
            return EEPROM_RESULT_INVALID_PARAMETERS;
        }

        default: {
            return EEPROM_RESULT_UNCATCHED_FAIL;
        }
    }
}

int eeprom_low_read_word(
    uint32_t flash_address, t_eeprom_config *config,
    uint32_t page_index, uint32_t cell_index, uint32_t *value)
{
    uint32_t address = get_page_address(flash_address, config, page_index) + cell_index * sizeof(uint32_t);
    return convert_flash_result(flash_read_word(address, value));
}

int eeprom_low_write_word(
    uint32_t flash_address, t_eeprom_config *config,
    uint32_t page_index, uint32_t cell_index, uint32_t value)
{
    uint32_t address = get_page_address(flash_address, config, page_index) + cell_index * sizeof(uint32_t);
    return convert_flash_result(flash_write_word(address, value));
}

int eeprom_low_erase_page(
    uint32_t flash_address, t_eeprom_config *config,
    uint32_t page_index)
{
    uint32_t address = get_page_address(flash_address, config, page_index);
    return convert_flash_result(flash_erase_page(address, config->words_on_page));
}

int eeprom_low_can_overwrite(uint32_t value_old, uint32_t value_new)
{
    return convert_flash_result(flash_can_overwrite(value_old, value_new));
}

static int eeprom_find_page_by_state(
    uint32_t flash_address, t_eeprom_config *config,
    uint32_t state, uint32_t *page_index)
{
    uint32_t page_header, page;
    uint32_t result;

    for (page = 0; page < config->pages_count; page++) {
        if ((result = eeprom_low_read_word(flash_address, config, page, 0, &page_header)) != EEPROM_RESULT_SUCCESS) {
            return result;
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
    uint32_t result;

    // index of the next page
    next_page_index = eeprom_calc_next_page_index(flash_address, config, start_page_index);

    // read page header
    if ((result = eeprom_low_read_word(flash_address, config, next_page_index, 0, &page_header)) != EEPROM_RESULT_SUCCESS) {
        return result;
    }

    if ((page_header & EEPROM_KEY_MASK) == EEPROM_PAGE_EMPTY) {
        *page_index = next_page_index;
        return EEPROM_RESULT_SUCCESS;
    }

    *page_index = 0;
    return EEPROM_RESULT_NO_EMPTY_PAGE;
}

static int eeprom_can_compact(
    uint32_t flash_address, t_eeprom_config *config,
    uint32_t page_index)
{
    uint32_t offset1, offset2, stored, search_key;
    uint32_t result;
    offset1 = config->words_on_page - 1;

    // Loop external from page end to start
    while (1) {
        if ((result = eeprom_low_read_word(flash_address, config, page_index, offset1, &stored)) != EEPROM_RESULT_SUCCESS) {
            return result;
        }

        if (offset1 <= 1) {
            break;
        }

        if ((stored & EEPROM_KEY_MASK) != EEPROM_PAGE_EMPTY) {
            search_key = (stored >> 16) & 0x0000ffff;
            offset2 = offset1 - 1;

            // Loop internal from external to start
            while (1) {
                if ((result = eeprom_low_read_word(flash_address, config, page_index, offset2, &stored)) != EEPROM_RESULT_SUCCESS) {
                    return result;
                }

                if (search_key == ((stored >> 16) & 0x0000ffff)) {
                    return EEPROM_RESULT_SUCCESS;
                }

                if (offset2 <= 1) {
                    break;
                }

                offset2--;
            }
        }

        offset1--;
    }

    return EEPROM_RESULT_NO_EMPTY_RECORD;
}

static int eeprom_find_key_from_end(
    uint32_t flash_address, t_eeprom_config *config,
    uint32_t page_index, uint16_t key, uint16_t *value, uint32_t *valueShift)
{
    uint32_t offset, stored, search_key;
    uint32_t result;

    offset = config->words_on_page - 1;
    search_key = key << 16;

    while (1) {
        if ((result = eeprom_low_read_word(flash_address, config, page_index, offset, &stored)) != EEPROM_RESULT_SUCCESS) {
            return result;
        }

        if ((stored & EEPROM_KEY_MASK) == search_key) {
            *value = (stored & 0x0000ffff);
            *valueShift = offset;
            return EEPROM_RESULT_SUCCESS;
        }

        if (offset <= 1) {
            break;
        }

        offset--;
    }

    *value = 0;
    *valueShift = 0;
    return EEPROM_RESULT_KEY_NOT_FOUND;
}

static int eeprom_store_value(
    uint32_t flash_address, t_eeprom_config *config,
    uint32_t page_index, uint16_t key, uint16_t value)
{
    uint32_t offset = config->words_on_page - 1, stored, same = 0xffffffff, same_offset = 0xffffffff;
    uint32_t to_store = (key << 16) | value;
    uint16_t value_readed;
    uint32_t result;

    // check for overwrite possibility on existing same record
    switch (eeprom_find_key_from_end(flash_address, config, page_index, key, &value_readed, &same_offset)) {
        case EEPROM_RESULT_SUCCESS: {
            same = (key << 16) | value_readed;

            // if same value, nothing to do
            if (same == to_store) {
                return EEPROM_RESULT_SUCCESS;
            }

            // overwrite if can
            if ((result = eeprom_low_can_overwrite(same, to_store)) == EEPROM_RESULT_SUCCESS) {
                return eeprom_low_write_word(flash_address, config, page_index, same_offset, to_store);
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

    for (offset = 1; offset < config->words_on_page; offset++) {
        if ((result = eeprom_low_read_word(flash_address, config, page_index, offset, &stored)) != EEPROM_RESULT_SUCCESS) {
            return result;
        }

        // if empty record was found, save key and value
        if ((stored & EEPROM_KEY_MASK) == EEPROM_PAGE_EMPTY) {
            return eeprom_low_write_word(flash_address, config, page_index, offset, to_store);
        }
    }

    return EEPROM_RESULT_NO_EMPTY_RECORD;
}

static int eeprom_move_current_page(
    uint32_t flash_address, t_eeprom_config *config)
{
    uint32_t active_page_index, next_page_index;
    uint32_t offset, stored, stored_offset;
    uint16_t stored_key, stored_value;
    uint32_t result;

    active_page_index = config->active_page_index;

    // where no reason to move page, what cannot be compacted;
    // so check possibility
    if ((result = eeprom_can_compact(flash_address, config, active_page_index)) != EEPROM_RESULT_SUCCESS) {
        return result;
    }

    // find room to new page
    if ((result = eeprom_find_next_empty_page_index(flash_address, config, &next_page_index)) != EEPROM_RESULT_SUCCESS) {
        return result;
    }

    // current page to copy-out state
    if ((result = eeprom_low_write_word(flash_address, config, active_page_index, 0, EEPROM_PAGE_COPY_OUT)) != EEPROM_RESULT_SUCCESS) {
        return result;
    }

    // next page to copy-in state
    if ((result = eeprom_low_write_word(flash_address, config, next_page_index, 0, EEPROM_PAGE_COPY_IN)) != EEPROM_RESULT_SUCCESS) {
        return result;
    }

    // from end to start
    offset = config->words_on_page - 1;

    // iterate records
    while (1) {
        if ((result = eeprom_low_read_word(flash_address, config, active_page_index, offset, &stored)) != FLASH_RESULT_SUCCESS) {
            return result;
        }

        if ((stored & EEPROM_KEY_MASK) != EEPROM_PAGE_EMPTY) {
            // get key and value
            stored_key = (stored >> 16) & 0x0000ffff;

            // find this key on the next page
            // if not found
            if (eeprom_find_key_from_end(flash_address, config, next_page_index, stored_key, &stored_value, &stored_offset) == EEPROM_RESULT_KEY_NOT_FOUND) {
                // copy from active to next
                stored_value = stored & 0x0000ffff;

                if ((result = eeprom_store_value(flash_address, config, next_page_index, stored_key, stored_value)) != EEPROM_RESULT_SUCCESS) {
                    return result;
                }
            }

            // if found - skip
        }

        if (offset <= 1) {
            break;
        }

        offset--;

    }

    // next page to active state
    if ((result = eeprom_low_write_word(flash_address, config, next_page_index, 0, EEPROM_PAGE_ACTIVE)) != EEPROM_RESULT_SUCCESS) {
        return result;
    }

    config->active_page_index = next_page_index;

    if ((result = eeprom_low_erase_page(flash_address, config, active_page_index)) != EEPROM_RESULT_SUCCESS) {
        return result;
    }

    return EEPROM_RESULT_SUCCESS;
}

int eeprom_read_value(
    uint32_t flash_address, t_eeprom_config *config,
    uint16_t key, uint16_t *value)
{
    uint32_t offset;
    return (eeprom_find_key_from_end(flash_address, config, config->active_page_index, key, value, &offset));
}

int eeprom_write_value(
    uint32_t flash_address, t_eeprom_config *config,
    uint16_t key, uint16_t value)
{
    uint32_t result;

    // try store valye
    switch (result = eeprom_store_value(flash_address, config, config->active_page_index, key, value)) {
        case EEPROM_RESULT_NO_EMPTY_RECORD: {
            // move page
            switch (result = eeprom_move_current_page(flash_address, config)) {
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

                default: {
                    return result;
                }
            }
        }

        default: {
            return result;
        }
    }
}

int eeprom_keys_count(
    uint32_t flash_address, t_eeprom_config *config,
    uint16_t *count)
{
    uint32_t offset1, offset2, stored, search_key;
    const uint32_t page_index = config->active_page_index;
    uint32_t result;
    offset1 = config->words_on_page - 1;
    *count = 0;

    // Loop external from page end to start
    while (1) {
        if ((result = eeprom_low_read_word(flash_address, config, page_index, offset1, &stored)) != EEPROM_RESULT_SUCCESS) {
            return result;
        }


        if ((stored & EEPROM_KEY_MASK) != EEPROM_PAGE_EMPTY) {
            (*count)++;
            search_key = (stored >> 16) & 0x0000ffff;
            offset2 = offset1 - 1;

            // Loop internal from external to start
            while (offset1 > 1) {
                if ((result = eeprom_low_read_word(flash_address, config, page_index, offset2, &stored)) != EEPROM_RESULT_SUCCESS) {
                    return result;
                }

                if (search_key == ((stored >> 16) & 0x0000ffff)) {
                    (*count)--;
                    break;
                }

                if (offset2 <= 1) {
                    break;
                }

                offset2--;
            }
        }

        if (offset1 <= 1) {
            break;
        }

        offset1--;
    }

    return EEPROM_RESULT_SUCCESS;
}

int eeprom_read_by_index(
    uint32_t flash_address, t_eeprom_config *config,
    uint16_t index, uint16_t *key, uint16_t *value)
{
    uint32_t offset1, offset2, stored, search_key;
    const uint32_t page_index = config->active_page_index;
    uint16_t count = 0;
    uint32_t result;
    offset1 = config->words_on_page - 1;

    // Loop external from page end to start
    while (1) {
        if ((result = eeprom_low_read_word(flash_address, config, page_index, offset1, &stored)) != EEPROM_RESULT_SUCCESS) {
            return result;
        }

        if ((stored & EEPROM_KEY_MASK) != EEPROM_PAGE_EMPTY) {
            count++;
            search_key = (stored >> 16) & 0x0000ffff;
            offset2 = offset1 - 1;

            if (count == index + 1) {
                *key = (stored >> 16) & 0x0000ffff;
                *value = stored & 0x0000ffff;
                return EEPROM_RESULT_SUCCESS;
            }

            // Loop internal from external to start
            while (offset1 > 1) {
                if ((result = eeprom_low_read_word(flash_address, config, page_index, offset2, &stored)) != EEPROM_RESULT_SUCCESS) {
                    return result;
                }

                if (search_key == ((stored >> 16) & 0x0000ffff)) {
                    count--;
                    break;
                }

                if (offset2 <= 1) {
                    break;
                }

                offset2--;
            }
        }

        if (offset1 <= 1) {
            break;
        }

        offset1--;
    }

    return EEPROM_RESULT_KEY_NOT_FOUND;
}

static int eeprom_format(
    uint32_t flash_address, t_eeprom_config *config)
{
    uint32_t offset, value, page_index;
    uint32_t result;

    // erase
    for (page_index = 0; page_index < config->pages_count; page_index++) {
        eeprom_low_erase_page(flash_address, config, page_index);
    }

    // check
    for (page_index = 0; page_index < config->pages_count; page_index++) {
        for (offset = 0; offset < config->words_on_page; offset++) {
            if ((result = eeprom_low_read_word(flash_address, config, page_index, offset, &value)) == EEPROM_RESULT_SUCCESS) {
                return result;
            }

            if (value != 0xffffffff) {
                return EEPROM_RESULT_NEED_ERASE;
            }
        }
    }

    // set first page active
    return eeprom_low_write_word(flash_address, config, 0, 0, EEPROM_PAGE_ACTIVE);
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
    uint32_t result;

    switch (result = eeprom_find_page_by_state(flash_address, config, EEPROM_PAGE_ACTIVE, &active_index)) {
        case EEPROM_RESULT_SUCCESS: {
            active_exists = 1;
            break;
        }

        case EEPROM_RESULT_KEY_NOT_FOUND: {
            active_exists = 0;
            break;
        }

        default: {
            return result;
        }
    }

    switch (result = eeprom_find_page_by_state(flash_address, config, EEPROM_PAGE_COPY_IN, &copy_in_index)) {
        case EEPROM_RESULT_SUCCESS: {
            copy_in_exists = 1;
            break;
        }

        case EEPROM_RESULT_KEY_NOT_FOUND: {
            copy_in_exists = 0;
            break;
        }

        default: {
            return result;
        }
    }

    switch (result = eeprom_find_page_by_state(flash_address, config, EEPROM_PAGE_COPY_OUT, &copy_out_index)) {
        case EEPROM_RESULT_SUCCESS: {
            copy_out_exists = 1;
            break;
        }

        case EEPROM_RESULT_KEY_NOT_FOUND: {
            copy_out_exists = 0;
            break;
        }

        default: {
            return result;
        }
    }

    if (!copy_out_exists && !copy_in_exists && active_exists) {// state #4
        // all ok
        return EEPROM_RESULT_SUCCESS;
    } else if (!copy_out_exists && !copy_in_exists && !active_exists) { // state #0
        // no one known page found

        // format all
        if ((result = eeprom_format(flash_address, config)) != EEPROM_RESULT_SUCCESS) {
            return result;
        }

        // set it active
        return eeprom_low_write_word(flash_address, config, 0, 0, EEPROM_PAGE_ACTIVE);
    } else if (copy_out_exists && !active_exists) { // state #1, #3
        // !copy_in_exists = terminated between seq #1 and seq #2, copy-out is a source page;
        // copy_in_exists = terminated between seq #2 and seq #4;

        copy_in_index = eeprom_calc_next_page_index(flash_address, config, copy_out_index);

        // erase next page
        if ((result = eeprom_low_erase_page(flash_address, config, copy_in_index)) != EEPROM_RESULT_SUCCESS) {
            return result;
        }

        // repeat copy
        config->active_page_index = copy_out_index;

        return eeprom_move_current_page(flash_address, config);
    } else if (copy_out_exists && active_exists && !copy_in_exists) { // state #5
        // terminated between 4 and 5

        // erase previous page
        return eeprom_low_erase_page(flash_address, config, copy_out_index);
    }

    // unrecognized state, need format
    return EEPROM_RESULT_UNCATCHED_FAIL;
}

int eeprom_init(
    uint32_t flash_address, t_eeprom_config *config)
{
    uint32_t active_page_index;
    uint32_t result;

    config->total_size = config->pages_count * config->words_on_page * sizeof(uint32_t);

    if (eeprom_check_state(flash_address, config) != EEPROM_RESULT_SUCCESS) {

        if ((result = eeprom_format(flash_address, config)) != EEPROM_RESULT_SUCCESS) {
            return result;
        }
    }

    if ((result = eeprom_find_page_by_state(flash_address, config, EEPROM_PAGE_ACTIVE, &active_page_index)) == EEPROM_RESULT_SUCCESS) {
        config->active_page_index = active_page_index;
        return EEPROM_RESULT_SUCCESS;
    } else {
        return result;
    }
}
