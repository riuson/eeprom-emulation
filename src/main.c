/*
 ============================================================================
 Name        : eeprom-emulation.c
 Author      : riuson
 Version     :
 Copyright   : Chelyabinsk 2015
 Description : EEPROM Emulation on Flash
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "flash_abstraction.h"
#include "eeprom_emulation.h"

int main(void)
{
    uint16_t a, b;
    //uint32_t data[4] = { 1, 0x12345678, 0x87654321, 0x5555aaaa };
    flash_init_debug();

    //flash_write(0x2, 4, data);
    //flash_print_debug(0, 512);

    eeprom_init_debug(
        0,
        FLASH_PAGES_COUNT * FLASH_WORDS_ON_PAGE,
        FLASH_WORDS_ON_PAGE,
        FLASH_PAGES_COUNT);

    for (a = 0; a < 50; a++) {
        eeprom_write_value(a, 100 + a);
    }

    flash_print_debug(0, 1024);

    for (a = 0; a < 50; a++) {
        eeprom_write_value(a, 100 + a);
    }

    flash_print_debug(0, 1024);

    for (a = 0; a < 50; a++) {
        eeprom_write_value(a, 100 + a);
    }

    flash_print_debug(0, 1024);
    //eeprom_write_value(0x5678, 0xbbcc);
    //eeprom_read_value(0x1234, &a);
    //eeprom_read_value(0x5678, &b);
    //flash_print_debug(0, 512);

    return EXIT_SUCCESS;
}
