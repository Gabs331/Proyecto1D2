#define F_CPU 16000000  // Frecuencia del micro

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <math.h>   // <-- agregar
#include "UART/UART.h"
#include "LCD_8b/LCD_8b.h"
#include "I2C/I2C.h"

// Direcciones I2C
#define SLAVE_HUMO 0x30   // <-- agregar
#define SLAVE_2 0x40      // RFID + servo
#define SLAVE_TEMP 0x38   // sensor temperatura

// Variables globales
uint8_t rfid_status = 3;   // 0=denegado, 1=Pablo, 2=Pedro, 3=sin_novedad, 4=cierre manual
uint8_t online_access = 0; // 0=offline, 1=online
uint8_t servo_pos = 0;     // 0=cerrado, 1=abierto
uint8_t last_sent_servo = 255; // Para no mandar lo mismo siempre
uint16_t dato = 0;
uint8_t valorTYH[6];
float temperatura;
char buf[20];
uint8_t fan_state = 0;     // 0=apagado, 1=encendido
uint8_t last_sent_fan = 255; // Para no mandar lo mismo siempre
uint8_t valorH[1];   // byte leído del esclavo de humo
float CH;            // concentración de humo calculada (kppm aprox.)
char humoStr[20];    // buffer para imprimir/enviar humo


// Funciones
void disp_acceso(uint8_t rfid_status);
void send_val(uint8_t s, uint8_t r, float t, float h);
void read_UART(void);
void I2Ctemp(void);
void I2Chumo(void);  // <-- agregar
void floatToString(float num, char *str, int precision);
void disp_val(float t, float h, uint8_t d, uint8_t s, uint8_t m);


int main(void) {
	// Inicializar I2C, UART y LCD
	I2C_Master_Init(100000, 1);
	UART_Init();
	initLCD8();
	LCD8_Set_Cursor(0,0);
	LCD8_Write_String("Iniciando...");
	_delay_ms(2000);
	LCD8_Clear();

	DDRD |= (1<<2);
	PORTD |= (1<<2);

	// Inicializar sensor AHT10
	I2C_Master_Start();
	I2C_Master_Write((SLAVE_TEMP << 1) | 0);
	I2C_Master_Write(0xE1);
	I2C_Master_Stop();
	_delay_ms(350);

	while(1) {
		read_UART(); // revisar comandos desde UART

		I2Ctemp();   // leer temperatura

		// Calcular temperatura
		uint32_t rawTemp = ((uint32_t)(valorTYH[3] & 0x0F) << 16) | ((uint32_t)valorTYH[4] << 8) | valorTYH[5];
		temperatura = (rawTemp * 200.0 / 1048576.0) - 50.0;
		floatToString(temperatura, buf, 1);
		PORTD &= ~(1<<2);

		// Control de ventilador según temperatura
		if (temperatura > 29.0) {
			// prender ventilador
			I2C_Master_Start();
			if (I2C_Master_Write((SLAVE_2 << 1) & 0xFE) == 1) I2C_Master_Write('V');
			I2C_Master_Stop();
			} else {
			// apagar ventilador
			I2C_Master_Start();
			if (I2C_Master_Write((SLAVE_2 << 1) & 0xFE) == 1) I2C_Master_Write('v');
			I2C_Master_Stop();
		}

		I2Chumo();                        // lee 1 byte del esclavo 0x30
		uint8_t humo = valorH[0];         // Rs leído desde el esclavo
		if (humo == 0) humo = 1;          // evita división entre cero

		// Calcular concentración (ejemplo con Ro = 5k)
		CH = (pow(50.0 / (5.0 / humo), 2)) / 1000.0f;

		// Leer estado RFID del esclavo
		I2C_Master_Start();
		if (I2C_Master_Write((SLAVE_2 << 1) | 0x01) == 1) {
			TWCR = (1<<TWINT)|(1<<TWEN);
			while (!(TWCR & (1<<TWINT)));
			rfid_status = TWDR;
		}
		I2C_Master_Stop();
		_delay_ms(5);

		// Actualizar posición del servo según RFID
		if (rfid_status == 1 || rfid_status == 2) {
			servo_pos = 1;            // tarjeta válida abre
			disp_acceso(rfid_status); // mensaje bienvenida
			} else if (rfid_status == 0) {
			servo_pos = 0;
			disp_acceso(0);           // acceso denegado
			} else if (rfid_status == 4) {
			servo_pos = 0;            // cierre manual
		}

		// Mandar S/s al esclavo solo si cambió
		if (servo_pos != last_sent_servo) {
			I2C_Master_Start();
			if (I2C_Master_Write((SLAVE_2 << 1) & 0xFE) == 1) {
				I2C_Master_Write(servo_pos == 1 ? 'S' : 's');
			}
			I2C_Master_Stop();
			last_sent_servo = servo_pos;
		}

		// Mandar V/v al esclavo solo si cambió ventilador
		if (fan_state != last_sent_fan) {
			I2C_Master_Start();
			if (I2C_Master_Write((SLAVE_2 << 1) & 0xFE) == 1) {
				I2C_Master_Write(fan_state == 1 ? 'V' : 'v');
			}
			I2C_Master_Stop();
			last_sent_fan = fan_state;
		}

		// Mostrar temperatura y servo en LCD
		disp_val(temperatura, CH,  rfid_status, servo_pos, online_access);

		// Enviar datos por UART
		send_val(servo_pos, rfid_status, temperatura, CH);
		
		_delay_ms(500);
	}
}

