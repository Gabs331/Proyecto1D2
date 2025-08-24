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
// Inicializa el m�dulo I2C en modo Maestro
// Par�metros:
//  - SCL_Clock: frecuencia del reloj SCL deseada
//  - Prescaler: valor del prescaler (1, 4, 16, 64)
//-----------------------------------------------------------------------------
void I2C_Master_Init(unsigned long SCL_Clock, uint8_t Prescaler);

//-----------------------------------------------------------------------------
// Genera una condici�n de inicio (START) en la comunicaci�n I2C
//-----------------------------------------------------------------------------
void I2C_Master_Start(void);

//-----------------------------------------------------------------------------
// Genera una condici�n de parada (STOP) en la comunicaci�n I2C
//-----------------------------------------------------------------------------
void I2C_Master_Stop(void);

//-----------------------------------------------------------------------------
// Env�a un dato desde el Maestro al Esclavo
// Retorna 1 si el Esclavo respondi� con ACK, de lo contrario retorna el c�digo de estado
//-----------------------------------------------------------------------------
uint8_t I2C_Master_Write(uint8_t dato);

//-----------------------------------------------------------------------------
// Recibe un dato desde el Esclavo hacia el Maestro
// Par�metros:
//  - buffer: puntero donde se almacenar� el dato recibido
//  - ack: 1 para enviar ACK, 0 para enviar NACK
// Retorna 1 si la lectura fue exitosa, o el c�digo de estado en caso de error
//-----------------------------------------------------------------------------
uint8_t I2C_Master_Read(uint8_t *buffer, uint8_t ack);

//-----------------------------------------------------------------------------
// Inicializa el m�dulo I2C en modo Esclavo con una direcci�n espec�fica
//-----------------------------------------------------------------------------
void I2C_Slave_Init(uint8_t address);

#endif /* I2C_H_ */

