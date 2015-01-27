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
  
  Program automatically arms ESC and waits for a pushbutton input.
  Hitting the button once starts the motor spinning at runSpeed for
  duration timeTest (seconds). Stops when (a) time is done or (b)
  button is pressed again. Refreshes value at rate refreshRate,
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
int escPin = 5;
Servo escOut;

// Output parameters
long timeSinceStart = 0;
int pwmOut = 15;
double tempCelsius = 0;
double tempRefCelsius = 0;

// Test parameters
long timeTest = 120;
int runSpeed = 110;
long timeTestStart;

/*
  Mode 1 = Stop (red LED on); Mode 2 = Run (green LED on). prevButtonTime, prevButtonStatus
  and debounceTime are used for button debounce.
*/
int mode = 1;
int greenPin = 13;
int redPin = 12;
int buttonPin = 11;
long prevButtonTime = 0;
int debounceTime = 10;
boolean prevButtonStatus = LOW;

/*
  nextTick is used to ensure that each clock cycle happens once, since the update rate
  of loop() is uncontrolled.
*/
boolean nextTick = true;
int refreshRate = 500;

void setup() {
  Serial.begin(9600);
  Serial.println("testThermalPerformance");
  Serial.println("timeSinceStart,pwmOut,tempCelsius,tempRefCelsius");
  
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  escOut.attach(escPin);
  escOut.write(pwmOut);
  // wait for MAX chip to stabilize
  delay(5000);
}

void loop() {
  timeSinceStart = millis();
  escOut.write(pwmOut);
  
  if (mode == 1) {
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, LOW);
    pwmOut = 15;
  }
  if (mode == 2) {
    digitalWrite(redPin, LOW);
    digitalWrite(greenPin, HIGH);
    pwmOut = runSpeed;
  }
  
  if (timeSinceStart - timeTestStart > (timeTest * 1000)) {
    mode = 1;
  }
    
  if ((digitalRead(buttonPin) == HIGH) && (prevButtonStatus == LOW) && ((timeSinceStart - prevButtonTime) > debounceTime)) {
    if (mode == 2) {
      mode = 1; 
    }
    else {
      mode = 2;
      timeTestStart = timeSinceStart;
    }
    prevButtonTime = timeSinceStart;
  }

/*
  everything time-critical should happen within this if() statement, since this is where 
  execution rate is controlled via refreshRate.
*/
  if ((timeSinceStart % refreshRate == 0) && (nextTick == true)) {
    tempCelsius = thermocouple.readCelsius();
    tempRefCelsius = thermocouple.readInternal();  
    
    updateSerial(timeSinceStart,pwmOut,tempCelsius,tempRefCelsius);
    nextTick = false;
    //escOut.write((tempCelsius - 20) * 8);
  }

/*
  this if statement just checks to see that time has moved on since the last execution
  of the timed loop. Deals with millis() values repeating themselves--can that happen?
*/
  if ((nextTick == false) && (timeSinceStart % refreshRate > 0)) {
    nextTick = true;
  }
  prevButtonStatus = digitalRead(buttonPin);
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
