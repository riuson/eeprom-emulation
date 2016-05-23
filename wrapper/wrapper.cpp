#include "stdafx.h"
#include "wrapper.h"

WRAPPER_API int LIB_eeprom_init(
    uint32_t flash_address, t_eeprom_config *config)
{
    return eeprom_init(flash_address, config);
}

WRAPPER_API int LIB_eeprom_read_value(
    uint32_t flash_address, t_eeprom_config *config,
    uint16_t key, uint16_t *value)
{
    return eeprom_read_value(flash_address, config, key, value);
}

WRAPPER_API int LIB_eeprom_write_value(
    uint32_t flash_address, t_eeprom_config *config,
    uint16_t key, uint16_t value)
{
    return eeprom_write_value(flash_address, config, key, value);
}

WRAPPER_API int LIB_eeprom_keys_count(
    uint32_t flash_address, t_eeprom_config *config,
    uint16_t *count)
{
    return eeprom_keys_count(flash_address, config, count);
}

WRAPPER_API int LIB_eeprom_read_by_index(
    uint32_t flash_address, t_eeprom_config *config,
    uint16_t index, uint16_t *key, uint16_t *value)
{
    return eeprom_read_by_index(flash_address, config, index, key, value);
}

WRAPPER_API int LIB_eeprom_low_read_word(
    uint32_t flash_address, t_eeprom_config *config,
    uint32_t page_index, uint32_t cell_index, uint32_t *value)
{
    return eeprom_low_read_word(flash_address, config, page_index, cell_index, value);
}

WRAPPER_API int LIB_eeprom_low_write_word(
    uint32_t flash_address, t_eeprom_config *config,
    uint32_t page_index, uint32_t cell_index, uint32_t value)
{
    return eeprom_low_write_word(flash_address, config, page_index, cell_index, value);
}

WRAPPER_API int LIB_eeprom_low_erase_page(
    uint32_t flash_address, t_eeprom_config *config,
    uint32_t page_index)
{
    return eeprom_low_erase_page(flash_address, config, page_index);
}

WRAPPER_API int LIB_eeprom_low_can_overwrite(uint32_t value_old, uint32_t value_new)
{
    return eeprom_low_can_overwrite(value_old, value_new);
}
