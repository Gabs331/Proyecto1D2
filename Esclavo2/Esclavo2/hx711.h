/*
 * hx711.h
 *
 * Created: 18/08/2025 13:46:37
 *  Author: Rodrigo Lara
 */ 


// hx711.h
#ifndef HX711_H
#define HX711_H

#include <avr/io.h>
#include <util/delay.h>

void HX711_init(uint8_t dt_pin, uint8_t sck_pin);
long HX711_read(void);
long HX711_read_average(uint8_t times);
double HX711_get_units(uint8_t times);
void HX711_tare(uint8_t times);
void HX711_set_scale(double scale);
void HX711_set_offset(long offset);

#endif
