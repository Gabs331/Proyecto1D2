/*
 * LCD_8b.h
 *
 * Created: 18/07/2025 20:44:02
 * Author: pyocm
 */

#ifndef LCD_8B_H_
#define LCD_8B_H_

#define F_CPU 16000000
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

// Inicializa el LCD de 16x2 en modo de 8 bits
void initLCD8(void);

// Envía un valor al puerto conectado al LCD (RS, E, D7–D0)
void LCD8_PORT(uint16_t valor);

// Envía un comando al LCD
void LCD8_CMD(uint16_t mascara);

// Muestra un número flotante en el LCD con una cantidad específica de decimales
void LCD8_Variable(float v, uint8_t n);

// Muestra un número entero sin signo en el LCD
void LCD8_Variable_U(uint8_t v);

// Convierte un número entero sin signo a cadena de caracteres
void uint8_to_stringl(uint8_t num, char *buffer);

// Convierte un número flotante a una cadena con cierto número de decimales
void float_to_string(float num, char *buffer, uint8_t decimales);

// Muestra un solo carácter en el LCD
void LCD8_Write_Char(char a);

// Muestra una cadena de texto en el LCD
void LCD8_Write_String(char *a);

// Posiciona el cursor en una coordenada específica (columna, fila)
void LCD8_Set_Cursor(uint16_t c, uint16_t f);

// Limpia el contenido del LCD
void LCD8_Clear(void);

#endif /* LCD_8B_H_ */
