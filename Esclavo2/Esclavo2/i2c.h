/*
 * i2c.h
 *
 * Created: 18/08/2025 13:49:26
 *  Author: Rodrigo Lara
 */ 

// i2c.h
#ifndef I2C_H
#define I2C_H

#include <avr/io.h>

void I2C_init(void);
void I2C_start(uint8_t address);
void I2C_write(uint8_t data);
void I2C_stop(void);

#endif
