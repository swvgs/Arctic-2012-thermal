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
#define txPin 6
// since the LCD does not send data back to the Arduino, we should only define the txPin
SoftwareSerial LCD = SoftwareSerial(0, txPin);
const int LCDdelay=10;  // conservative, 2 actually works
// wbp: goto with row & column



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
  
  LCD.begin(9600);
  Serial.println("LCD ready); 
  
  // connect to the GPS at the desired rate
  mySerial.begin(GPSRATE);
  // prints title with ending line break 
  Serial.println("GPS parser"); 

  // This will turn the GPS on
  digitalWrite(powerpin, LOW);         // pull low to turn on!


  //////////////////////////////
  /////     Display         ////
  //////////////////////////////
  

  pinMode(txPin, OUTPUT);

  clearLCD();
  lcdPosition(0,0);
  LCD.print("Hello world!");


  //////////////////////////////
  /////     Thermal         ////
  //////////////////////////////
  i2c_init(); //Initialise the i2c bus
  PORTC = (1 << PORTC4) | (1 << PORTC5);//enable pullups


  // Use the pin 13 LED as an indicator
  pinMode(13, OUTPUT);


}


//////////////////////////////
/////     GPS             ////
//////////////////////////////

uint32_t parsedecimal(char *str) {
  uint32_t d = 0;

  while (str[0] != 0) {
    if ((str[0] > '9') || (str[0] < '0'))
      return d;
    d *= 10;
    d += str[0] - '0';
    str++;
  }
  return d;
}

void readline(void) {
  char c;

  buffidx = 0; // start at begninning
  while (1) {
    c=mySerial.read();
    if (c == -1)
      continue;
    Serial.print(c);
    if (c == '\n')
      continue;
    if ((buffidx == BUFFSIZ-1) || (c == '\r')) {
      buffer[buffidx] = 0;
      return;
    }
    buffer[buffidx++]= c;
  }
}


//////////////////////////////
/////     Display         ////
//////////////////////////////
void lcdPosition(int row, int col) {
  LCD.write(0xFE);   //command flag
  LCD.write((col + row*64 + 128));    //position 
  delay(LCDdelay);
}
void clearLCD(){
  LCD.write(0xFE);   //command flag
  LCD.write(0x01);   //clear command.
  delay(LCDdelay);
}
void backlightOn() {  //turns on the backlight
  LCD.write(0x7C);   //command flag for backlight stuff
  LCD.write(157);    //light level.
  delay(LCDdelay);
}
void backlightOff(){  //turns off the backlight
  LCD.write(0x7C);   //command flag for backlight stuff
  LCD.write(128);     //light level for off.
  delay(LCDdelay);
}
void serCommand(){   //a general function to call the command flag for issuing all other commands   
  LCD.write(0xFE);
}


void loop(){


  /////////////////////////////////////////////
  /////////////////////////////////////////////
  //// EM-406 /////////////////////////////////
  /////////////////////////////////////////////
  /////////////////////////////////////////////

  uint32_t tmp;

  Serial.print("\n\rRead: ");
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
    Serial.println(second, DEC);
    Serial.print("\tDate: ");
    Serial.print(month, DEC); 
    Serial.print('/');
    Serial.print(date, DEC); 
    Serial.print('/');
    Serial.println(year, DEC);

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
    Serial.println('"');

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
  int dev = 0x5A<<1;
  int data_low = 0;
  int data_high = 0;
  int pec = 0;

  i2c_start_wait(dev+I2C_WRITE);
  i2c_write(0x07);

  // read
  i2c_rep_start(dev+I2C_READ);
  data_low = i2c_readAck(); //Read 1 byte and then send ack
  data_high = i2c_readAck(); //Read 1 byte and then send ack
  pec = i2c_readNak();
  i2c_stop();

  //This converts high and low bytes together and processes temperature, MSB is a error bit and is ignored for temps
  double tempFactor = 0.02; // 0.02 degrees per LSB (measurement resolution of the MLX90614)
  double tempData = 0x0000; // zero out the data
  int frac; // data past the decimal point

  // This masks off the error bit of the high byte, then moves it left 8 bits and adds the low byte.
  tempData = (double)(((data_high & 0x007F) << 8) + data_low);
  tempData = (tempData * tempFactor)-0.01;

  float celcius = tempData - 273.15;
  float fahrenheit = (celcius*1.8) + 32;

  Serial.print("Celcius: ");
  Serial.println(celcius);

  Serial.print("Fahrenheit: ");
  Serial.println(fahrenheit);

  LCD.print("lat");

  delay(1000); // wait a second before printing again
}

