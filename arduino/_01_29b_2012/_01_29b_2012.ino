#include <i2cmaster.h>
#include "Arduino.h"
#include "SoftwareSerial.h"

//////////////////////////////
/////     GPS             ////
//////////////////////////////
// Use pin 4 to control power to the GPS
#define powerpin 4
// Use pins 2 and 3 to talk to the GPS. 2 is the TX pin, 3 is the RX pin
SoftwareSerial mySerial = SoftwareSerial(2, 3);
// Set the GPSRATE to the baud rate of the GPS module. Most are 4800
#define GPSRATE 4800
// The buffer size that will hold a GPS sentence. They tend to be 80 characters long
// so 90 is plenty.
#define BUFFSIZ 90 // plenty big
// global variables
char buffer[BUFFSIZ];        // string buffer for the sentence
char *parseptr;              // a character pointer for parsing
char buffidx;                // an indexer into the buffer

// The time, date, location data, etc.
uint8_t hour, minute, second, year, month, date;
uint32_t latitude, longitude;
uint8_t groundspeed, trackangle;
char latdir, longdir;
char status;

//////////////////////////////
/////     Display         ////
//////////////////////////////
// Use pin 6 to control the display
//#define txPin 6
// since the LCD does not send data back to the Arduino, we should only define the txPin
//SoftwareSerial LCD = SoftwareSerial(0, txPin);
//const int LCDdelay=10;  // conservative, 2 actually works
// wbp: goto with row & column

//////////////////////////////
//// Momentary and speaker ///
//////////////////////////////

#define buttonPin 5    // the number of the pushbutton pin
#define ledPin 13      // the number of the LED pin
#define sensorPin A0   // the number of the pot pin
#define TONE_PIN 9     // the number of the speaker pin
float sensorValue = 0;
float secondValue = 0;

long previousMillis = 0;
long interval = 1000;

int buttonState = 0;

char *song = "smbSHORT:d=4,o=5,b=100:16e6,16e6,32p,8e6,16c6,8e6,8g6,8p,8g,8p"; 


void setup(){

  // Start serial port so you can write to the screen/datalogger  
  Serial.begin(9600);
  Serial.println("Setup...");

  //////////////////////////////
  /////     GPS             ////
  //////////////////////////////
  //this sets the GPS power pin as output so you can turn it on and off
  if (powerpin) {
    pinMode(powerpin, OUTPUT);
  }
  digitalWrite(powerpin, HIGH);         // pull high to turn off! so everything else can setup

  // connect to the GPS at the desired rate
  mySerial.begin(GPSRATE);
  // prints title with ending line break 
  Serial.println("GPS parser"); 

  //////////////////////////////
  /////     Display         ////
  //////////////////////////////

  //  LCD.begin(9600);
  //  Serial.println("LCD ready); 
  //  pinMode(txPin, OUTPUT);
  //
  //  clearLCD();
  //  lcdPosition(0,0);
  //  LCD.print("Hello world!");


  //////////////////////////////
  /////     Thermal         ////
  //////////////////////////////
  i2c_init(); //Initialise the i2c bus
  PORTC = (1 << PORTC4) | (1 << PORTC5);//enable pullups


  //////////////////////////////
  //// Momentary and speaker ///
  //////////////////////////////

  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  // play the song!
  play_rtttl(song);

  delay(1000);

  // This will turn the GPS on
  digitalWrite(powerpin, LOW);         // pull low to turn on!

}


