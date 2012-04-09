/*
    Web client
 
 This sketch connects to a website (http://www.google.com)
 using an Arduino Wiznet Ethernet shield. 
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 
 created 18 Dec 2009
 by David A. Mellis
 
 */
#include <Wire.h>
#include <SPI.h>
#include <Ethernet.h>
#include "RTClib.h"
#include <SD.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x03 };
IPAddress server(69,89,31,63); // my IP server
const int requestInterval = 30000;
long lastAttemptTime = 0;            // last time you connected to the server, in milliseconds
boolean requested;
const int resetLED = 13;
float temp;
float voltage;
// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):


const int chipSelect = 4;//changed from 8
const int LOCATION_FILE_NUMBER_LSB = 0x00;
const int LOCATION_FILE_NUMBER_MSB = 0x01;

File dataFile;
RTC_DS1307 RTC;
EthernetClient client;
DateTime now;

void setup() {
  // start the serial library:
  Serial.begin(9600);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  // A2 is the ground, A3 is the power:
  digitalWrite(A2, LOW);
  digitalWrite(A3, HIGH);
    pinMode(10, OUTPUT);

  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:

  }
  Serial.println("card initialized.");


  Wire.begin();
  RTC.begin();
  delay(50);
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  dataFile = SD.open("data.txt", FILE_WRITE);
  delay(500);

  // start the Ethernet connection:
  Ethernet.begin(mac);
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    for(;;)
      ;
  }



  // connectToServer();
  // give the Ethernet shield a second to initialize:
  delay(1500);
  blink(resetLED, 3);
  Serial.println("connecting...");
  connectToServer();
}

void loop()
{

  now = RTC.now(); 
  if(client.connected()){
    if(!requested){

      requested = makeRequest(); 
   //   Serial.println("requesting!");
    }
    if(millis() - lastAttemptTime>requestInterval){
      //if youre not connected and two minutes have passed, attempt to connect again
      client.stop();
    //  Serial.println("stopping and reconnecting!");

     // getData();
      delay(1500);
      //connectToServer();
    }
    // if there are incoming bytes available 
    // from the server, read them and print them:

  }

  if (client.available()) {
    char c = client.read();
  //  Serial.print(c);
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
   // Serial.println();
  //  Serial.println("disconnecting.");

    client.stop();
    delay(1500);
 if(millis() - lastAttemptTime>requestInterval){
      //if youre not connected and two minutes have passed, attempt to connect again
    connectToServer();
    //try to reconnect here...
 }
  }


}

void getData(){
  voltage = 5 * analogRead(A0) / 1024.0;
  //float temp = 5 * analogRead(A1) / 1024.0;
  temp=(analogRead(A1))/10;

//  Serial.print(voltage);
//  Serial.print(F(","));
//  Serial.print(temp);
//  Serial.print(F(","));
//  Serial.println("   ");
//  
    if (dataFile) {
          DateTime now = RTC.now();
          
  dataFile.print(now.month());
  dataFile.print('/');
  dataFile.print(now.day());
  dataFile.print('/');
  dataFile.print(now.year());
  dataFile.print(F(","));
  dataFile.print(now.hour());
  dataFile.print(F(":"));
  dataFile.print(now.minute());
  dataFile.print(F(":"));
  dataFile.print(now.second());
  dataFile.print(F(","));
  dataFile.print(voltage);
  dataFile.print(F(","));
  dataFile.print(temp);
  dataFile.println();
          
    }
      dataFile.flush();

}

void connectToServer(){
 // Serial.println("connecting to server...");
  if (client.connect(server, 80)) {
    requested = false;
  }
 
  lastAttemptTime = millis();
}

boolean makeRequest() {
 // Serial.println("requesting");
  getData();
  // Make a HTTP request:
 
  client.print("GET /understanding_networks/dataLogger.php?data=");
  client.print(now.month());
  client.print('/');
  client.print(now.day());
  client.print('/');
  client.print(now.year());
  client.print(F(","));
  client.print(now.hour());
  client.print(F(":"));
  client.print(now.minute());
  client.print(F(":"));
  client.print(now.second());
  client.print(F(","));
  client.print(voltage);
  client.print(F(","));
  client.print(temp);
  client.println(" HTTP/1.1 ");
  client.println("HOST: www.levinegabriella.com");

  client.println();
  return true;

}

void blink(int thisPin, int howManyTimes){
  for (int blinks = 0;blinks<howManyTimes;blinks++){
    digitalWrite(thisPin, HIGH);
    delay(200);
    digitalWrite(thisPin, LOW);
    delay(200);
  } 
}

//questions: what other sensors will be good? 
//what is too much data
//i seem to be making my request twice


//when my request interval is more than 5000 i get 
//10/13/2011,11:14:56,3.67,15.00
//10/13/2011,11:15:5,3.65,15.00
//10/13/2011,11:15:16,0.00,0.00
//10/13/2011,11:15:44,0.00,0.00
//why

//can you go back a directory

