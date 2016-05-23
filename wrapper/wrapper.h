#include <stdint.h>
#include "../src/eeprom_emulation.h"

#ifdef WRAPPER_EXPORTS
#define WRAPPER_API __declspec(dllexport)
#else
#define WRAPPER_API __declspec(dllimport)
#endif

extern "C" WRAPPER_API
int LIB_eeprom_init(
    uint32_t flash_address, t_eeprom_config *config);

extern "C" WRAPPER_API
int LIB_eeprom_read_value(
    uint32_t flash_address, t_eeprom_config *config,
    uint16_t key, uint16_t *value);

extern "C" WRAPPER_API
int LIB_eeprom_write_value(
    uint32_t flash_address, t_eeprom_config *config,
    uint16_t key, uint16_t value);

extern "C" WRAPPER_API
int LIB_eeprom_keys_count(
    uint32_t flash_address, t_eeprom_config *config,
    uint16_t *count);

extern "C" WRAPPER_API
int LIB_eeprom_read_by_index(
    uint32_t flash_address, t_eeprom_config *config,
    uint16_t index, uint16_t *key, uint16_t *value);

extern "C" WRAPPER_API
int LIB_eeprom_low_read_word(
    uint32_t flash_address, t_eeprom_config *config,
    uint32_t page_index, uint32_t cell_index, uint32_t *value);

extern "C" WRAPPER_API
int LIB_eeprom_low_write_word(
    uint32_t flash_address, t_eeprom_config *config,
    uint32_t page_index, uint32_t cell_index, uint32_t value);

extern "C" WRAPPER_API
int LIB_eeprom_low_erase_page(
    uint32_t flash_address, t_eeprom_config *config,
    uint32_t page_index);

extern "C" WRAPPER_API
int LIB_eeprom_low_can_overwrite(uint32_t value_old, uint32_t value_new);
