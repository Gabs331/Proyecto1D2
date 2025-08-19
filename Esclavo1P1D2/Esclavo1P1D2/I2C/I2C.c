/*
 * I2CS.c
 *
 * Created: 1/08/2025 09:06:08
 *  Author: pyocm
 */ 


#include "I2C.h"

//***************************************************************
// Inicializa el módulo I2C en modo Maestro
//***************************************************************
void I2C_Master_Init(unsigned long SCL_Clock, uint8_t Prescaler) {

    DDRC &= ~((1 << DDC4) | (1 << DDC5));  // Configura los pines SDA y SCL como entradas

    switch (Prescaler) {
        case 1:
            TWSR &= ~((1 << TWPS1) | (1 << TWPS0));
            break;
        case 4:
            TWSR &= ~(1 << TWPS1);
            TWSR |= (1 << TWPS0);
            break;
        case 16:
            TWSR &= ~(1 << TWPS0);
            TWSR |= (1 << TWPS1);
            break;
        case 64:
            TWSR |= (1 << TWPS1) | (1 << TWPS0);
            break;
		default:
			TWSR &= ~((1 << TWPS1) | (1 << TWPS0)); // Valor por defecto si no coincide
			Prescaler = 1;
			break;
    }

	TWBR = (((16000000) / SCL_Clock) - 16) / (2 * Prescaler); // Cálculo del registro de velocidad. Se recomienda >10
	TWCR |= (1 << TWEN); // Habilita el módulo TWI
}

//************************************************************************
// Genera la condición de inicio (START) en la comunicación I2C
//************************************************************************
void I2C_Master_Start(void){
    uint8_t estado;
    
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); // Solicita START
    while (!(TWCR & (1 << TWINT))); // Espera a que se complete la operación
}

//************************************************************************
// Genera la condición de parada (STOP) en la comunicación I2C
//************************************************************************
void I2C_Master_Stop(void){
    TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWSTO); // Envia STOP para finalizar transmisión
}

//************************************************************************
// Envía un dato desde el maestro al esclavo
// Retorna 1 si el esclavo respondió con ACK, o el estado si hubo error
//************************************************************************
uint8_t I2C_Master_Write(uint8_t dato){
    uint8_t estado;

    TWDR = dato;  // Carga el dato a enviar
    TWCR = (1 << TWEN) | (1 << TWINT); // Inicia transmisión

    while (!(TWCR & (1 << TWINT))); // Espera a que termine

    estado = TWSR & 0xF8; // Lee el estado de TWI

    // Comprueba si se recibió ACK tras SLA+W, SLA+R o DATO
    if (estado == 0x18 || estado == 0x28 || estado == 0x40){
        return 1; // OK
    } else {
        return estado; // Error
    }
}

//************************************************************************
// Lee un dato enviado por el esclavo al maestro
// Parámetro 'ack': 1 para enviar ACK después de recibir, 0 para NACK
//************************************************************************
uint8_t I2C_Master_Read(uint8_t *buffer, uint8_t ack){
    uint8_t estado;
    
    if (ack){
        TWCR |= (1 << TWEA);  // Configura para enviar ACK
    } else {
        TWCR &= ~(1 << TWEA); // Configura para no enviar ACK
    }

    TWCR |= (1 << TWINT); // Inicia recepción
    while (!(TWCR & (1 << TWINT))); // Espera a que finalice

    estado = TWSR & 0xF8; // Lee el estado actual

    // Verifica si se recibió el dato correctamente
    if (estado == 0x58 || estado == 0x50){
        *buffer = TWDR; // Guarda el dato recibido
        return 1; // OK
    } else {
        return estado; // Error
    }
}

//*****************************************************************************
// Inicializa el módulo I2C en modo Esclavo
//*****************************************************************************
void I2C_Slave_Init(uint8_t address) {
    DDRC &= ~((1 << DDC4) | (1 << DDC5));  // Configura SDA y SCL como entradas

    TWAR = address << 1;  // Asigna la dirección del esclavo (bit 0 es para lectura/escritura)

    // Habilita ACK automático, el módulo TWI y su interrupción
    TWCR = (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
}
