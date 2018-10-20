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

// these constants describe the pins. They won't change:
const int groundpin = 18;             // analog input pin 4 -- ground
const int powerpin = 19;              // analog input pin 5 -- voltage
const int xpin = A3;                  // x-axis of the accelerometer
const int ypin = A2;                  // y-axis
const int zpin = A1;                  // z-axis (only on 3-axis models)
const int buttonPin = 2;     // the number of the pushbutton pin
const int ledPin =  13;      // the number of the LED pin

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status

int prevx = 0;
int prevy = 0;
int prevz = 0;
bool isButtonPressed = false;

void setup() {
  // initialize the serial communications:
  Serial.begin(9600);

  // Provide ground and power by using the analog inputs as normal digital pins.
  // This makes it possible to directly connect the breakout board to the
  // Arduino. If you use the normal 5V and GND pins on the Arduino,
  // you can remove these lines.
  // accelerometer
  pinMode(groundpin, OUTPUT);
  pinMode(powerpin, OUTPUT);
  digitalWrite(groundpin, LOW);
  digitalWrite(powerpin, HIGH);

  //button
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
}

void loop() {
  buttonState = digitalRead(buttonPin);
  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    if(isButtonPressed){
      isButtonPressed = false;
      Serial.println("Stop reading");
    } else{
      Serial.println("Start reading");
      isButtonPressed = true;
    }
    digitalWrite(ledPin, HIGH);
  } else {
    // turn LED off:
    digitalWrite(ledPin, LOW);
  }

  if(isButtonPressed){
    Serial.print("x: ");
    Serial.print(analogRead(xpin));
    Serial.print("\t");
    Serial.print("y: ");
    Serial.print(analogRead(ypin));
    Serial.print("\t");
    Serial.print("z: ");
    Serial.print(analogRead(zpin));
    Serial.println();
  }

  // delay before next reading:
  delay(100);

}
