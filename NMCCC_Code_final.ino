#include <Wire.h>
#include "RTClib.h"
//#include <AdafruitIO.h>
//#include <AdafruitIO_Dashboard.h>
//#include <AdafruitIO_Data.h>
//#include <AdafruitIO_Definitions.h>
//#include <AdafruitIO_Feed.h>
//#include <AdafruitIO_Group.h>
#include <SPI.h>
#include <SD.h>
#include <WiFi101.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// Adafruit IO Analog In Example
// Tutorial Link: https://learn.adafruit.com/adafruit-io-basics-analog-input
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Written by Todd Treece for Adafruit Industries
// Copyright (c) 2016 Adafruit Industries
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.

/************************* WiFI Setup *****************************/
char ssid[] = "Projects";     //  your network SSID (name)
char pass[] = "StudentProjects";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "djcarris"
#define AIO_KEY         "0fdce9c9a6f5488c89ad4274f5abf722"

/************ Global State (you don't need to change this!) ******************/

//Set up the wifi client
WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// You don't need to change anything below this line!
#define halt(s) { Serial.println(F( s )); while(1);  }


/****************************** Feeds ***************************************/

// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish Depth = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/marcellus-library.stage-m");
Adafruit_MQTT_Publish Temperature = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/marcellus-library.temperature-f");
Adafruit_MQTT_Publish TSS = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/marcellus-library.tss-mg-slash-l");
Adafruit_MQTT_Publish Voltage = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/marcellus-library.voltage-v");
Adafruit_MQTT_Publish Depth_calc = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/marcellus-library.stage-dn");
Adafruit_MQTT_Publish Temperature_calc = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/marcellus-library.temperature-dn");
Adafruit_MQTT_Publish TSS_calc = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/marcellus-library.tss-dn");
Adafruit_MQTT_Publish Voltage_calc = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/marcellus-library.voltage-dn");

// Sensor reading variables
// first assignemnt of current and last variables to be overwritten later
int current = 0;
int last = -1;

// RTC clock setup
#if defined(ARDUINO_ARCH_SAMD)
#endif

RTC_PCF8523 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// SD Card setup
// Set the pins used
#define cardSelect 10

File logfile;

// blink out an error code
void error(uint8_t errno) {
  while(1) {
    uint8_t i;
    for (i=0; i<errno; i++) {
      digitalWrite(13, HIGH);
      delay(100);
      digitalWrite(13, LOW);
      delay(100);
    }
    for (i=errno; i<10; i++) {
      delay(200);
    }
  }
}

void setup() {
  //WiFi pins specific to the Adafruit Feather M0 WiFi - ATSAMD21 + ATWINC1500 breakout board
  WiFi.setPins(8,7,4,2);
  
  // start the serial connection, sets rate of data transmission
  Serial.begin(115200);

  // wait for serial monitor to open
  while (!Serial);
  // attempt to connect to Wifi network:
  // Initialise the Client
  Serial.print(F("\nInit the WiFi module..."));
  // check for the presence of the breakout
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WINC1500 not present");
    // don't continue:
    while (true);
  }
  Serial.println("ATWINC OK!");
  
if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (! rtc.initialized()) {
    Serial.println("RTC is NOT running!");
  }

pinMode(13, OUTPUT);


  // see if the card is present and can be initialized:
  if (!SD.begin(cardSelect)) {
    Serial.println("Card init. failed!");
    error(2);
  }
  char filename[15];
  strcpy(filename, "ANALOG00.TXT");
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = '0' + i/10;
    filename[7] = '0' + i%10;
    // create if does not exist, do not open existing, write, sync after write
    if (! SD.exists(filename)) {
      break;
    }
  }

  logfile = SD.open(filename, FILE_WRITE);
  if( ! logfile ) {
    Serial.print("Couldnt create "); 
    Serial.println(filename);
    error(3);
  }
  Serial.print("Writing to "); 
  Serial.println(filename);

  pinMode(13, OUTPUT);
  pinMode(8, OUTPUT);
  Serial.println("Ready!");
}

