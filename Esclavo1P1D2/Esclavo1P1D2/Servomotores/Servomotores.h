/*
 * Servomotores.h
 *
 * Created: 8/08/2025 11:16:21
 *  Author: pyocm
 */ 


#ifndef SERVOMOTORES_H_
#define SERVOMOTORES_H_

#include <avr/io.h>

void PWM_init(void);
void servo_writeA(float servo180a);


void PWM_init_Timer0(void);
void servo_write_Timer0(uint8_t angle);
float map(float x, float in_min, float in_max, float out_min, float out_max);


#endif /* SERVOMOTORES_H_ */