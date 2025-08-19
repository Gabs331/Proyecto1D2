/*
 * spi.h
 *
 * Created: 8/08/2025 10:56:18
 *  Author: pyocm
 */ 


#ifndef SPI_H_
#define SPI_H_


#include "spi_C.h"
#include "stdint.h"

void spi_init();
uint8_t spi_transmit(uint8_t data);

#define ENABLE_CHIP() (SPI_PORT &= (~(1<<SPI_SS)))
#define DISABLE_CHIP() (SPI_PORT |= (1<<SPI_SS))




#endif /* SPI_H_ */