/*
 * twi_slave.c
 *
 * Created: 18/08/2025 14:58:08
 *  Author: Rodrigo Lara
 */ 


#include "twi_slave.h"
#include <avr/interrupt.h>

volatile uint8_t twi_received_data;
volatile uint8_t twi_data_ready = 0;

void TWI_SlaveInit(uint8_t address) {
	TWAR = (address << 1);       // Dirección esclavo
	TWCR = (1 << TWEN) |         // Habilitar TWI
	(1 << TWEA) |         // Responder ACK
	(1 << TWINT) |
	(1 << TWIE);          // Habilitar interrupción
	sei();                       // Habilitar interrupciones globales
}

void TWI_SlaveTransmit(uint8_t data) {
	TWDR = data;                 // Cargar dato a transmitir
	TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
}

uint8_t TWI_SlaveDataReady(void) {
	return twi_data_ready;
}

uint8_t TWI_SlaveReceive(void) {
	twi_data_ready = 0;
	return twi_received_data;
}

ISR(TWI_vect) {
	switch (TWSR & 0xF8) {
		case 0x60:  // SLA+W recibido
		case 0x68:
		TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
		break;

		case 0x80:  // Datos recibidos de Maestro
		twi_received_data = TWDR;
		twi_data_ready = 1;
		TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
		break;

		case 0xA8:  // SLA+R recibido ? Maestro quiere leer
		case 0xB0:
		TWDR = 0x00;  // Default, lo sobrescribimos en main
		TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
		break;

		case 0xB8:  // Maestro leyó un dato, enviar otro
		TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
		break;

		case 0xC0:  // Maestro terminó lectura
		case 0xC8:
		TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
		break;

		default:
		TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
		break;
	}
}
