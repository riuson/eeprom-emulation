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
