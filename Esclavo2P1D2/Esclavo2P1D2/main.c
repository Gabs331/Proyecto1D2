/*
 * Esclavo 2P1D2.c
 *
 * Created: 17/08/2025 20:47:48
 * Author : pyocm
 */ 
// ESCLAVO DETECTOR DE HUMO/CO2, BUZZER Y MOTOR DC
//*************************************************************************************************
// LIBRERIAS
//*************************************************************************************************
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "ADC/ADC.h"
#include "I2C/I2C.h"

//********************************************
// VARIABLES GLOBALES
//********************************************
#define SlaveAddress 0x30
#define UMBRAL_AIRE 100   // Ajusta según tu calibración del MQ-135

uint8_t HUMO = 0;
uint8_t buffer = 0;        // Recibe comandos I2C
uint8_t send_HUMO = 0;     // Valor que se enviará al maestro

//********************************************
// PROTOTIPOS
//********************************************
void setup_pines(void);

//********************************************
// MAIN
//********************************************
int main(void)
{
    setup_pines();

    I2C_Slave_Init(SlaveAddress);
    sei();

    // Configurar ADC
    setADC(externo, izquierda, 7);
    controlADC(1, 0, 1, 128);

    while(1)
    {
        ADCSRA |= (1 << ADSC); // Inicia conversión ADC
        _delay_ms(50);          // Espera a que termine la conversión

        // Activar buzzer automáticamente según lectura del MQ-135
        if(HUMO > UMBRAL_AIRE){
            PORTD |= (1 << PD3); // Enciende buzzer
        } else {
            PORTD &= ~(1 << PD3); // Apaga buzzer
        }

        // Control manual del buzzer vía I2C
        if(buffer == 'H'){       
            PORTD |= (1 << PD3);
            buffer = 0;
        }
        if(buffer == 'h'){       
            PORTD &= ~(1 << PD3);
            buffer = 0;
        }
    }
}

//********************************************
// INTERRUPCIONES
//********************************************
ISR(ADC_vect){
    HUMO = ADCH;
    send_HUMO = HUMO;          // Actualiza valor a enviar al maestro
    ADCSRA |= (1 << ADIF);     // Apagar bandera ADC
}

ISR(TWI_vect){
    uint8_t estado = TWSR & 0xFC;
    switch(estado){
        case 0x60: case 0x70: // SLA+W recibido
            TWCR |= (1 << TWINT) | (1 << TWEA);
            break;
        case 0x80: case 0x90: // Dato recibido
            buffer = TWDR;    // Guarda comando I2C
            TWCR |= (1 << TWINT) | (1 << TWEA);
            break;
        case 0xA8: case 0xB8: // SLA+R recibido
            TWDR = send_HUMO; // Envía lectura del MQ-135
            TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWINT) | (1 << TWEA);
            break;
        default:
            TWCR |= (1 << TWINT) | (1 << TWEA);
            break;
    }
}

//********************************************
// FUNCIONES
//********************************************
void setup_pines(void){
    DDRD |= (1 << PD3); // PD3 buzzer
}
