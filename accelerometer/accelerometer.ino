    /*
  ADXL3xx

  Reads an Analog Devices ADXL3xx accelerometer and communicates the
  acceleration to the computer. The pins used are designed to be easily
  compatible with the breakout boards from SparkFun, available from:
  http://www.sparkfun.com/commerce/categories.php?c=80

  The circuit:
  - analog 0: accelerometer self test
  - analog 1: z-axis
  - analog 2: y-axis
  - analog 3: x-axis
  - analog 4: ground
  - analog 5: vcc

  created 2 Jul 2008
  by David A. Mellis
  modified 30 Aug 2011
  by Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/ADXL3xx
*/

#include <SoftwareSerial.h>
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"

RTC_DS1307 rtc;

#define CHIP_SELECT 10                         // For an Uno it's 10

SoftwareSerial SoftSerial(8, 7);
File logfile;

// these constants describe the pins. They won't change:
const int groundpin = 18;             // analog input pin 4 -- ground
const int powerpin = 19;              // analog input pin 5 -- voltage
const int xpin = A3;                  // x-axis of the accelerometer
const int ypin = A2;                  // y-axis
const int zpin = A1;                  // z-axis (only on 3-axis models)
const int buttonPin = 8;     // the number of the pushbutton pin
const int ledPin =  13;      // the number of the LED pin
const int DATA_NUMBER = 4;    //line number, date, current time, speed
const int MIN_TIME = 700;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status

// button variables
bool isButtonPressed = false;
unsigned long lastDebounceTime = 0;  
unsigned long debounceDelay = 150;

// data variables
int prevy = 0;
int startTime = 0;
int lineNumber = 0;


void setup() {
  // initialize the serial communications:
  Serial.begin(9600);

  //Set up Sd card
  Serial.println("Setting up file on SD card");
  setupSDFile();
  
  SoftSerial.begin(9600);                  // the SoftSerial baud rate

  //Real Time Clock Set Up
  while (!Serial); // for Leonardo/Micro/Zero

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // accelerometer
  pinMode(groundpin, OUTPUT);
  pinMode(powerpin, OUTPUT);
  digitalWrite(groundpin, LOW);
  digitalWrite(powerpin, HIGH);

  // LED pin
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
}

void loop() {  
  
  if((millis() - lastDebounceTime) > debounceDelay){
    buttonState = digitalRead(buttonPin);
    // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
    if (buttonState == HIGH) {
      if (isButtonPressed) {
        isButtonPressed = false;
        Serial.println("Stop reading");
        digitalWrite(ledPin, LOW);
      } else {
        Serial.println("Start reading");
        isButtonPressed = true;
        digitalWrite(ledPin, HIGH);
      }
    } 
    lastDebounceTime = millis();
  }

  if (isButtonPressed) {
    int currenty = analogRead(ypin);
    if (detectStrongChange(prevy, currenty)) {
      DateTime now = rtc.now();
      prevy = currenty;
      int readingTime;
      //Serial.println("Movement detected in y!");
      if (startTime == 0) {
        // This is the first movement. We start the timer.
        startTime = millis();
      } else {
        int current = millis();
        readingTime = current - startTime;
        if (readingTime >= MIN_TIME) {
          startTime = millis();
          lineNumber++;
          String realDate = String(now.day()) + '/' + String(now.month()) +'/' + String(now.year());
          String realTime = String(now.hour()) + ':'+ String(now.minute()) + ':' + String(now.second());
          String dataArr[DATA_NUMBER] = {String(lineNumber), realDate, realTime, String(float(readingTime)/ 1000)};
          writeToFile(dataArr);
        }
      }
    }
  }

  // delay before next reading:
  delay(100);

}

bool detectStrongChange(int prev, int current) {
  int diff = abs(current - prev);
  //Serial.println(diff);
  if (diff >= 2) {
    return true;
  } else {
    return false;
  }
}

//
// setupSDFile - Create and open for writing the file on the SD card
//
void setupSDFile()
{
  pinMode(10, OUTPUT);

  // see if the card is present and can be initialized:
  if (!SD.begin(CHIP_SELECT)) 
  {
    Serial.println("Card init. failed!");
  }

  char filename[15];
  strcpy(filename, "LOGFIL00.TXT");
  for (uint8_t i = 0; i < 100; i++) 
  {
    filename[6] = '0' + i/10;
    filename[7] = '0' + i%10;
    
    // create if does not exist, do not open existing, write, sync after write
    if (!SD.exists(filename)) 
    {
        break;
    }
  }

  logfile = SD.open(filename, FILE_WRITE);
  if ( !logfile ) 
  {
      Serial.print("FAILED TO CREATE "); 
  }
  else
  {
      Serial.print("Writing to "); 
  }

  Serial.println(filename);
}

void writeToFile(String* data){

  String dataString = "";
  for(int i=0; i < DATA_NUMBER; i++){
    if(i==0){
  delay(100);
      dataString = data[i];
    } else{
      dataString += ", " + data[i];
    }
  }
  
  if (logfile) 
  {
    logfile.println(dataString);

    // print to the serial port too:
    Serial.println(dataString);
  }

  logfile.flush();
}