uint8_t i=0;
void loop() 
{

  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();
  
  // read the input on analog pin A0 A1 A2 A3
  // Calculate water quality parameters using equations from calibration curves
  // These variables are required for the below functions
  int Depth_pin = analogRead(A0);
  int TSS_pin = analogRead(A1); 
  int Temp_pin = analogRead(A2);
  int Volt_pin = analogRead(A3);
  float Depth_m = 0.0075*(Depth_pin)-0.7786;
  float TSS_mgL = TSS_pin;
  float Temp_F = 0.2165*(Temp_pin)-7.6926;
  float Volt_V = 23977*exp(-0.007*Volt_pin);
  
  // call function to write data to SD card
  SD_write(Depth_pin, TSS_pin, Temp_pin, Volt_pin, Depth_m, TSS_mgL, Temp_F, Volt_V);
  
  // call function to publish data
  IO_publish ( Depth_pin, TSS_pin, Temp_pin, Volt_pin, Depth_m, TSS_mgL, Temp_F, Volt_V);
  
  delay(60000);
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

// attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);  
    // Connect to open network.
    // status = WiFi.begin(ssid);
  
    // wait 10 seconds for connection:
    uint8_t timeout = 10;
    while (timeout && (WiFi.status() != WL_CONNECTED)) {
      timeout--;
      delay(1000);
    }
  }
  
  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
  }
  Serial.println("MQTT Connected!");
}
// SD card writing function
void SD_write(int Depth_pin, int TSS_pin, int Temp_pin, int Volt_pin, int Depth_m, int TSS_mgL, int Temp_F, int Volt_V)
{
  DateTime now = rtc.now();
    String line = "";
    line += now.year();
    line +='/';
    line +=now.month();
    line +='/';
    line +=now.day();
    line +=" (";
    line += (daysOfTheWeek[now.dayOfTheWeek()]);
    line +=") ";
    line +=now.hour();
    line +=':';
    line +=now.minute();
    line +=':';
    line +=now.second();
    Serial.print(line);
    Serial.println(" ");
    line+=" ";    
    line+=Depth_pin;
    line+=" ";
    line+=TSS_pin;
    line+=" ";
    line+=Temp_pin;
    line+=" ";
    line+=Volt_pin;
    line+=" ";
    line+=Depth_m;
    line+=" ";
    line+=TSS_mgL;
    line+=" ";
    line+=Temp_F;
    line+=" ";
    line+=Volt_V;
    Serial.print(line);
    Serial.println(" ");
    logfile.print(line);
    logfile.println(" ");
    logfile.flush();
  // Sace to SD card
  
  // Pressure Sensor
  /*digitalWrite(8, HIGH);
  Serial.print("Stage DN = "); Serial.println(Depth_pin);
  digitalWrite(8, LOW);
  // TSS Sensor
  digitalWrite(8, HIGH);
  Serial.print("TSS DN = "); Serial.println(TSS_pin);
  digitalWrite(8, LOW);
  // Temperature Sensor
  digitalWrite(8, HIGH);
  Serial.print("Temperature DN = "); Serial.println(Temp_pin);
  digitalWrite(8, LOW);
  // Volt Meter
  digitalWrite(8, HIGH);
  Serial.print("Voltage DN = "); Serial.println(Volt_pin);
  digitalWrite(8, LOW);
  Serial.print("Stage_calculation = "); Serial.println(Depth_m);
  digitalWrite(8, LOW);
  Serial.print("TSS_calculation = "); Serial.println(TSS_mgL);
  digitalWrite(8, LOW);
  Serial.print("Temperature_calculation = "); Serial.println(Temp_F);
  digitalWrite(8, LOW);
  Serial.print("Voltage_calculation = "); Serial.println(Volt_V);
  digitalWrite(8, LOW);*/
}

// IO publishing function
void IO_publish (int Depth_pin, int TSS_pin, int Temp_pin, int Volt_pin, int Depth_m, int TSS_mgL, int Temp_F, int Volt_V)
{
Serial.print(F("\nSending Depth val "));
  Serial.print(Depth_pin);
  Serial.print("...");
  if (! Depth.publish(uint32_t(Depth_pin))) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

  Serial.print(F("\nSending Temperature val "));
  Serial.print(TSS_pin);
  Serial.print("...");
  if (! Temperature.publish(uint32_t(TSS_pin))) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

  Serial.print(F("\nSending TSS val "));
  Serial.print(Temp_pin);
  Serial.print("...");
  if (! TSS.publish(uint32_t(Temp_pin))) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

  Serial.print(F("\nSending Voltage val "));
  Serial.print(Volt_pin);
  Serial.print("...");
  if (! Voltage.publish(uint32_t(Volt_pin))) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

  Serial.print(F("\nSending Depth calculation val "));
  Serial.print(Depth_m);
  Serial.print("...");
  if (! Depth_calc.publish(uint32_t(Depth_m))) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

  Serial.print(F("\nSending TSS calculation val "));
  Serial.print(TSS_mgL);
  Serial.print("...");
  if (! TSS_calc.publish(uint32_t(TSS_mgL))) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

  Serial.print(F("\nSending Temperature calculation val "));
  Serial.print(Temp_F);
  Serial.print("...");
  if (! Temperature_calc.publish(uint32_t(Temp_F))) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

  Serial.print(F("\nSending Voltage calculation val "));
  Serial.print(Volt_V);
  Serial.print("...");
  if (! Voltage_calc.publish(uint32_t(Volt_V))) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
}

 

