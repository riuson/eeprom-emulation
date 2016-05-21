#include "stdafx.h"
#include "wrapper.h"
#include "../src/eeprom_emulation.h"

WRAPPER_API int LIB_eeprom_init_debug(uint32_t flash_address, uint32_t words_on_page, uint32_t pages_count)
{
    return eeprom_init_debug(flash_address, words_on_page, pages_count);
}

WRAPPER_API int LIB_eeprom_read_value(uint16_t key, uint16_t *value)
{
    return eeprom_read_value(key, value);
}

WRAPPER_API int LIB_eeprom_write_value(uint16_t key, uint16_t value)
{
    return eeprom_write_value(key, value);
}

WRAPPER_API int LIB_eeprom_keys_count(uint16_t *count)
{
    return eeprom_keys_count(count);
}
