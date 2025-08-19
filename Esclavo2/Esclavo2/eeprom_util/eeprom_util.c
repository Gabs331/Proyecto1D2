/*
 * eeprom_util.c
 *
 * Created: 18/08/2025 13:48:04
 *  Author: Rodrigo Lara
 */ 

// eeprom_util.c
#include "eeprom_util.h"

void EEPROM_write_long(uint16_t addr, long value) {
	eeprom_update_block((const void*)&value, (void*)addr, sizeof(long));
}

long EEPROM_read_long(uint16_t addr) {
	long value;
	eeprom_read_block((void*)&value, (const void*)addr, sizeof(long));
	return value;
}
