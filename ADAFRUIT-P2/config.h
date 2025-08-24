/************************ Adafruit IO Config *******************************/

// visit io.adafruit.com if you need to create an account,
// or if you need your Adafruit IO key.
#define IO_USERNAME  "Yoc22007"
#define IO_KEY       "aio_MONj64sswYf6t2PGOXZ8uQRNgofg"


#define WIFI_SSID "TIGO-E0AC"
#define WIFI_PASS "4NJ9ED800491"

#include "AdafruitIO_WiFi.h"

AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);