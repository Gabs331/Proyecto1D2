/*
 * I2C.h
 *
 * Created: 1/08/2025 09:06:33
 * Author: pyocm
 */

#ifndef I2C_H_
#define I2C_H_

#include <avr/io.h>
#include <stdint.h>

//-----------------------------------------------------------------------------
// Inicializa el módulo I2C en modo Maestro
// Parámetros:
//  - SCL_Clock: frecuencia del reloj SCL deseada
//  - Prescaler: valor del prescaler (1, 4, 16, 64)
//-----------------------------------------------------------------------------
void I2C_Master_Init(unsigned long SCL_Clock, uint8_t Prescaler);

//-----------------------------------------------------------------------------
// Genera una condición de inicio (START) en la comunicación I2C
//-----------------------------------------------------------------------------
void I2C_Master_Start(void);

//-----------------------------------------------------------------------------
// Genera una condición de parada (STOP) en la comunicación I2C
//-----------------------------------------------------------------------------
void I2C_Master_Stop(void);

//-----------------------------------------------------------------------------
// Envía un dato desde el Maestro al Esclavo
// Retorna 1 si el Esclavo respondió con ACK, de lo contrario retorna el código de estado
//-----------------------------------------------------------------------------
uint8_t I2C_Master_Write(uint8_t dato);

//-----------------------------------------------------------------------------
// Recibe un dato desde el Esclavo hacia el Maestro
// Parámetros:
//  - buffer: puntero donde se almacenará el dato recibido
//  - ack: 1 para enviar ACK, 0 para enviar NACK
// Retorna 1 si la lectura fue exitosa, o el código de estado en caso de error
//-----------------------------------------------------------------------------
uint8_t I2C_Master_Read(uint8_t *buffer, uint8_t ack);

//-----------------------------------------------------------------------------
// Inicializa el módulo I2C en modo Esclavo con una dirección específica
//-----------------------------------------------------------------------------
void I2C_Slave_Init(uint8_t address);

#endif /* I2C_H_ */

