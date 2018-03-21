#include <Wire.h>
#include "RTClib.h"
#include <AdafruitIO.h>
#include <AdafruitIO_Dashboard.h>
#include <AdafruitIO_Data.h>
#include <AdafruitIO_Definitions.h>
#include <AdafruitIO_Feed.h>
#include <AdafruitIO_Group.h>
#include <SPI.h>
#include <SD.h>
#include <WiFi101.h>
//#include "config.h"


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

/************************** Configuration ***********************************/

// edit the config.h tab and enter your Adafruit IO credentials
// and any additional configuration needed for WiFi, cellular,
// or ethernet clients.
// https://io.adafruit.com/djcarris/dashboards
#define IO_USERNAME    "djcarris"
#define IO_KEY         "0fdce9c9a6f5488c89ad4274f5abf722"

/******************************* WIFI **************************************/

// the AdafruitIO_WiFi client will work with the following boards:
//   - HUZZAH ESP8266 Breakout -> https://www.adafruit.com/products/2471
//   - Feather HUZZAH ESP8266 -> https://www.adafruit.com/products/2821
//   - Feather M0 WiFi -> https://www.adafruit.com/products/3010
//   - Feather WICED -> https://www.adafruit.com/products/3056

#define WIFI_SSID "Projects"
#define WIFI_PASS "StudentProjects"

// comment out the following two lines if you are using fona or ethernet
#include "AdafruitIO_WiFi.h"
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);


/************************ Example Starts Here *******************************/
// The Wifi radio's status
int status = WL_IDLE_STATUS;
// Global Variables for WiFi_Connection
WiFiClient client;
// Global variables for sending alert emails
// smtp Email server assignment
char server[] = "mail.smtp2go.com";
int port = 2525;
// set boolean for email sending, change to true when condition met
boolean statusCheck = false;

//Sensor reading variables
// first assignemnt of current and last variables to be overwritten later
int current = 0;
int last = -1;

// set up the 'analog' feed with adafruit io
AdafruitIO_Feed *Turb = io.feed("Turbidity");

AdafruitIO_Feed *Temp = io.feed("Temperature");

AdafruitIO_Feed *Depth = io.feed("Depth");

AdafruitIO_Feed *Volt = io.feed("Voltage");

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
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(WIFI_SSID);
    status = WiFi.begin(WIFI_SSID,WIFI_PASS); 
    // wait 10 seconds for connection:
    delay(10000);
  }
  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
  Serial.println(" ");

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
  // read the input on analog pin A0 A1 A2 A3:
  int Depth_pin = analogRead(A0);
  int Turb_pin = analogRead(A1); 
  int Temp_pin = analogRead(A2);
  int Volt_pin = analogRead(A3);
  
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
    line+=analogRead(0);
    line+=" ";
    line+=analogRead(1);
    line+=" ";
    line+=analogRead(2);
    line+=" ";
    line+=analogRead(3);
    Serial.print(line);
    Serial.println(" ");
    logfile.print(line);
    logfile.println(" ");
    logfile.flush();
  // Sace to SD card
  // Pressure Sensor
  //logfile.print("DateTime: "); logfile.println(rtc.now());
  //Serial.print("DateTime: "); Serial.println(rtc.now());
  digitalWrite(8, HIGH);
  //logfile.print("Pressure = "); logfile.println(analogRead(0));
  //logfile.flush();
  Serial.print("Pressure = "); Serial.println(analogRead(0));
  digitalWrite(8, LOW);
  // Turbidity Sensor
  digitalWrite(8, HIGH);
  //logfile.print("Turbidity = "); logfile.println(analogRead(1));
  //logfile.flush();
  Serial.print("Turbidity = "); Serial.println(analogRead(1));
  digitalWrite(8, LOW);
  // Temperature Sensor
  digitalWrite(8, HIGH);
  //logfile.print("Temperature = "); logfile.println(analogRead(2));
  //logfile.flush();
  Serial.print("Temperature = "); Serial.println(analogRead(2));
  digitalWrite(8, LOW);
  // Volt Meter
  digitalWrite(8, HIGH);
  //logfile.print("Voltage = "); logfile.println(analogRead(3));
  //logfile.flush();
  Serial.print("Voltage = "); Serial.println(analogRead(3));
  digitalWrite(8, LOW);
   //email data
   if(sendEmail(Depth_pin, Turb_pin, Temp_pin, Volt_pin)) Serial.println("Email sent");    
       else Serial.println("Email failed");
        statusCheck = true;
  // Change delay to 3.6 x 10^6 for miliseconds in an hour
  //delay(3600000);

