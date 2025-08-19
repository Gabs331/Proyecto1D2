/*
 * twi_slave.h
 *
 * Created: 18/08/2025 14:55:54
 *  Author: Rodrigo Lara
 */ 

#ifndef TWI_SLAVE_H
#define TWI_SLAVE_H

#include <avr/io.h>

void TWI_SlaveInit(uint8_t address);
void TWI_SlaveTransmit(uint8_t data);
uint8_t TWI_SlaveDataReady(void);
uint8_t TWI_SlaveReceive(void);

#endif
