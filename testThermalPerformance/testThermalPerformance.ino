/*
  Zach Fredin
  zachary.fredin@gmail.com
  github.com/zakqwy
  
  Thermal performance tester for GimbalBot. Uses Adafruit's MAX31855
  breakout board and library to get temperature readings from a
  Type K thermocouple. TC is mounted with Kapton tape in groove on
  the back of one of GimbalBot's motors. 
  
  Datalogs a bit of info via serial connection in *.csv format:
  
  timeSinceStart,pwmOut,tempCelsius,tempRefCelsius
  
  timeSinceStart = time in milliseconds since program start
  pwmOut = servo signal sent to ESC in degrees (0-179)
  tempCelsius = TC temperature reading in C
  tempRefCelsius = MAX reference temp in C
  
  Program automatically arms ESC then ramps up to runMax. Maintains
  this value until T = tempMax. Refreshes value at rate refreshRate,
  which is a delay measured in ms.
*/

/*************************************************** 
  This is an example for the Adafruit Thermocouple Sensor w/MAX31855K

  Designed specifically to work with the Adafruit Thermocouple Sensor
  ----> https://www.adafruit.com/products/269

  These displays use SPI to communicate, 3 pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
****************************************************/
 
#include "Adafruit_MAX31855.h"
#include <Servo.h>
 
// Adafruit MAX31855 initialization
int thermoDO = 9;
int thermoCS = 8;
int thermoCLK = 7;
Adafruit_MAX31855 thermocouple(thermoCLK, thermoCS, thermoDO);

// ESC initialization
int escPin = 10;
int escStart = 90;
Servo escOut;

long timeSinceStart = 0;
int pwmOut = 0;
double tempCelsius = 0;
double tempRefCelsius = 0;

/*
  nextTick is used to ensure that each clock cycle happens once, since the update rate
  of loop() is uncontrolled.
*/
boolean nextTick = true;

int refreshRate = 500;

void setup() {
  Serial.begin(9600);
  Serial.println("testThermal Performance");
  Serial.println("GimbalBot, Sparkfun Arduino Pro Mini 5V, Adafruit Thermocouple Sensor w/MAX31855K, KDE ESCs");
  Serial.println("timeSinceStart,pwmOut,tempCelsius,tempRefCelsius");
  
  escOut.attach(escPin);
  escOut.write(escStart);
  // wait for MAX chip to stabilize
  delay(5);
}

void loop() {
//  timeSinceStart = millis();
  
/*
  everything time-critical should happen within this if() statement, since this is where 
  execution rate is controlled via refreshRate.
*/
//  if ((timeSinceStart % refreshRate == 0) && (nextTick == true)) {
    tempCelsius = thermocouple.readCelsius();
//    tempRefCelsius = thermocouple.readInternal();  
    
    //updateSerial(timeSinceStart,pwmOut,tempCelsius,tempRefCelsius);
//    nextTick = false;
    escOut.write((tempCelsius - 20) * 8);
 // }

/*
  this if statement just checks to see that time has moved on since the last execution
  of the timed loop. Deals with millis() values repeating themselves--can that happen?
*/
//  if ((nextTick == false) && (timeSinceStart % refreshRate > 0)) {
//    nextTick = true;
//  }
}

void updateSerial(long timeSinceStart,int pwmOut,double tempCelsius,double tempRefCelsius) {
  Serial.print(timeSinceStart);
  Serial.print(",");
  Serial.print(pwmOut);
  Serial.print(",");
  Serial.print(tempCelsius);
  Serial.print(",");
  Serial.println(tempRefCelsius);  
}
