// Adafruit IO Publish & Subscribe Example
// Adaptado para ESP32 con UART desde maestro ATmega

#include "config.h"

// ---------------- Variables globales ----------------
int count = 0;
float temperature = 0.0;
float concentracionHumo = 0.0;
int rfidAccess = 3;     // 1 = Pablo, 2 = Pedro, 3 = sin novedad
int servoState = 0;     // 1 = activado, 0 = desactivado

#define IO_LOOP_DELAY 15000
unsigned long lastUpdate = 0;

#define RX2 16         // Pin RX2 del ESP32
#define TX2 17         // Pin TX2 del ESP32
#define LED_BUILTIN 2  // LED integrado ESP32

// ---------------- Feeds de Adafruit IO ----------------
AdafruitIO_Feed *feedWiFi        = io.feed("wifi");
AdafruitIO_Feed *feedServo       = io.feed("servo");
AdafruitIO_Feed *feedMotorDC     = io.feed("motorDC");   
AdafruitIO_Feed *feedTemperatura = io.feed("Temperatura");
AdafruitIO_Feed *feedRFIDPE      = io.feed("rfidPedro");
AdafruitIO_Feed *feedRFIDPA      = io.feed("rfidPablo");
AdafruitIO_Feed *feedHumo        = io.feed("Humo");

// ---------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RX2, TX2); // UART maestro

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.print("Connecting to Adafruit IO");
  io.connect();

  feedWiFi->onMessage(handleMessageOnlineAccess);
  feedServo->onMessage(handleMessageServo);
  feedMotorDC->onMessage(handleMessageMotorDC);

  while (io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println();
  Serial.println(io.statusText());

  feedWiFi->get();
  feedServo->get();
  feedMotorDC->get();  
}

// ---------------------------------------------------------------------------
void loop() {
  io.run();

  // Leer datos UART del maestro primero
  read_Data();

  // Publicar a Adafruit cada 15 segundos
  if (millis() - lastUpdate > IO_LOOP_DELAY) {
    feedTemperatura->save(temperature);        
    feedHumo->save(concentracionHumo);        

    feedRFIDPA->save(rfidAccess == 1 ? 1 : 0); 
    feedRFIDPE->save(rfidAccess == 2 ? 1 : 0); 

    feedServo->save(servoState == 1 ? 1 : 0);  

    Serial.print("Publicado -> T: "); Serial.print(temperature);
    Serial.print(" Humo: "); Serial.print(concentracionHumo);
    Serial.print(" Servo: "); Serial.print(servoState);
    Serial.print(" RFID: "); Serial.println(rfidAccess);

    lastUpdate = millis();
  }
}

// ---------------------------------------------------------------------------
// Mensajes recibidos desde Adafruit IO (control online)
// Se envía solo un carácter al maestro con Serial2.write()
void handleMessageOnlineAccess(AdafruitIO_Data *data) {
  char valor = data->value()[0];
  Serial.print("Adafruit Online: "); Serial.println(valor);
  Serial2.write(valor == '1' ? 'A' : 'a'); // solo un carácter
  Serial.print("Enviado al maestro: "); Serial.println(valor == '1' ? 'A' : 'a');
}

void handleMessageServo(AdafruitIO_Data *data) {
  char valor = data->value()[0];
  Serial.print("Adafruit Servo: "); Serial.println(valor);
  Serial2.write(valor == '1' ? 'S' : 's'); // solo un carácter
  Serial.print("Enviado al maestro: "); Serial.println(valor == '1' ? 'S' : 's');
}

void handleMessageMotorDC(AdafruitIO_Data *data) {
  char valor = data->value()[0];
  Serial.print("Adafruit Motor: "); Serial.println(valor);
  Serial2.write(valor == '1' ? 'V' : 'v'); // solo un carácter
  Serial.print("Enviado al maestro: "); Serial.println(valor == '1' ? 'V' : 'v');
}

// ---------------------------------------------------------------------------
// Parseo de datos recibidos del maestro ATmega por UART
void read_Data() {
  while (Serial2.available()) {
    String data = Serial2.readStringUntil('\n');  // Leer línea completa
    data.trim(); // Elimina espacios y \r si hay

    if (data.length() == 0) return; // ignorar líneas vacías
    Serial.println("Recibido: " + data);

    int sIndex = data.indexOf('s');
    int rIndex = data.indexOf('r');
    int tIndex = data.indexOf('T');
    int hIndex = data.indexOf('H');

    if (sIndex >= 0 && rIndex > sIndex)
      servoState = data.substring(sIndex + 1, rIndex).toFloat();

    if (rIndex >= 0 && tIndex > rIndex)
      rfidAccess = data.substring(rIndex + 1, tIndex).toInt();

    if (tIndex >= 0 && hIndex > tIndex)
      temperature = data.substring(tIndex + 1, hIndex).toFloat();

    if (hIndex >= 0)
      concentracionHumo = data.substring(hIndex + 1).toFloat();

    // Debug
    Serial.print("Servo: "); Serial.println(servoState);
    Serial.print("RFID: "); Serial.println(rfidAccess);
    Serial.print("Temp: "); Serial.println(temperature);
    Serial.print("Humo: "); Serial.println(concentracionHumo);
  }
}
