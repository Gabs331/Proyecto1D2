/*
 * lcd_i2c.h
 *
 * Created: 18/08/2025 13:47:07
 *  Author: Rodrigo Lara
 */ 


// lcd_i2c.h
#ifndef LCD_I2C_H
#define LCD_I2C_H

#include <avr/io.h>

void LCD_init(uint8_t addr);
void LCD_clear(void);
void LCD_setCursor(uint8_t col, uint8_t row);
void LCD_print(const char *str);
void LCD_print_int(int value);

#endif
