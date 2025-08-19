/*
 * lcd_i2c.c
 *
 * Created: 18/08/2025 13:47:36
 *  Author: Rodrigo Lara
 */ 

// lcd_i2c.c
#include "lcd_i2c.h"
#include "i2c.h"
#include <util/delay.h>
#include <stdio.h>

static uint8_t lcd_addr;

// Comandos del LCD
#define LCD_CLEAR       0x01
#define LCD_HOME        0x02
#define LCD_ENTRYMODE   0x06
#define LCD_DISPLAYON   0x0C
#define LCD_FUNCTIONSET 0x28

#define ENABLE 0b00000100
#define BACKLIGHT 0x08

static void LCD_send_nibble(uint8_t nibble, uint8_t rs) {
	uint8_t data = nibble | BACKLIGHT | (rs ? 0x01 : 0x00);

	I2C_start(lcd_addr << 1);
	I2C_write(data | ENABLE);
	_delay_us(1);
	I2C_write(data & ~ENABLE);
	I2C_stop();
	_delay_us(50);
}

static void LCD_send_byte(uint8_t data, uint8_t rs) {
	LCD_send_nibble(data & 0xF0, rs);
	LCD_send_nibble((data << 4) & 0xF0, rs);
}

static void LCD_cmd(uint8_t cmd) {
	LCD_send_byte(cmd, 0);
	_delay_ms(2);
}

void LCD_init(uint8_t addr) {
	lcd_addr = addr;

	I2C_init();

	_delay_ms(50);

	// Inicialización en 4 bits
	LCD_send_nibble(0x30, 0);
	_delay_ms(5);
	LCD_send_nibble(0x30, 0);
	_delay_us(200);
	LCD_send_nibble(0x30, 0);
	_delay_ms(5);
	LCD_send_nibble(0x20, 0);
	_delay_ms(5);

	LCD_cmd(LCD_FUNCTIONSET);
	LCD_cmd(LCD_DISPLAYON);
	LCD_cmd(LCD_CLEAR);
	LCD_cmd(LCD_ENTRYMODE);
}

void LCD_clear(void) {
	LCD_cmd(LCD_CLEAR);
	_delay_ms(2);
}

void LCD_setCursor(uint8_t col, uint8_t row) {
	uint8_t row_offsets[] = {0x00, 0x40};
	LCD_cmd(0x80 | (col + row_offsets[row]));
}

void LCD_print(const char *str) {
	while(*str) {
		LCD_send_byte(*str++, 1);
	}
}

void LCD_print_int(int value) {
	char buffer[16];
	snprintf(buffer, sizeof(buffer), "%d", value);
	LCD_print(buffer);
}
