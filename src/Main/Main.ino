#include <EnableInterrupt.h>  // to enable that all pins can generate external interrupts
#include <avr/sleep.h>
#define RED_LED 11			// the pin that the LED is attached to

#define INIT 0
#define SLEEP 1
#define CONFIRM 2
#define PATTERN 3
#define GAME 4
#define GAMEOVER 5

//enum state = {INIT, SLEEP, CONFIRM, PATTERN, GAME, GAMEOVER};
//const int RED_LED = 11;	// RED LED PIN
int ledPin[] = { 2, 3, 4, 5 }; // LED PIN
int buttonPin[] = { 6, 7, 8, 9 }; // BUTTON PIN

int max_number = sizeof(ledPin)/sizeof(int);

bool pattern[] = {false, false, false, false};
bool read_values[] = {false, false, false, false};

int brightness = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by
long prevts = 0;

// variables not to be interrupted
// T1: time in which the leds are turned off
// T2: 
// T3: 
volatile unsigned long timeOne;
const unsigned long timeTwo = 5000, timeThree = 7000; // in ms
volatile int stateGame;
volatile bool inGame;

void buttonPushed() {

  long ts = micros();
  if (ts - prevts > 20000) {

    prevts = ts;
    
    switch(stateGame) {
      case SLEEP:
        stateGame = CONFIRM;
        break;
      case CONFIRM:
        inGame = true;
        stateGame = PATTERN;
        break;
      case GAME:
        for(int i=0; i < max_number; i++) {
          if (digitalRead(buttonPin[i]) == HIGH) {
            digitalWrite(ledPin[i], HIGH);
          }
        }
        
    }
  }
}

void generatePattern() {
  for (int i = 0; i < max_number; i++) {
    pattern[i] = random(1,10)%2;
  }
}

void wakeUp(){}

// here we put the arduino to sleep
void sleepNow() {
  Serial.println("I'm going in sleep mode");
  delay(500);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable(); // enables the sleep bit in the mcucr register
  sleep_mode();
}

void viewPattern(bool visibility) {
  for(int i = 0; i < max_number; i++) {
    if(pattern[i] == true) {
      digitalWrite(ledPin[i], visibility ? HIGH : LOW);
    }
  }
}

// the setup routine runs once when you press reset:
void setup() {
  stateGame = INIT;
  inGame = false;
  bool read_values[max_number];

  // declare all pins that have to be INPUT or OUTPUT
  pinMode(RED_LED, OUTPUT);
  for (int i = 0; i < max_number; i++) {
    pinMode(ledPin[i], OUTPUT);
    pinMode(buttonPin[i], INPUT);
    enableInterrupt(buttonPin[i], wakeUp, RISING);
    enableInterrupt(buttonPin[i], buttonPushed, RISING);
  }

  Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
    //Serial.println("Hello");

  switch(stateGame) {
    case INIT:
        timeOne = random(2000, 5000);
        Serial.println("Welcome to the Catch the Led Game. Press Key T1 to Start");
        stateGame = SLEEP;
        break;
    case SLEEP:
       // set the brightness of RED_LED pin:
      analogWrite(RED_LED, brightness);
      // change the brightness for next time through the loop:
      brightness = brightness + fadeAmount;
      // reverse the direction of the fading at the ends of the fade:
      if (brightness == 0 || brightness == 255) {
        fadeAmount = -fadeAmount;
      }
      // wait for 30 milliseconds to see the dimming effect

      delay(30);
      break;
    case CONFIRM:
      analogWrite(RED_LED, LOW);
    
      Serial.println("You have " + (String)(timeOne/1000) + "s to press first button to confirm!!!");
      delay(timeOne);
      if (!inGame) {
        // go in deep sleep
        sleepNow();
        noInterrupts();
        inGame = false;
        stateGame = INIT;
        interrupts();
      }
      break;
    case PATTERN:
      generatePattern();
      viewPattern(true);
      delay(timeTwo);
      viewPattern(false);
      stateGame = GAME;
      break;
    case GAME:
      Serial.println("Go!");

      
      //TODO
      delay(timeThree);


      for(int i=0; i < max_number; i++) {
         read_values[i] = false;
         digitalWrite(ledPin[i], LOW);
      }

      stateGame = GAMEOVER;
      break;
    case GAMEOVER:
      Serial.println("The Game is restarting ...");
      noInterrupts();
      stateGame = INIT;
      inGame = false;
      interrupts();
      break;
  }
}
