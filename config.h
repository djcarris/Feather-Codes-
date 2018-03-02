
// edit the config.h tab and enter your Adafruit IO credentials
// and any additional configuration needed for WiFi, cellular,
// or ethernet clients.
// https://io.adafruit.com/djcarris/dashboards

// Define AIO credentials and server information
#define IO_SERVER      "io.adafruit.com"
#define IO_USERNAME    "USERNAME"
#define IO_KEY         "KEY"

//Store MQTT server, username, and password in flash memory
const char MQTT_SERVER[] PROGMEM    = IO_SERVER;
const char MQTT_USERNAME[] PROGMEM  = IO_USERNAME;
const char MQTT_PASSWORD[] PROGMEM  = IO_KEY;
/******************************* WIFI **************************************/

// the AdafruitIO_WiFi client will work with the following boards:
//   - HUZZAH ESP8266 Breakout -> https://www.adafruit.com/products/2471
//   - Feather HUZZAH ESP8266 -> https://www.adafruit.com/products/2821
//   - Feather M0 WiFi -> https://www.adafruit.com/products/3010
//   - Feather WICED -> https://www.adafruit.com/products/3056

// Define WiFi credentials
#define WIFI_SSID "SSID"
#define WIFI_PASS "PASSWORD"

// comment out the following two lines if you are using fona or ethernet
#include "AdafruitIO_WiFi.h"
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);

