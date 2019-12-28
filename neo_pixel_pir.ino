//pir sensor ws2812 neo xmas
#include <Wire.h>
#include "RTClib.h"
#include <Adafruit_NeoPixel.h>

RTC_DS1307 rtc;

// probably don't need this end
#define PIN 8
// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
Adafruit_NeoPixel strip = Adafruit_NeoPixel(30, PIN, NEO_GRB + NEO_KHZ800);
/*
 * How to use a PIR sensor with the Arduino
http://www.electronicslovers.com/2015/08/how-to-use-pir-sensor-with-arduino.html
 * The sensor's output pin goes to HIGH if motion is present.
 * However, even if motion is present it goes to LOW from time to time, 
 * which might give the impression no motion is present. 
 * This program deals with this issue by ignoring LOW-phases shorter than a given time, 
 * assuming continuous motion is present during these phases.
*/
//the time we give the sensor to calibrate (10-60 secs according to the datasheet)
int calibrationTime = 10;         
//the time when the sensor outputs a low impulse
long unsigned int lowIn;         
//the amount of milliseconds the sensor has to be low 
//before we assume all motion has stopped
long unsigned int pause = 5000; 

 
boolean lockLow = true;
boolean takeLowTime;  
int pirPin = 7;    //the digital pin connected to the PIR sensor's output
int ledPin = 8; 

const unsigned long motionDelay = 1; //the number of minutes the LEDs stay on if motion is detected
unsigned long currentMillis = 0;  //used to store ms since system was turned on
unsigned long prevMillis = 0;  //used to count elapsed time
bool ledState = false;  //stores whether the LEDs are on or off
bool motionState = false, offState = true; //states used to change what activates the LEDs according to daily cycle
const int onHourAM = 6, onMinuteAM = 30, offHourAM = 8, offMinuteAM = 30;   //morning time
const int onHourPM = 20, onMinutePM = 0, offHourPM = 21, offMinutePM = 10;  //evening time

void setup(){
  Serial.begin(9600);
  strip.begin();
  strip.show();
  
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);
  
  digitalWrite(pirPin, LOW);
  //give the sensor some time to calibrate
  Serial.print("calibrating sensor please wait for 10 secs ");
    for(int i = 0; i < calibrationTime; i++){
      Serial.print(".");
      delay(1000);
      }
    Serial.println(" done");
    Serial.println("SENSOR ACTIVE");
    Serial.print("pirPin =");
    Serial.println(pirPin);
    delay(50);

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  //if RTC hasn't started yet initalize time values from PC
  //Used to set RTC with a give date and time. Used for debugging
  //If you want the time to be January 21, 2014 at 3am you would call:  rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0)
  //rtc.adjust(DateTime(2016, 9, 28, 18, 20, 50));

  
    
  }
////////////////////////////
//LOOP

void loop(){
    
     if(digitalRead(pirPin) == HIGH){
       digitalWrite(ledPin, HIGH);   //the led visualizes the sensors output pin state
       colorWipe(strip.Color(255, 255, 255), 10); // White
        colorWipe(strip.Color(0, 255, 0), 10); // Green  

       if(lockLow){ 
         //makes sure we wait for a transition to LOW before any further output is made:
         lockLow = false;            
         Serial.println("---");
         Serial.print("motion detected at ");
         Serial.print(millis()/1000);
         Serial.println(" sec"); 
         delay(50);
         }         
         takeLowTime = true;
       }

     if(digitalRead(pirPin) == LOW){ 
       digitalWrite(ledPin, LOW);  //the led visualizes the sensors output pin state

       if(takeLowTime){
        colorWipe(strip.Color(0, 0, 0), 10); // OFF
        lowIn = millis();          //save the time of the transition from high to LOW
        takeLowTime = false;       //make sure this is only done at the start of a LOW phase
        }
       //if the sensor is low for more than the given pause, 
       //we assume that no more motion is going to happen
       if(!lockLow && millis() - lowIn > pause){  
           //makes sure this block of code is only executed again after 
           //a new motion sequence has been detected
           lockLow = true;                        
           Serial.print("motion ended at ");      //output
           Serial.print((millis() - pause)/1000);
           Serial.println(" secs ");
           delay(50);
           }
       }
       
  }

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
    strip.show();
}
