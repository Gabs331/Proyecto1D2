/*
 * eeprom_util.h
 *
 * Created: 18/08/2025 13:48:34
 *  Author: Rodrigo Lara
 */ 


// eeprom_util.h
#ifndef EEPROM_UTIL_H
#define EEPROM_UTIL_H
#include <avr/eeprom.h>

void EEPROM_write_long(uint16_t addr, long value);
long EEPROM_read_long(uint16_t addr);

#endif
