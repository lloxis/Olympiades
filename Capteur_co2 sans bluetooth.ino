/*
  SD card read/write
 This example shows how to read and write data to and from an SD card file
 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)
 created   Nov 2010
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe
 This example code is in the public domain.
 */

#include <SPI.h>
#include <SD.h>
#include "RTClib.h"
#include <Wire.h>
#include "rgb_lcd.h"

rgb_lcd lcd;

File myFile;
int sensorIn = A0;
int NumeroMesure=1;

float concentration;

float mesureDepart;
bool mesureDepartIsSet=false;

RTC_DS3231 rtc;

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  
  delay(1000);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  Serial.print("Initializing RTC...");

  rtc.begin();

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("RTC lost power,");
    lcd.setCursor(0, 1);
    lcd.print("time reseted");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    }

  Serial.println("RTC initialization done.");
}


void loop() {
  Serial.print("Initializing SD card...");
  if (!SD.begin(4)) {
    Serial.println("SD card initialization failed!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Init carte SD");
    lcd.setCursor(0, 1);
    lcd.print("echouee !");
    while (1);
  }
  Serial.println("SD card initialization done.");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Prise de mesures");

  float average=0;
  for (int i=1; i <= 10; i++) {
    lcd.setCursor(0, 1);
    lcd.print(String(i) + " sur 10");
    //Read voltage
    int sensorValue = analogRead(sensorIn);
  
    // The analog signal is converted to a voltage
    float voltage = sensorValue*(5000/1024.0);
  
    int voltage_diference=voltage-400;
    concentration=voltage_diference*50.0/16.0;
    average += concentration;
    delay(1000);
  }
  average = average/10;
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CO2: "+String(average)+" ppm");
  /*if (mesureDepartIsSet == true) {
      Serial.println("ok");
      lcd.print(" %:"+String(concentration/mesureDepart*100));
    }
  if (MinBtwMesure*(NumeroMesure-1) >= 5) {
      Serial.println("set");
      float mesureDepart = concentration;
      mesureDepartIsSet=true;
    }*/
  
  
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("mesure.csv", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    DateTime now = rtc.now(); //get current time
    //String printString = String(now.year()) + "/" + String(now.month()) + "/" + String(now.day()) + ";" + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()) + ";" + String(NumeroMesure) + ";" + String(NumeroMesure*MinBtwMesure) + ";" + String(average);
  
    Serial.print("Writing to mesure.csv...");
    myFile.println(String(now.year()) + "/" + String(now.month()) + "/" + String(now.day()) + ";" + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()) + ";" + String(NumeroMesure) + ";" + String(NumeroMesure*3) + ";" + String(average));
    Serial.print(String(now.year()) + "/" + String(now.month()) + "/" + String(now.day()) + ";" + String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()) + ";" + String(NumeroMesure) + ";" + String(NumeroMesure*3) + ";" + String(average));
    // close the file:
    myFile.close();
    Serial.println("done.");
    //HC06.write((char *) &average);
    //char a[sizeof(float)];
    //memcpy(a, &average, sizeof(float));
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening mesure.csv !");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("échec ouverture");
    lcd.setCursor(0, 1);
    lcd.print("du ficher !");
  }
  NumeroMesure+=1;
  delay(3*60000-10000);
}