// email when voltage too low
  /*if(Volt_pin < 2.7 && statusCheck == false);
   {
       if(sendEmail()) Serial.println("Email sent");    
       else Serial.println("Email failed");
        statusCheck = true;
   }*/
delay(3600000);
}
byte sendEmail(int Depth_pin, int Turb_pin, int Temp_pin, int Volt_pin){
  byte thisByte = 0;
  byte respCode;
  char tBuf[64];
 
  if(client.connect(server,port) == 1) {
    Serial.println(F("connected"));
  } else {
    client.stop();
    Serial.println(F("connection failed"));
    return 0;
  }
 
  if(!eRcv()) return 0;
 
  // change to the IP of your Arduino
  strcpy_P(tBuf,PSTR("EHLO 149.119.146.59\r\n"));  
  client.write(tBuf);
  if(!eRcv()) return 0;
 
  Serial.println(F("Sending auth login"));
  strcpy_P(tBuf,PSTR("auth login\r\n"));  
  client.write(tBuf);
  if(!eRcv()) return 0;
 
  Serial.println(F("Sending User"));
  strcpy_P(tBuf,PSTR("bm1jZGF0YWxvZ2dlcnNAZ21haWwuY29t\r\n"));  
 
  client.write(tBuf);
  if(!eRcv()) return 0;
 
  Serial.println(F("Sending Password"));
  strcpy_P(tBuf,PSTR("Vk95UHU2VVpQNk9a\r\n"));  
  client.write(tBuf);
  if(!eRcv()) return 0;
 
// change to your email address (sender)
  Serial.println(F("Sending From"));
  strcpy_P(tBuf,PSTR("MAIL From: <nmcdataloggers@smtp2go.com>\r\n"));  
  client.write(tBuf);
  if(!eRcv()) return 0;
 
// change to recipient address
  Serial.println(F("Sending To"));
  strcpy_P(tBuf,PSTR("RCPT To: <nmcdataloggers@gmail.com>\r\n"));  
  client.write(tBuf);
  if(!eRcv()) return 0;
 
  Serial.println(F("Sending DATA"));
  strcpy_P(tBuf,PSTR("DATA\r\n"));  
  client.write(tBuf);
  if(!eRcv()) return 0;
 
  Serial.println(F("Sending email"));
 
// change to recipient address
  strcpy_P(tBuf,PSTR("To: nmccc <nmcdataloggers@gmail.com>\r\n"));  
  client.write(tBuf);
 
// change to your address
  strcpy_P(tBuf,PSTR("From: nmccc <nmcdataloggers@smtp2go.com\r\n"));  
  client.write(tBuf);
 
  client.println("Subject: Marcellus Library Datalogger Data!!\r\n");
  client.println("DateTime: ");
  //client.print(datestamp);
  client.println("Pressure Reading: ");
  client.print(Depth_pin);
  client.println("Turbidity Reading: ");
  client.print(Turb_pin);
  client.println("Temperatrue Reading: ");
  client.print(Temp_pin);
  client.println("Voltage Reading: ");
  client.print(Volt_pin);
  if(!eRcv()) return 0;
 
  Serial.println(F("Sending QUIT"));
  strcpy_P(tBuf,PSTR("QUIT\r\n"));  
  client.write(tBuf);
  if(!eRcv()) return 0;
 
  client.stop();
 
  Serial.println(F("disconnected"));
 
  return 1;
}
 
byte eRcv()
{
  byte respCode;
  byte thisByte;
  int loopCount = 0;
 
  while(!client.available()){
    delay(1);
    loopCount++;
 
    // if nothing received for 10 seconds, timeout
    if(loopCount > 10000) {
      client.stop();
      Serial.println(F("\r\nTimeout"));
      return 0;
      }
  }
 
  respCode = client.peek();
 
  while(client.available()){  
    thisByte = client.read();    
    Serial.write(thisByte);
  }
 
  if(respCode >= '4'){
    efail();
    return 0;  
  }
  return 1;
}
 
 
void efail() {
  byte thisByte = 0;
  int loopCount = 0;
 
  client.println("QUIT");
 
  while(!client.available()) {
    delay(1);
    loopCount++;
 
    // if nothing received for 10 seconds, timeout
    if(loopCount > 10000) {
      client.stop();
      Serial.println(F("\r\nTimeout"));
      return;
    }
  }
}
