/*
The thermal sensor seems to have broken and is causing the entire system to wait for it to come online
to fix that I replaced the sensor, not much else you can do.
Future versions might need test points to see if the wire is broken, the way that
the sensor is placed in the PVC I find it hard to belieave that the actual sensor
is broken, so it *must* be the cat3 cable that I am using. it is probably
not rated for this temperature range. 
We also had one USB connectors outer shell crack on us. The temperature seems
even more extreme than one would imagine. 

It would be good to have an easier way to swap sensors where they plug into the 
side of the enclosure. 
This also needs a pause data taking switch (non-momentary would probably be best)

*/

#include <i2cmaster.h>
//#include "Arduino.h"
#include "SoftwareSerial.h"
#include <TinyGPS.h>

//////////////////////////////
/////     GPS             ////
//////////////////////////////
#define RXPIN 2
#define TXPIN 3
// Use pin 4 to control power to the GPS
#define powerpin 4

TinyGPS gps;
// Use pins 2 and 3 to talk to the GPS. 2 is the TX pin, 3 is the RX pin
SoftwareSerial uart_gps(RXPIN, TXPIN);
// Set the GPSRATE to the baud rate of the GPS module. Most are 4800
#define GPSBAUD 4800



//////////////////////////////
/////     Display         ////
//////////////////////////////
// Use pin 6 to control the display
//#define LCDtxPin 6
// since the LCD does not send data back to the Arduino, we should only define the txPin
//SoftwareSerial LCD = SoftwareSerial(0, LCDtxPin);
//const int LCDdelay=10;  // conservative, 2 actually works wbp: goto with row & column

//////////////////////////////
//// Momentary and speaker ///
//////////////////////////////

#define buttonPin 5    // the number of the pushbutton pin
#define ledPin 13      // the number of the LED pin
#define sensorPin A0   // the number of the pot pin
#define TONE_PIN 9     // the number of the speaker pin
float sensorValue = 0;
//float secondValue = 0;

long previousMillis = 0;
long interval = 1000;

int buttonState = 0;

char *song = "smbSHORT:d=4,o=5,b=100:16e6,16e6,32p,8e6,16c6,8e6,8g6,8p,8g,8p"; 

//////////////////////////////
//// Thermister            ///
//////////////////////////////

#include <math.h>

#define ThermistorPIN 0                 // Analog Pin 0

float vcc = 4.82;                       // only used for display purposes, if used
// set to the measured Vcc.
float pad = 10050;                       // balance/pad resistor value, set this to
// the measured resistance of your pad resistor
float thermr = 10000;                   // thermistor nominal resistance
float ambiant = 0.0;


void setup(){

  // Start serial port so you can write to the screen/datalogger  
  Serial.begin(9600);
  Serial.println("Setup...");

  //////////////////////////////
  /////     GPS             ////
  //////////////////////////////
  Serial.println("Enable GPS...");
  uart_gps.begin(GPSBAUD);
  //this sets the GPS power pin as output so you can turn it on and off
  if (powerpin) {
    pinMode(powerpin, OUTPUT);
  }
  digitalWrite(powerpin, HIGH);         // pull high to turn off! so everything else can setup

  // prints title with ending line break 
  Serial.println("GPS Awaiting Lock"); 

  //////////////////////////////
  /////     Display         ////
  //////////////////////////////
  //  Serial.println("Enable LCD...");
  //  LCD.begin(9600);
  //  Serial.println("LCD ready"); 
  //  pinMode(LCDtxPin, OUTPUT);

  //  clearLCD();
  //  lcdPosition(0,0);
  //  LCD.print("Therma V2.2.17");

  //////////////////////////////
  /////     Thermal         ////
  //////////////////////////////
  Serial.println("Enable Thermal Sensor...");
  i2c_init(); //Initialise the i2c bus
  PORTC = (1 << PORTC4) | (1 << PORTC5);//enable pullups


  //////////////////////////////
  //// Momentary and speaker ///
  //////////////////////////////
  Serial.println("Enable Momentary...");
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  // play the song!
  Serial.println("Play startup song...");
  play_rtttl(song);

  delay(200);

  // This will turn the GPS on
  digitalWrite(powerpin, LOW);         // pull low to turn on!

  Serial.println("mark,time,ir,ambiant,lat,lon,mon,day,year,hr,min,sec,ms");
  //  clearLCD();
}


void loop(){


  unsigned long currentMillis = millis();
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);
  // read the pot's setting
  ambiant = Thermistor(analogRead(sensorPin));

  // convert from 8bit to a number, then convert to a number of seconds to delay
  //  secondValue = (sensorValue/1023) *3.0 * 1000;
  //  Serial.println(secondValue);


  //  if(currentMillis - previousMillis > secondValue) {
  //    // save the last time you blinked the LED 
  //    previousMillis = currentMillis;
  //    tone(TONE_PIN, 600);
  //    delay(10);
  //    noTone(TONE_PIN);
  //    //    clearLCD();
  //    //    lcdPosition(0,15);
  //    //    LCD.print(" ");
  //  }

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (buttonState == HIGH) {     
    // turn LED on:    
    digitalWrite(ledPin, HIGH);
    Serial.print("*,");
    //    lcdPosition(0,15);
    //    LCD.print("*");
  } 
  else {
    // turn LED off:
    digitalWrite(ledPin, LOW); 
    Serial.print(" ,");
    //    lcdPosition(0,15);
    //    LCD.print(" ");  
  }

  Serial.print(currentMillis);

  /////////////////////////////////////////////
  /////////////////////////////////////////////
  //// MLX9061-acc/////////////////////////////
  /////////////////////////////////////////////
  /////////////////////////////////////////////

  float celcius = mlx9600();

  Serial.print(",");
  Serial.print(celcius);
  Serial.print(",");
  Serial.print(ambiant);

  /////////////////////////////////////////////
  /////////////////////////////////////////////
  //// Display ////////////////////////////////
  /////////////////////////////////////////////
  /////////////////////////////////////////////


  //    lcdPosition(1,2); //(up/down,left/right)
  //  lcdPosition(0,0);
  //  LCD.print((char)223);
  //  lcdPosition(0,1);
  //  LCD.print("C:");
  //  lcdPosition(0,4);
  //  LCD.print(celcius);
  //  lcdPosition(1,0);
  //  LCD.print("Beat:");
  //  lcdPosition(1,8);
  //  LCD.print(secondValue);

  /////////////////////////////////////////////
  /////////////////////////////////////////////
  //// EM-406 /////////////////////////////////
  /////////////////////////////////////////////
  /////////////////////////////////////////////


  while(uart_gps.available())     // While there is data on the RX pin...
  {
    int c = uart_gps.read();    // load the data into a variable...
    if(gps.encode(c))      // if there is a new valid sentence...
    {
      getgps(gps);         // then grab the data.
    }
  }
  Serial.println("");
}





