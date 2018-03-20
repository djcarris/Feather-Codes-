#include <AdafruitIO.h>
#include <AdafruitIO_Dashboard.h>
#include <AdafruitIO_Data.h>
#include <AdafruitIO_Definitions.h>
#include <AdafruitIO_Feed.h>
#include <AdafruitIO_Group.h>
#include <AdafruitIO_MQTT.h>
#include <Adafruit_MQTT.h>
#include <AdafruitIO_WiFi.h>
#include <Adafruit_MQTT_Client.h>
#include <SPI.h>
#include <WiFi101.h>
#include "config.h"


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

/************************ Example Starts Here *******************************/
// Global Variables for WiFi_Connection
WiFiClient client;
// The Wifi radio's status
int status = WL_IDLE_STATUS;

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
AdafruitIO_Feed *Turb = io.feed("Turbidity"); //Maybe Temp

AdafruitIO_Feed *Temp = io.feed("Temperature"); //Maybe Turb

AdafruitIO_Feed *Depth = io.feed("Depth");

AdafruitIO_Feed *Volt = io.feed("Voltage");

#define MQTT_CONN_KEEPALIVE 300

// flash memory storage of feed name on IO
const char DEPTH_FEED[] PROGMEM = IO_USERNAME "/feeds/marcellus-library-water-quality-data.depth";
const char TURBIDITY_FEED[] PROGMEM = IO_USERNAME "/feeds/marcellus-library-water-quality-data.turbidity";
const char TEMPERATURE_FEED[] PROGMEM = IO_USERNAME "/feeds/marcellus-library-water-quality-data.temperature";
const char VOLTAGE_FEED[] PROGMEM = IO_USERNAME "/feeds/marcellus-library-water-quality-data.voltage";

// object to publish sensor readings to adafruit IO feeds
//Adafruit_MQTT_Publish Depth = Adafruit_MQTT_Publish(&mqtt, DEPTH_FEED);
//Adafruit_MQTT_Publish Turbidity = Adafruit_MQTT_Publish(&mqtt, TURBIDITY_FEED);
//Adafruit_MQTT_Publish Temperature = Adafruit_MQTT_Publish(&mqtt, TEMPERATURE_FEED);
//Adafruit_MQTT_Publish Voltage = Adafruit_MQTT_Publish(&mqtt, VOLTAGE_FEED);
void MQTT_connect() {
    int8_t ret;
 
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
void setup() {
  //WiFi pins specific to the Adafruit Feather M0 WiFi - ATSAMD21 + ATWINC1500 breakout board
  WiFi.setPins(8,7,4,2);
  
  // start the serial connection, sets rate of data transmission
  Serial.begin(115200);

  // wait for serial monitor to open
  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    Serial.print(F("Attempting to connect to open SSID: "));
    Serial.println(WIFI_SSID);
    status = WiFi.begin(WIFI_SSID,WIFI_PASS); 
    // wait 10 seconds for connection:
    delay(10000);
  }
  // you're connected now, so print out the data:
  Serial.print(F("You're connected to the network"));

  Serial.println(" ");
  Serial.print("Connecting to Adafruit IO");
  //io.connect();
  MQTT_connect();
  // wait for a connection
  /*while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }*/

  // we are connected
  Serial.println("Connected to Adafruit IO");
  Serial.println();
  Serial.println(io.statusText());
}

