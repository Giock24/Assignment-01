#include <EnableInterrupt.h>  // to enable that all pins can generate external interrupts
#define RED_LED 11			// the pin that the LED is attached to
#define BUTTON_ONE 3

#define SLEEP 0
#define CONFIRM 1
#define PATTERN 2

//const int RED_LED = 11;	// RED LED PIN
int ledPin[] = { 2, 4, 6, 8 }; // LED PIN
int buttonPin[] = { 3, 5, 7, 9 }; // BUTTON PIN

int max_number = sizeof(ledPin)/sizeof(int);

bool pattern[] = {false, false, false, false};

int brightness = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by

// variables not to be interrupted
volatile bool sleepMode;
volatile unsigned long timeOne, timeTwo, timeThree; //T1 = 3, T2 = 5, T3 = 7 
volatile int stateGame;
volatile bool start;

void startGame() {
  if (stateGame == SLEEP) {
    sleepMode = false;
    stateGame = CONFIRM;  
  } else if (stateGame == CONFIRM) {
    start = true;
    stateGame = PATTERN;
  }
}

void generatePattern() {
  for (int i = 0; i < max_number; i++) {
    pattern[i] = random(1,10)%2;
  }
}

// here we put the arduino to sleep
void sleepNow() {
 //set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
 //sleep_enable(); // enables the sleep bit in the mcucr register
 // so sleep is possible. just a safety pin
 // Now it is time to enable an interrupt.
 //attachInterrupt(0,wakeUpNow, LOW); // use interrupt 0 (pin 2) and run function
 // wakeUpNow when pin 2 gets LOW
 //sleep_mode(); // here the device is actually put to sleep!!
  // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
 //sleep_disable(); // first thing after waking from sleep:
 // disable sleep...
 //detachInterrupt(0); // disables interrupt 0 on pin 2 so the
 // wakeUpNow code will not be executed
 // during normal running time.
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
  sleepMode = true;
  stateGame = SLEEP;
  start = false;
  timeTwo = 5000;

  // declare all pins that have to be INPUT or OUTPUT
  pinMode(RED_LED, OUTPUT);
  for (int i = 0; i < max_number; i++) {
    pinMode(ledPin[i], OUTPUT);
    pinMode(buttonPin[i], INPUT);
  }

  enableInterrupt(BUTTON_ONE, startGame, RISING);

  Serial.begin(9600);
  Serial.println("Welcome to the Catch the Led Game. Press Key T1 to Start");
}

// the loop routine runs over and over again forever:
void loop() {
  noInterrupts();
  //Serial.println("Hello");

  interrupts();
  if (sleepMode) {
    // set the brightness of pin 9:
    analogWrite(RED_LED, brightness);
    // change the brightness for next time through the loop:
    brightness = brightness + fadeAmount;
    // reverse the direction of the fading at the ends of the fade:
    if (brightness == 0 || brightness == 255) {
      fadeAmount = -fadeAmount;
    }
    // wait for 30 milliseconds to see the dimming effect

    delay(30);
  } else {

    analogWrite(RED_LED, LOW);
    
    //delay(10000); //if in this 10s don't press T1 button go in deep sleep
    Serial.println("You have 5s to press first button to confirm!!!");
    delay(5000);
    if (!start) {
      // go in deep sleep
      sleepNow();
    } else {

      generatePattern();

      ledOnOff(HIGH);
      delay(timeTwo);
      ledOnOff(LOW);
    }
    
    Serial.println("The Game is restarting in 2.5s ...");
    delay(2500);
    noInterrupts();
    stateGame = SLEEP;
    start = false;
    sleepMode = true;
    interrupts();
    Serial.println("Welcome to the Catch the Led Game. Press Key T1 to Start");

  }
}
