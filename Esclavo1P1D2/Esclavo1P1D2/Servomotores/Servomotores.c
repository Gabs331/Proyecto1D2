/*
 * Servomotores.c
 *
 * Created: 8/08/2025 11:16:05
 *  Author: pyocm
 */ 

#include "Servomotores.h"

void PWM_init(void){
	// PB1 | PB2
	DDRB |= (1<< PB1);
	TCNT1 = 0; // reset
	ICR1 = 39999; // TOP
	TCCR1A =  (1 << COM1A1) | (1 << COM1B1) | (0 << COM1A0) ; // low --> Compare Match
	TCCR1A |=  (1 << WGM11) | (0 << WGM10) ; // Fast PWM TOP ICR1
	TCCR1B = (1 << WGM13) | (1 << WGM12); // Fast PWM TOP ICR1
	TCCR1B |= (0 << CS12) | (1 << CS11) | ( 0 << CS10 ); // Prescaler 8
}
void servo_writeA(float servo180a){
	OCR1A = map(servo180a, 0, 180, 1000, 4800);
}

float map(float x, float in_min, float in_max, float out_min, float out_max){
	return ((x - in_min)*(out_max - out_min)/(in_max - in_min)) + out_min;
}

void servo_write_Timer0(uint8_t angle) {
	OCR0B = map(angle, 0, 180, 8, 38);  // Pulso entre 0.5ms (8) y 2.5ms (31)
}

void PWM_init_Timer0(void) {
	DDRD |= (1 << PD5);  // Configurar PD5 (OC0B) como salida

	TCCR0A = (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);  // Fast PWM
	TCCR0B = (1 << WGM02) | (1 << CS02) | (1 << CS00);     // Prescaler 1024 para 60Hz aprox.

	OCR0A = 249;  // Ajustar para frecuencia de 50Hz
}