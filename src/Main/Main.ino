#include <EnableInterrupt.h>  // to enable that all pins can generate external interrupts
#include <avr/sleep.h>
#define RED_LED 11			// the pin that the LED is attached to

#define INIT 0
#define SLEEP 1
#define CONFIRM 2
#define PATTERN 3
#define GAME 4
#define GAMEOVER 5

//enum state = {INIT, SLEEP, CONFIRM, PATTERN,  GAME, GAMEOVER};

//const int RED_LED = 11;	// RED LED PIN
int ledPin[] = { 2, 3, 4, 5 }; // LED PIN
int buttonPin[] = { 6, 7, 8, 9 }; // BUTTON PIN

int max_number = sizeof(ledPin)/sizeof(int);

bool pattern[] = {false, false, false, false};

int brightness = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by

// variables not to be interrupted
volatile unsigned long timeOne, timeTwo, timeThree; //T1 = 3, T2 = 5, T3 = 7 
volatile int stateGame;
volatile bool start;

void startGame() {
  switch(stateGame) {
    case SLEEP:
       stateGame = CONFIRM;
       break;
    case CONFIRM:
       start = true;
       stateGame = PATTERN;
       break;
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
  delay(2000);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable(); // enables the sleep bit in the mcucr register
  sleep_mode();
}

void ledOnOff(int value) {
  for(int i = 0; i < max_number; i++) {
    if(pattern[i] == true) {
      digitalWrite(ledPin[i], value);
    }
  }
}

// the setup routine runs once when you press reset:
void setup() {
  stateGame = INIT;
  start = false;
  timeTwo = 5000;

  // declare all pins that have to be INPUT or OUTPUT
  pinMode(RED_LED, OUTPUT);
  for (int i = 0; i < max_number; i++) {
    pinMode(ledPin[i], OUTPUT);
    pinMode(buttonPin[i], INPUT);
    enableInterrupt(buttonPin[i], wakeUp, RISING);
  }
  enableInterrupt(buttonPin[0], startGame, RISING);

  Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
    //Serial.println("Hello");

  switch(stateGame) {
    case INIT:
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
    
      //delay(10000); //if in this 10s don't press T1 button go in deep sleep
      Serial.println("You have 5s to press first button to confirm!!!");
      delay(5000);
      if (!start) {
        // go in deep sleep
        sleepNow();
        stateGame = SLEEP;
      }
      break;
    case PATTERN:
      generatePattern();
      ledOnOff(HIGH);
      delay(timeTwo);
      ledOnOff(LOW);
      stateGame = GAME;
      break;
    case GAME:
      //TODO
      stateGame = GAMEOVER;
      break;
    case GAMEOVER:
      Serial.println("The Game is restarting in 2.5s ...");
      delay(2500);
      noInterrupts();
      stateGame = INIT;
      start = false;
      interrupts();
      break;
  }
}
