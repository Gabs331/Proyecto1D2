/*
 * HX711.c
 *
 * Created: 18/08/2025 13:46:21
 *  Author: Rodrigo Lara
 */ 

// hx711.c
#include "hx711.h"

static uint8_t DT, SCK;
static double SCALE = 1;
static long OFFSET = 0;

void HX711_init(uint8_t dt_pin, uint8_t sck_pin) {
	DT = dt_pin;
	SCK = sck_pin;

	DDRD &= ~(1 << DT);   // DT como entrada
	DDRD |= (1 << SCK);   // SCK como salida
	PORTD &= ~(1 << SCK); // SCK en LOW
}

long HX711_read(void) {
	long count = 0;
	while(PIND & (1 << DT)); // espera a que esté LOW
	for(uint8_t i=0; i<24; i++) {
		PORTD |= (1 << SCK);
		_delay_us(1);
		count = count << 1;
		PORTD &= ~(1 << SCK);
		if(PIND & (1 << DT)) count++;
		_delay_us(1);
	}
	// Ganancia 128
	PORTD |= (1 << SCK);
	_delay_us(1);
	PORTD &= ~(1 << SCK);
	count ^= 0x800000;
	return count;
}

long HX711_read_average(uint8_t times) {
	long sum = 0;
	for(uint8_t i=0; i<times; i++) {
		sum += HX711_read();
	}
	return sum / times;
}

void HX711_tare(uint8_t times) {
	long sum = HX711_read_average(times);
	HX711_set_offset(sum);
}

void HX711_set_scale(double scale) {
	SCALE = scale;
}

void HX711_set_offset(long offset) {
	OFFSET = offset;
}

double HX711_get_units(uint8_t times) {
	long val = HX711_read_average(times);
	return (double)(val - OFFSET) / SCALE;
}
