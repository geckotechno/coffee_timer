#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
#include "RTClib.h"

// I2C address of the display.  Stick with the default address of 0x70
// unless you've changed the address jumpers on the back of the display.
#define DISPLAY_1   0x70
#define DISPLAY_2   0x71

#define DAY (24L * 60L * 60L)
#define HOUR (60L * 60L)
#define MINUTE (60L)


// Create display and DS1307 objects.  These are global variables that
// can be accessed from both the setup and loop function below.
Adafruit_7segment clock1 = Adafruit_7segment();
Adafruit_7segment clock2 = Adafruit_7segment();

unsigned int TIME_PIN_1 = 9;
unsigned int LED_PIN_1 = 2;

unsigned int TIME_PIN_2 = 11;
unsigned int LED_PIN_2 = 4;

// Remember if the colon was drawn on the display so it can be blinked
// on and off every second.
bool blinkColon = false;

unsigned long basetime;
unsigned long currtime;
unsigned long timer1;
unsigned long timer2;

 //       _      1     1
 //      | |    6 2   6 2
 //       -      4     7
 //      | |    5 3   5 3
 //       _      7     4

uint8_t dash = 0x40;
uint8_t lower_o = 0x5c;
uint8_t lower_l = 0x06;
uint8_t lower_d = 0x5e;

void setup() {
  Serial.begin(9600);
  // Setup the display.
  clock1.begin(DISPLAY_1);
  clock2.begin(DISPLAY_2);

  pinMode(TIME_PIN_1, INPUT);
  pinMode(LED_PIN_1, OUTPUT);
  
  pinMode(TIME_PIN_2, INPUT);
  pinMode(LED_PIN_2, OUTPUT);
  
  timer1 = timer2 = 0;
  basetime = millis();
}

void loop() {
  // Loop function runs over and over again to implement the clock logic.

  currtime = millis();
  if (currtime - basetime >= 1000) {
    basetime = currtime;
    setClockDisplay(clock1,timer1);
    setClockDisplay(clock2,timer2);
    Serial.println(timer1);
    Serial.println(timer2);
    timer1++;
    timer2++;
  }

  digitalWrite(LED_PIN_1, HIGH);
  delay(50);
  if (digitalRead(TIME_PIN_1) == HIGH) {
    timer1 = 0;
  }
  digitalWrite(LED_PIN_1, LOW);
  
  digitalWrite(LED_PIN_2, HIGH);
  delay(50);
  if (digitalRead(TIME_PIN_2) == HIGH) {
    timer2 = 0;
  }
  digitalWrite(LED_PIN_2, LOW);
  
  // Pause for a second for time to elapse.  This value is in milliseconds
  // so 1000 milliseconds = 1 second.
  delay(100);


  // Loop code is finished, it will jump back to the start of the loop
  // function again!
}

void setClockDisplay(Adafruit_7segment localClock, long timer) {
  uint8_t type = DEC;
  long  day = (timer / DAY);
  timer -= day * DAY;
  long hour = (timer / HOUR);
  timer -= hour * HOUR;
  long minute = (timer / MINUTE);
  timer -= minute * MINUTE;
  long second = timer;
  boolean blink = (second % 2) == 0;

  if (day > 0) {
    oldClockDisplay(localClock);
  } else if ( hour > 0) {
    setClockDisplay(localClock, hour, minute, blink);
  } else if ( minute > 0 || second > 0) {
    setClockDisplay(localClock, minute, second, blink);
  } else {
    dashClockDisplay(localClock);
  }

}

void setClockDisplay(Adafruit_7segment localClock, int numA, int numB, boolean blink) {
  int num10s = numA / 10;
  int num1s = numA - (num10s * 10);
  localClock.writeDigitNum(0, num10s);
  localClock.writeDigitNum(1, num1s);

  num10s = numB / 10;
  num1s = numB - (num10s * 10);
  localClock.writeDigitNum(3, num10s);
  localClock.writeDigitNum(4, num1s);
  localClock.drawColon(blink);
  
  localClock.writeDisplay();
}


void dashClockDisplay(Adafruit_7segment localClock) {
  localClock.writeDigitRaw(0, dash);
  localClock.writeDigitRaw(1, dash);
  localClock.writeDigitRaw(3, dash);
  localClock.writeDigitRaw(4, dash);  
  localClock.writeDisplay();
}

void oldClockDisplay(Adafruit_7segment localClock) {
  localClock.writeDigitRaw(1, lower_o);
  localClock.writeDigitRaw(3, lower_l);
  localClock.writeDigitRaw(4, lower_d);  
  localClock.writeDisplay();
}
