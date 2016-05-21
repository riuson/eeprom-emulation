#include <stdint.h>

#ifdef WRAPPER_EXPORTS
#define WRAPPER_API __declspec(dllexport)
#else
#define WRAPPER_API __declspec(dllimport)
#endif

extern "C" WRAPPER_API int LIB_eeprom_init_debug(uint32_t flash_address, uint32_t words_on_page, uint32_t pages_count);
extern "C" WRAPPER_API int LIB_eeprom_read_value(uint16_t key, uint16_t *value);
extern "C" WRAPPER_API int LIB_eeprom_write_value(uint16_t key, uint16_t value);
extern "C" WRAPPER_API int LIB_eeprom_keys_count(uint16_t *count);
