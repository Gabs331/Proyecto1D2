/*
 * i2c.c
 *
 * Created: 18/08/2025 13:49:14
 *  Author: Rodrigo Lara
 */ 

// i2c.c
#include "i2c.h"

void I2C_init(void) {
	TWSR = 0x00;
	TWBR = 72; // 100kHz @ 16MHz
}

void I2C_start(uint8_t address) {
	TWCR = (1<<TWSTA)|(1<<TWEN)|(1<<TWINT);
	while (!(TWCR & (1<<TWINT)));
	TWDR = address;
	TWCR = (1<<TWEN)|(1<<TWINT);
	while (!(TWCR & (1<<TWINT)));
}

void I2C_write(uint8_t data) {
	TWDR = data;
	TWCR = (1<<TWEN)|(1<<TWINT);
	while (!(TWCR & (1<<TWINT)));
}

void I2C_stop(void) {
	TWCR = (1<<TWSTO)|(1<<TWINT)|(1<<TWEN);
}
