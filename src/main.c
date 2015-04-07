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

int main(void)
{
    uint32_t data[4] = { 1, 0x12345678, 0x87654321, 0x5555aaaa };
    flash_init_debug();

    flash_write(0x2, 4, data);
    flash_print_debug(0, 512);

    return EXIT_SUCCESS;
}