void loop() 
{
  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  // io.run();
  // Checking if still connected to AIO
  //if (io.status() < AIO_CONNECTED);
    //Serial.print("Connection Lost");
    //Serial.print("Re-Connecting to AIO");
    //io.connect();
  //Serial.println(io.statusText());
  
  // grab the current state of the depth reading pin
  current = analogRead (A0);
  // save the current state to the analog feed "Depth"
  Serial.println(" ");
  Serial.print("sending -> Depth: ");
  Serial.print(" ");
  Serial.println(current);
  Depth->save(current);
  // store last state of the depth reading pin
  last = current;
  
  // grab the current state of the turbidity reading pin
  current = analogRead(A1);
  // save the current state to the analog feed "Turbidity"
  Serial.println(" ");
  Serial.print("sending -> Turbidity: "); //Maybe Temp
  Serial.print(" ");
  Serial.println(current);
  Turb->save(current);
  // store last state of the turbidity reading pin
  last = current;

  // grab the current state of the temperature reading pint
  current = analogRead(A2);
  // save the current state to the analog feed "Temperature"
  Serial.println(" ");
  Serial.print("sending -> Temperature"); // Maybe Turb
  Serial.print(" ");
  Serial.println(current);
  Temp->save(current);
  
  
  // store last state of the temperature reading pin
  last = current;

  // grab the current state of the voltage reading pin
  current = analogRead(A3);
  // save the current state to the analog feed "voltage"
  Serial.print(" ");
  Serial.print("sending -> Voltage");
  Serial.println(current);
  Serial.println(" ");
  Volt->save(current);
  // store last state of the voltage reading pin
  last = current;

  // for printing in the Serial monitor
  // read the input on analog pin A0 A1 A2 A3:
  int Depth_pin = analogRead(A0);
  int Turb_pin = analogRead(A1); 
  int Temp_pin = analogRead(A2);
  int Volt_pin = analogRead(A3);
  // print out the value you read:
  //setTime(1,1,1,1,1,2000);
  Serial.print(Depth_pin);
  Serial.print(" ");
  Serial.print(Turb_pin);  
  Serial.print(" ");
  Serial.println(Temp_pin);
  Serial.print(" ");
  Serial.print(Volt_pin);
  Serial.print(" ");
  // datetime = {int day(),int hour(), int minute(), int second()}
  // const int analog_val[] = {datetime,sensorValue,sensorValue2,sensorValue3,sensorValue4};
  delay(5000);
}
  if(Volt_pint < 2.7 && statusCheck == false);
   {
       if(sendEmail()) Serial.println("Email sent");    
       else Serial.println("Email failed");
        statusCheck = true;
   }
    
 } 
byte sendEmail()
{
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
 
  Serial.println(F("Sending Hello"));
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
 
  client.println("Subject: Marcellus Library Datalogger ALERT!!\r\n");
 
  client.println("The datalogger at the Marcellus Library site is low on battery. Action must be taken shortly to assure consistent data collection");
 
  client.println(".");
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
 
  while(!client.available()) {
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
 
  while(client.available())
  {  
    thisByte = client.read();    
    Serial.write(thisByte);
  }
 
  if(respCode >= '4')
  {
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
 
  while(client.available())
  {  
    thisByte = client.read();    
    Serial.write(thisByte);
  }
 
  client.stop();
 
  Serial.println(F("disconnected"));

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print(F("SSID: Projects "));
  Serial.println(WiFi.SSID());
 
  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);    
  Serial.print(F("BSSID: "));
  Serial.print(bssid[5],HEX);
  Serial.print(F(":"));
  Serial.print(bssid[4],HEX);
  Serial.print(F(":"));
  Serial.print(bssid[3],HEX);
  Serial.print(F(":"));
  Serial.print(bssid[2],HEX);
  Serial.print(F(":"));
  Serial.print(bssid[1],HEX);
  Serial.print(F(":"));
  Serial.println(bssid[0],HEX);
 
  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print(F("signal strength (RSSI):"));
  Serial.println(rssi);
 
  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print(F("Encryption Type:"));
  Serial.println(encryption,HEX);
}
 
void printWifiData() {
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
    Serial.print(F("IP Address: "));
  Serial.println(ip);
  Serial.println(ip);
 
  // print your MAC address:
  byte mac[6];  
  WiFi.macAddress(mac);
  Serial.print(F("MAC address: "));
  Serial.print(mac[5],HEX);
  Serial.print(F(":"));
  Serial.print(mac[4],HEX);
  Serial.print(F(":"));
  Serial.print(mac[3],HEX);
  Serial.print(F(":"));
  Serial.print(mac[2],HEX);
  Serial.print(F(":"));
  Serial.print(mac[1],HEX);
  Serial.print(F(":"));
  Serial.println(mac[0],HEX);
 
  // print your subnet mask:
  IPAddress subnet = WiFi.subnetMask();
  Serial.print(F("NetMask: "));
  Serial.println(subnet);
 
  // print your gateway address:
  IPAddress gateway = WiFi.gatewayIP();
  Serial.print(F("Gateway: "));
  Serial.println(gateway);
}