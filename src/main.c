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

static uint32_t memory_array[128 * 8];

int main(void)
{
    uint16_t a, b;
    //uint32_t data[4] = { 1, 0x12345678, 0x87654321, 0x5555aaaa };

    //flash_write(0x2, 4, data);
    //flash_print_debug(0, 512);

    eeprom_init_debug(
        memory_array,
        512 / 4,
        8);

    for (a = 0; a < 50; a++) {
        eeprom_write_value(a, 100 + a);
    }

    eeprom_print_debug(memory_array, 1024);

    for (a = 0; a < 50; a++) {
        eeprom_write_value(a, 103 + a);
    }

    eeprom_print_debug(memory_array, 1024);

    for (a = 0; a < 50; a++) {
        eeprom_write_value(a, 105 + a);
    }

    eeprom_print_debug(memory_array, 1024);
    //eeprom_write_value(0x5678, 0xbbcc);
    //eeprom_read_value(0x1234, &a);
    //eeprom_read_value(0x5678, &b);
    //flash_print_debug(0, 512);

    eeprom_write_value(0, 1);
    eeprom_write_value(0, 2);
    eeprom_write_value(0, 3);
    eeprom_write_value(0, 4);
    eeprom_write_value(0, 5);
    eeprom_write_value(0, 5);
    eeprom_write_value(0, 5);
    eeprom_write_value(0, 6);
    eeprom_write_value(0, 3);

    eeprom_print_debug(memory_array, 1024);

    eeprom_write_value(0x78, 0xf8);
    eeprom_print_debug(memory_array, 1024);

    eeprom_write_value(0x78, 0xe0);
    eeprom_print_debug(memory_array, 1024);

    return EXIT_SUCCESS;
}
