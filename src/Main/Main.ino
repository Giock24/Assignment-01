#include <EnableInterrupt.h>  // to enable that all pins can generate external interrupts
#define RED_LED 11

// LED PIN = 2,4,6,8
// BUTTON PIN = 3,5,7,9
int ledPin[] = { 2, 4, 6, 8 };
int buttonPin[] = { 3, 5, 7, 9 };

int redLed = RED_LED;  // the pin that the LED is attached to
int brightness = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by

volatile bool sleepMode = true;

// the setup routine runs once when you press reset:
void setup() {
  // declare pin 9 to be an output:
  pinMode(redLed, OUTPUT);
  for (int i = 0; i < 4; i++) {
    pinMode(ledPin[i], OUTPUT);
    pinMode(buttonPin[i], INPUT);
  }

  // attachInterrupt(digitalPinToInterrupt(3), stopSleep, RISING); I don't know why throw a compile error
  Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
  noInterrupts();
  Serial.println("Hello");

  interrupts();
  if (sleepMode) {
    // set the brightness of pin 9:
    analogWrite(redLed, brightness);
    // change the brightness for next time through the loop:
    brightness = brightness + fadeAmount;
    // reverse the direction of the fading at the ends of the fade:
    if (brightness == 0 || brightness == 255) {
      fadeAmount = -fadeAmount;
    }
    // wait for 30 milliseconds to see the dimming effect
    Serial.println("I'm on SLEEP MODE!");
    delay(30);
  } else {

    noInterrupts();
    Serial.println("Going in SLEEP MODE in 2s ...");
    delay(2000);
    sleepMode = true;
    interrupts();

  }
}

void stopSleep() {
  sleepMode = true;
}