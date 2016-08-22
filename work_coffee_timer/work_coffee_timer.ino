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
unsigned long timer1Age;
bool timer1Reset;
unsigned long timer2;
unsigned long timer2Age;
bool timer2Reset;

 //       _         1
 //      | |       6 2
 //       -         7
 //      | |       5 3
 //       _         4

uint8_t DASH = 0x40;
uint8_t BLANK = 0x00;
uint8_t LOWER_O = 0x5c;
uint8_t LOWER_L = 0x06;
uint8_t LOWER_D = 0x5e;

void setup() {
  // Setup the display.
  clock1.begin(DISPLAY_1);
  clock1.setBrightness(15);
  
  clock2.begin(DISPLAY_2);
  clock2.setBrightness(8);

  pinMode(TIME_PIN_1, INPUT);
  pinMode(LED_PIN_1, OUTPUT);
  
  pinMode(TIME_PIN_2, INPUT);
  pinMode(LED_PIN_2, OUTPUT);
  
  timer1 = timer2 = 0;
  timer1Reset = timer2Reset = false;
  basetime = millis();
}

void loop() {
  // Loop function runs over and over again to implement the clock logic.

  currtime = millis();
  if (currtime - basetime >= 1000) {
    // set display every second
    basetime = currtime;
    if (timer1Reset) {
      setClockDisplay(clock1,0);
      timer1Age++;    
    } else {
      if (timer1Age > 60) {
        timer1 = 1;
      } 
      timer1Age = 0;
      setClockDisplay(clock1,timer1);      
    }
    if (timer2Reset) {
      setClockDisplay(clock2,0);
      timer2Age++;    
    } else {
      if (timer2Age > 60) {
        timer2 = 1;
      } 
      timer2Age = 0;
      setClockDisplay(clock2,timer2);      
    }
    timer1++;
    timer2++;
  }

  digitalWrite(LED_PIN_1, HIGH);
  delay(50);
  timer1Reset = (digitalRead(TIME_PIN_1) == HIGH);
  digitalWrite(LED_PIN_1, LOW);
  
  digitalWrite(LED_PIN_2, HIGH);
  delay(50);
  timer2Reset = (digitalRead(TIME_PIN_2) == HIGH);
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

  if (hour >= 12) {
    oldClockDisplay(localClock);
  } else if ( minute >= 1) {
    setClockDisplay2Numbers(localClock, hour, minute, blink);
  } else if ( second > 0) {
    setClockDisplay1Number(localClock, second, blink);
  } else {
    dashClockDisplay(localClock);
  }

}

void setClockDisplay2Numbers(Adafruit_7segment localClock, int numA, int numB, boolean blink) {
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

void setClockDisplay1Number(Adafruit_7segment localClock, int numB, boolean blink) {
  localClock.writeDigitRaw(0, BLANK);
  localClock.writeDigitRaw(1, BLANK);

  int num10s = numB / 10;
  int num1s = numB - (num10s * 10);
  localClock.writeDigitNum(3, num10s);
  localClock.writeDigitNum(4, num1s);
  localClock.drawColon(blink);
  
  localClock.writeDisplay();
}


void dashClockDisplay(Adafruit_7segment localClock) {
  localClock.writeDigitRaw(0, DASH);
  localClock.writeDigitRaw(1, DASH);
  localClock.writeDigitRaw(3, DASH);
  localClock.writeDigitRaw(4, DASH);  
  localClock.writeDisplay();
}

void oldClockDisplay(Adafruit_7segment localClock) {
  localClock.writeDigitRaw(0, BLANK);
  localClock.writeDigitRaw(1, LOWER_O);
  localClock.writeDigitRaw(3, LOWER_L);
  localClock.writeDigitRaw(4, LOWER_D);  
  localClock.writeDisplay();
}
