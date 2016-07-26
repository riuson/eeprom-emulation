// dllmain.cpp: определяет точку входа для приложения DLL.
#include "stdafx.h"
#include "..\src\eeprom_emulation.h"

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}

void demo_c_code(void)
{
	uint16_t key = 0x1234;
	uint16_t value = 0u;

	// Configuration structure
	t_eeprom_config configuration;

	// Address of eeprom emulation area in flash
	const uint32_t flash_address = 0x10000000;

	// Area size
	configuration.pages_count = 10u;
	configuration.words_on_page = 128u;

	// Initialize
	eeprom_init(flash_address, &configuration);

	// Read
	if (eeprom_read_value(flash_address, &configuration, key, &value) == EEPROM_RESULT_SUCCESS) {
		// value - contains readed value
	}

	// Write
	if (eeprom_write_value(flash_address, &configuration, key, value) == EEPROM_RESULT_SUCCESS) {
		// ...
	}
}
