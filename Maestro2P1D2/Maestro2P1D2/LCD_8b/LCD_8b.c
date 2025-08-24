/*
 * LCD_8b.c
 *
 * Created: 18/07/2025 20:43:50
 * Author: pyocm
 */

#include "LCD_8b.h"

// Mapeo de pines: RS, E, D7...D0
unsigned char pines[10] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 12};

void initLCD8(void) {
	// Configura los pines como salida
	for (uint8_t i = 0; i < 10; i++) {
		uint8_t pin = pines[i];
		if (pin >= 8 && pin <= 13) {
			// Pines en PORTB
			DDRB |= (1 << (pin - 8));
		} else if (pin <= 7) {
			// Pines en PORTD
			DDRD |= (1 << pin);
		}
	}
	_delay_ms(15);

	// Configuración inicial del LCD (Function Set)
	LCD8_PORT(0b0011100000);
	_delay_ms(5);

	// Encendido del display (Display ON/OFF)
	LCD8_PORT(0b0000110000);
	_delay_ms(10);

	// Modo de entrada (Entry Mode)
	LCD8_PORT(0b0000011000);
	_delay_ms(10);

	// Limpia el contenido del LCD
	LCD8_PORT(0b0000000100);
	_delay_ms(10);
}

// Envía una máscara de 10 bits al puerto para controlar RS, E y D7...D0
void LCD8_PORT(uint16_t mascara) {
	for (int i = 0; i < 10; i++) {
		if (mascara & (1 << i)) {
			uint8_t pin = pines[i];
			if (pin >= 8 && pin <= 13) {
				PORTB |= (1 << (pin - 8));
			} else if (pin <= 7) {
				PORTD |= (1 << pin);
			}
		} else {
			uint8_t pin = pines[i];
			if (pin >= 8 && pin <= 13) {
				PORTB &= ~(1 << (pin - 8));
			} else if (pin <= 7) {
				PORTD &= ~(1 << pin);
			}
		}
	}

	// Pulso de habilitación (Enable)
	PORTD |= (1 << 4);
	_delay_ms(3);
	PORTD &= ~(1 << 4);
}

// Envía una instrucción (comando) al LCD usando una máscara de 10 bits
void LCD8_CMD(uint16_t mascara) {
	for (int i = 0; i < 10; i++) {
		if (mascara & (1 << i)) {
			uint8_t pin = pines[i];
			if (pin >= 8 && pin <= 13) {
				PORTB |= (1 << (pin - 8));
			} else if (pin <= 7) {
				PORTD |= (1 << pin);
			}
		} else {
			uint8_t pin = pines[i];
			if (pin >= 8 && pin <= 13) {
				PORTB &= ~(1 << (pin - 8));
			} else if (pin <= 7) {
				PORTD &= ~(1 << pin);
			}
		}
	}
	// Pulso de habilitación
	_delay_ms(3);
	PORTD |= (1 << 4);
	_delay_ms(3);
	PORTD &= ~(1 << 4);
}

// Muestra un número flotante en el LCD con n decimales
void LCD8_Variable(float v, uint8_t n) {
	char str[10];
	float_to_string(v, str, n);
	LCD8_Write_String(str);
}

// Muestra un número entero sin signo en el LCD
void LCD8_Variable_U(uint8_t v) {
	char str[4];
	uint8_to_stringl(v, str);
	LCD8_Write_String(str);
}

// Escribe un único carácter en el LCD
void LCD8_Write_Char(char c) {
	uint16_t valor = (c << 2) | 1;
	LCD8_CMD(valor);
}

// Muestra una cadena de caracteres en el LCD
void LCD8_Write_String(char *a) {
	for (int i = 0; a[i] != '\0'; i++) {
		LCD8_Write_Char(a[i]);
	}
}

// Posiciona el cursor en columna `c` y fila `f` (0 o 1)
void LCD8_Set_Cursor(uint16_t c, uint16_t f) {
	uint16_t coordenada;
	if (f == 0) {
		// Primera fila
		coordenada = (c << 2) + 512;
		LCD8_CMD(coordenada);
	} else if (f == 1) {
		// Segunda fila
		coordenada = ((c + 64) << 2) + 512;
		LCD8_CMD(coordenada);
	}
}

// Borra el contenido del LCD
void LCD8_Clear(void) {
	LCD8_CMD(0b100);
}

// Convierte un número flotante a cadena con n decimales
void float_to_string(float num, char *buffer, uint8_t decimales) {
	int parte_entera = (int)num;
	int parte_decimal = (int)((num - parte_entera) * 100);

	if (parte_decimal < 0) parte_decimal *= -1;

	int i = 0;

	// Parte entera
	if (parte_entera == 0) {
		buffer[i++] = '0';
	} else {
		int temp = parte_entera;
		char temp_buffer[10];
		int j = 0;
		while (temp > 0) {
			temp_buffer[j++] = (temp % 10) + '0';
			temp /= 10;
		}
		while (j > 0) {
			buffer[i++] = temp_buffer[--j];
		}
	}

	// Agrega punto decimal
	buffer[i++] = '.';

	// Parte decimal (solo 2 dígitos)
	buffer[i++] = (parte_decimal / 10) + '0';
	buffer[i++] = (parte_decimal % 10) + '0';

	buffer[i] = '\0';
}

// Convierte un número uint8_t a cadena
void uint8_to_stringl(uint8_t num, char *buffer) {
	uint8_t i = 0;
	char temp[4];
	uint8_t j = 0;

	if (num == 0) {
		buffer[i++] = '0';
	} else {
		while (num > 0) {
			temp[j++] = (num % 10) + '0';
			num /= 10;
		}
		while (j > 0) {
			buffer[i++] = temp[--j];
		}
	}

	buffer[i] = '\0';
}