void loop(){


  unsigned long currentMillis = millis();
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);
  // read the pot's setting
  sensorValue = analogRead(sensorPin);

  // convert from 8bit to a number, then convert to a number of seconds to delay
  secondValue = (sensorValue/1023) *3.0 * 1000;
  //  Serial.println(secondValue);


  if(currentMillis - previousMillis > secondValue) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis; 
    tone(TONE_PIN, 600);
    delay(10);
    noTone(TONE_PIN);
  }

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (buttonState == HIGH) {     
    // turn LED on:    
    digitalWrite(ledPin, HIGH);
    Serial.print("***");
  } 
  else {
    // turn LED off:
    digitalWrite(ledPin, LOW); 
  }


  /////////////////////////////////////////////
  /////////////////////////////////////////////
  //// EM-406 /////////////////////////////////
  /////////////////////////////////////////////
  /////////////////////////////////////////////

  uint32_t tmp;

  // Serial.print("\n\rRead: ");
  readline();

  // check if $GPRMC (global positioning fixed data)
  if (strncmp(buffer, "$GPRMC",6) == 0) {

    // hhmmss time data
    parseptr = buffer+7;
    tmp = parsedecimal(parseptr); 
    hour = tmp / 10000;
    minute = (tmp / 100) % 100;
    second = tmp % 100;

    parseptr = strchr(parseptr, ',') + 1;
    status = parseptr[0];
    parseptr += 2;

    // grab latitude & long data
    // latitude
    latitude = parsedecimal(parseptr);
    if (latitude != 0) {
      latitude *= 10000;
      parseptr = strchr(parseptr, '.')+1;
      latitude += parsedecimal(parseptr);
    }
    parseptr = strchr(parseptr, ',') + 1;
    // read latitude N/S data
    if (parseptr[0] != ',') {
      latdir = parseptr[0];
    }

    //Serial.println(latdir);

    // longitude
    parseptr = strchr(parseptr, ',')+1;
    longitude = parsedecimal(parseptr);
    if (longitude != 0) {
      longitude *= 10000;
      parseptr = strchr(parseptr, '.')+1;
      longitude += parsedecimal(parseptr);
    }
    parseptr = strchr(parseptr, ',')+1;
    // read longitude E/W data
    if (parseptr[0] != ',') {
      longdir = parseptr[0];
    }


    // groundspeed
    parseptr = strchr(parseptr, ',')+1;
    groundspeed = parsedecimal(parseptr);

    // track angle
    parseptr = strchr(parseptr, ',')+1;
    trackangle = parsedecimal(parseptr);


    // date
    parseptr = strchr(parseptr, ',')+1;
    tmp = parsedecimal(parseptr); 
    date = tmp / 10000;
    month = (tmp / 100) % 100;
    year = tmp % 100;

    Serial.print("\n\tTime: ");
    Serial.print(hour, DEC); 
    Serial.print(':');
    Serial.print(minute, DEC); 
    Serial.print(':');
    Serial.print(second, DEC);
    Serial.print("\tDate: ");
    Serial.print(month, DEC); 
    Serial.print('/');
    Serial.print(date, DEC); 
    Serial.print('/');
    Serial.print(year, DEC);

    Serial.print("\tLat: "); 
    if (latdir == 'N')
      Serial.print('+');
    else if (latdir == 'S')
      Serial.print('-');

    Serial.print(latitude/1000000, DEC); 
    Serial.print("* ");
    Serial.print((latitude/10000)%100, DEC); 
    Serial.print('\''); 
    Serial.print(' ');
    Serial.print((latitude%10000)*6/1000, DEC); 
    Serial.print('.');
    Serial.print(((latitude%10000)*6/10)%100, DEC); 
    Serial.print('"');

    Serial.print("\tLong: ");
    if (longdir == 'E')
      Serial.print('+');
    else if (longdir == 'W')
      Serial.print('-');
    Serial.print(longitude/1000000, DEC); 
    Serial.print("* ");
    Serial.print((longitude/10000)%100, DEC); 
    Serial.print('\''); 
    Serial.print(' ');
    Serial.print((longitude%10000)*6/1000, DEC); 
    Serial.print('.');
    Serial.print(((longitude%10000)*6/10)%100, DEC); 
    Serial.println('"');

  }
  //Serial.println(buffer);


  /////////////////////////////////////////////
  /////////////////////////////////////////////
  //// MLX9061-acc/////////////////////////////
  /////////////////////////////////////////////
  /////////////////////////////////////////////

  float celcius = mlx9600();

  Serial.print("\tCelcius: ");
  Serial.println(celcius);

  //  LCD.print("lat");


}









