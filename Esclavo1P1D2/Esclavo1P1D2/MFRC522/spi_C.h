/*
 * spi_C.h
 *
 * Created: 8/08/2025 11:06:43
 *  Author: pyocm
 */ 


#ifndef SPI_C_H_
#define SPI_C_H_

#include <avr/io.h>

/*
 * Set to 1, spi api will work in master mode
 * else in slave mode
 */
#define SPI_CONFIG_AS_MASTER 	1


/*
 * Config SPI pin diagram
 */
#define SPI_DDR		DDRB
#define SPI_PORT	PORTB
#define SPI_PIN		PINB
#define SPI_MOSI	PB3
#define SPI_MISO	PB4
#define SPI_SS		PB2
#define SPI_SCK		PB5




#endif /* SPI_C_H_ */