// Mensajes temporales de acceso
void disp_acceso(uint8_t rfid_status){
	switch(rfid_status){
		case 1:
		LCD8_Clear();
		LCD8_Set_Cursor(0,0);
		LCD8_Write_String("Bienvenido Pablo");
		_delay_ms(1500);
		LCD8_Clear();
		break;
		case 2:
		LCD8_Clear();
		LCD8_Set_Cursor(0,0);
		LCD8_Write_String("Bienvenido Pedro");
		_delay_ms(1500);
		LCD8_Clear();
		break;
		case 0:
		LCD8_Clear();
		LCD8_Set_Cursor(0,0);
		LCD8_Write_String("Acceso denegado");
		_delay_ms(1500);
		LCD8_Clear();
		break;
		default:
		break; // 3/4 -> sin mensaje
	}
}

// Mostrar temperatura y estado del servo
void disp_val(float t, float h, uint8_t d, uint8_t s, uint8_t m){
	LCD8_Set_Cursor(0,0);
	LCD8_Write_String("T:");
	LCD8_Variable(t, 1);
	LCD8_Write_Char(' ');
	LCD8_Write_Char(0xDF); // °C
	LCD8_Write_String("C");

	LCD8_Set_Cursor(1, 13);
	LCD8_Write_String("S:");
	LCD8_Write_String(s ? "ON" : "OFF");
	
	LCD8_Set_Cursor(0, 1);   // esquina derecha de la 1ª línea
	LCD8_Write_String("H:");
	LCD8_Variable(h, 2);
	LCD8_Write_String("ppm");
}

// Enviar datos por UART
void send_val(uint8_t s, uint8_t r, float t, float h){
	UART_Char('s'); UART_Var(s); UART_Char(' ');
	UART_Char('r'); UART_Var(r); UART_Char(' ');
	UART_Char('T');
	char tempStr[10];
	floatToString(t, tempStr, 1);
	UART_Write(tempStr);
	UART_Char(' ');
	UART_Char('H');
	char humoStr[10];
	floatToString(h, humoStr, 2);
	UART_Write(humoStr);
	UART_Char(' ');
	UART_Char('\n');
}

// Leer comandos desde UART
void read_UART(void){
	dato = UART_Read();
	if(dato != 0){
		if(dato == 'A'){ // online
			online_access = 1;
			PORTB |= (1<<5);
			LCD8_Clear();
			LCD8_Set_Cursor(4,0);
			LCD8_Write_String("ONLINE");
			_delay_ms(1500);
			LCD8_Clear();
			} else if(dato == 'a'){ // offline
			online_access = 0;
			PORTB &= ~(1<<5);
			LCD8_Clear();
			LCD8_Set_Cursor(4,1);
			LCD8_Write_String("OFFLINE");
			_delay_ms(1500);
			LCD8_Clear();
		}
		if(online_access){
			if(dato=='S') servo_pos=1;
			else if(dato=='s') servo_pos=0;
			else if(dato=='V') fan_state=1;
			else if(dato=='v') fan_state=0;
		}
	}
}

// Leer temperatura del sensor AHT10
void I2Ctemp(void){
	uint8_t status;

	// pedir medición
	I2C_Master_Start();
	status = I2C_Master_Write((SLAVE_TEMP << 1) | 0);
	if(status != 1){ I2C_Master_Stop(); return; }
	I2C_Master_Write(0xAC);
	I2C_Master_Write(0x33);
	I2C_Master_Write(0x00);
	I2C_Master_Stop();
	_delay_ms(350);

	// leer 6 bytes
	I2C_Master_Start();
	status = I2C_Master_Write((SLAVE_TEMP << 1) | 1);
	if(status != 1){ I2C_Master_Stop(); return; }
	for(uint8_t i=0;i<6;i++){
		status = I2C_Master_Read(&valorTYH[i], i<5);
		if(status != 1){ I2C_Master_Stop(); return; }
	}
	I2C_Master_Stop();
}
void I2Chumo(void){
	uint8_t status;
	I2C_Master_Start();
	status = I2C_Master_Write((SLAVE_HUMO << 1) | 1); // lectura
	if(status != 1){ I2C_Master_Stop(); return; }
	status = I2C_Master_Read(&valorH[0], 0);          // NACK en el último byte
	I2C_Master_Stop();
}


// Convertir float a string
void floatToString(float num, char *str, int precision){
	int intPart = (int)num;
	float decimalPart = num - intPart;
	itoa(intPart,str,10);
	int i=0;
	while(str[i]!='\0') i++;
	str[i++]='.';
	for(int j=0;j<precision;j++){
		decimalPart *= 10;
		int decDigit = (int)decimalPart;
		str[i++] = decDigit+'0';
		decimalPart -= decDigit;
	}
	str[i]='\0';
}
