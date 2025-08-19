/*
 * Esclavo2.c
 *
 * Created: 18/08/2025 13:44:36
 * Author : Rodrigo Lara
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "hx711.h"
#include "lcd_i2c.h"
#include "i2c.h"

#define BTN_ZERO PD2
#define LED PB5

long escala;
int peso_calibracion = 185;

void calibration() {
	LCD_clear();
	LCD_print("Calibrando...");
	_delay_ms(2000);

	HX711_set_scale(1);
	HX711_tare(20);

	LCD_clear();
	LCD_print("Ponga peso ref");
	_delay_ms(3000);

	long adc_lecture = HX711_read_average(100);
	escala = adc_lecture / peso_calibracion;

	EEPROM_write_long(0, escala);

	LCD_clear();
	LCD_print("Calibrado OK");
	_delay_ms(2000);
}

int main(void) {
	DDRB |= (1 << LED);
	DDRD &= ~(1 << BTN_ZERO);

	LCD_init(0x27);
	HX711_init(PD4, PD5);

	escala = EEPROM_read_long(0);
	if(escala == 0) escala = 1000;

	if(PIND & (1 << BTN_ZERO)) {
		calibration();
	}

	HX711_set_scale(escala);
	HX711_tare(20);

	while(1) {
		double peso = HX711_get_units(10);

		LCD_clear();
		LCD_print("Peso: ");
		LCD_print_int((int)peso);
		LCD_print(" g");

		if(peso >= 500) PORTB |= (1 << LED);
		else PORTB &= ~(1 << LED);

		if(!(PIND & (1 << BTN_ZERO))) {
			HX711_tare(10);
		}

		_delay_ms(300);
	}
}