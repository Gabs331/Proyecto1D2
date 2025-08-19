#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "UART/UART.h"
#include "MFRC522/spi.h"
#include "MFRC522/MFRC522.h"
#include "Servomotores/Servomotores.h"
#include "I2C/I2C.h"

//********************************************
// VARIABLES GLOBALES
//********************************************
#define SlaveAddress 0x40       // Dirección I2C del esclavo
uint8_t buffer = 0;             // Buffer para recibir comandos I2C
uint8_t send_user = 3;          // 0=denegado, 1=Pablo, 2=Pedro, 3=sin_novedad, 4=cierre_manual

// UID de usuarios autorizados
uint8_t Pablo[]   = {0x5C, 0xF2, 0x8F, 0x02};
uint8_t Pedro[]   = {0x63, 0x83, 0xF7, 0x2C};

// Flags de coincidencia de tarjeta
uint8_t all_match_Pablo = 1;
uint8_t all_match_Pedro = 1;

// Flags de control de actuadores
volatile uint8_t abrir_servo = 0; // 0=cerrar, 1=abrir (manual o RFID)
volatile uint8_t flag_servo = 0;  // Control vía I2C para servo
volatile uint8_t flag_ventilador = 0; // Control ventilador vía I2C

uint8_t SelfTestBuffer[64];       // Buffer de autodiagnóstico RFID

//********************************************
// PROTOTIPOS
//********************************************
void setup_pines(void);  // Configura pines de entrada/salida

//********************************************
// MAIN
//********************************************
int main(void)
{
    setup_pines();       // Inicializa los pines

    UART_Init();          // Inicializa UART
    I2C_Slave_Init(SlaveAddress);  // Inicializa I2C como esclavo
    spi_init();           // Inicializa SPI
    mfrc522_init();       // Inicializa lector RFID
    PWM_init_Timer0();    // Inicializa PWM para servo
    servo_write_Timer0(180); // Mantener servo cerrado al inicio
    sei();                // Habilitar interrupciones globales

    uint8_t byte;
    uint8_t str[MAX_LEN];

    while(1)
    {
        // ===========================
        // Control servo según abrir_servo
        // ===========================
        if (abrir_servo){
            servo_write_Timer0(0);   // Abrir
        } else {
            servo_write_Timer0(180); // Cerrar
        }

        // ===========================
        // Comandos I2C para abrir/cerrar servo
        // ===========================
        if (buffer == 's'){
            buffer = 0;
            abrir_servo = 0;   // Cerrar servo
        }
        else if (buffer == 'S'){
            buffer = 0;
            abrir_servo = 1;   // Abrir servo
        }
		
        // ===========================
        // Control servo según flag_servo I2C
        // ===========================
        if(flag_servo) {
            servo_write_Timer0(0);   // Abrir
        } else {
            servo_write_Timer0(180); // Cerrar
        }

        // ===========================
        // Control ventilador
        // ===========================
        if(flag_ventilador) {
            PORTD |= (1 << PD4);    // Encender ventilador
        } else {
            PORTD &= ~(1 << PD4);   // Apagar ventilador
        }

        // ===========================
        // Lectura de tarjeta RFID
        // ===========================
        byte = mfrc522_request(PICC_REQALL, str); // Solicitar tarjeta
        if(byte == CARD_FOUND)
        {
            byte = mfrc522_get_card_serial(str);  // Obtener UID
            if(byte == CARD_FOUND)
            {
                all_match_Pablo = 1;
                all_match_Pedro = 1;

                // Comparar UID recibido con usuarios autorizados
                for(byte = 0; byte <= 3; byte++)
                {
                    uint8_to_string(str[byte]);   // Enviar UID a UART
                    UART_Char('\n');

                    if(str[byte] != Pablo[byte]) { all_match_Pablo = 0;}
                    if(str[byte] != Pedro[byte]) { all_match_Pedro = 0;}
                }

                // ===========================
                // Decisión según coincidencia de UID
                // ===========================
                if(all_match_Pablo){
                    send_user = 1;
                    UART_Write("Bienvenido Pablo");
                    abrir_servo = 1;          // Abrir servo
                    UART_Char('\n');
                }
                else if(all_match_Pedro){
                    send_user = 2;
                    UART_Write("Bienvenido Pedro");
                    abrir_servo = 1;          // Abrir servo
                    UART_Char('\n');
                }
                else{
                    UART_Write("Acceso denegado");
                    send_user = 0;
                    abrir_servo = 0;          // Cerrar servo
                    all_match_Pablo = 1;
                    all_match_Pedro = 1;
                    UART_Char('\n');
                }
                UART_Char('\n');
            }
            else {
                UART_Write("error"); // Error al leer UID
            }
        }

        // ===========================
        // Botón manual PD2
        // ===========================
        if (!(PIND & (1 << PD2))){
            abrir_servo = 0;          // Cerrar servo
            send_user = 4;            // Notificar cierre manual
            _delay_ms(500);           // Antirrebote simple
        }

        // ===========================
        // Control ventilador vía I2C
        // ===========================
        if(buffer == 'V'){
            PORTD |= (1 << PD4);      // Encender ventilador
            buffer = 0;
        }
        if(buffer == 'v'){
            PORTD &= ~(1 << PD4);     // Apagar ventilador
            buffer = 0;
        }

        _delay_ms(50);                // Pequeño delay para estabilidad
    }
}

//********************************************
// INTERRUPCIONES
//********************************************
ISR(TWI_vect){
    uint8_t estado = TWSR & 0xFC; // Obtener estado I2C

    switch(estado){
        case 0x60: case 0x70: // SLA+W recibido
            TWCR |= (1 << TWINT) | (1 << TWEA); // Preparar para recibir dato
            break;

        case 0x80: case 0x90: // Dato recibido
            if(TWDR == 'S') flag_servo = 1;
            else if(TWDR == 's') flag_servo = 0;
            else if(TWDR == 'V') flag_ventilador = 1;
            else if(TWDR == 'v') flag_ventilador = 0;
            TWCR |= (1 << TWINT) | (1 << TWEA); // Preparar siguiente dato
            break;

        case 0xA8: case 0xB8: // SLA+R recibido
            TWDR = send_user;  // Enviar estado al maestro
            send_user = 3;     // Reset "sin novedad"
            TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWINT) | (1 << TWEA);
            break;

        default:
            TWCR |= (1 << TWINT) | (1 << TWEA); // Manejo default
            break;
    }
}

//********************************************
// FUNCIONES
//********************************************
void setup_pines(void){
    DDRD |= (1 << DDD4);  // PD4 como salida ventilador
    PORTD &= ~(1 << PD4); // Apagar ventilador al inicio

    DDRD |= (1 << 3);     // PD3 como salida servo
    DDRD &= ~(1 << PD2);  // PD2 como entrada (botón)
    PORTD |= (1 << PD2);  // Habilitar pull-up en botón
}
