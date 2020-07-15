/*
  UVKEYSAN Control Software
  V0.0.1
  Paul Kuzan
  15/07/202
*/

#include <Bounce2.h>

#define SWITCH_NONE 1
#define SWITCH_PRESS_SHORT 2
#define SWITCH_PRESS_LONG 3

#define SHIELD_DARK 1
#define SHIELD_LIGHT 2

#define STATE_STANDBY 1
#define STATE_ON 2
#define STATE_SETUP 3

#define STATE_LED_OFF 1
#define STATE_LED_ON 2
#define STATE_LED_FLASH_SLOW 3
#define STATE_LED_FLASH_FAST 4

bool justTransitioned = false;
bool ledStateJustTransitioned = false;

//Power LED flash  interval
const unsigned long onFlashInterval = 1000UL;
const unsigned long offFlashInterval = 200UL;

unsigned long previousMillis = 0;
bool ledFlashState;

Bounce onOffSwitch = Bounce();

const int buzzerPin = 8;
const int switchPin = 3; //10
const int ledPinOrange = 10; //2
const int ledPinRed = 9; //3
const int UVPin = 7;
const int photo_diode_internal = A1; //12
const int photo_diode_external = A2; //11

//Pushbutton hold time
const unsigned long switchHoldTime = 5000UL;
unsigned long switchPressTime = 0;
volatile byte switchState = SWITCH_NONE;
bool switchPressed = false;

const unsigned long runTime = 10000UL;
unsigned long stopTime = 0;

volatile byte ledState;
volatile byte state;
volatile byte shieldState;

void setup() {
  analogReference(EXTERNAL);

  pinMode( switchPin, INPUT_PULLUP);
  onOffSwitch.attach(switchPin);

  pinMode(ledPinOrange, OUTPUT);
  pinMode(ledPinRed, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(UVPin, OUTPUT);
  digitalWrite(UVPin, HIGH);

  pinMode(photo_diode_internal, INPUT);
  pinMode(photo_diode_external, INPUT);

  transitionTo(STATE_STANDBY);
}

void loop() {
  readPhotoDiodes();
  readSwitch();
  doStateMachine();
  doLEDStateMachine();
}

void doStateMachine() {
  if (switchState == SWITCH_PRESS_LONG) {
    switchState = SWITCH_NONE;
    transitionTo(STATE_SETUP);
  }
  switch (state) {
    case STATE_STANDBY: {
        if (justTransitioned) {
          transitionLEDState(STATE_LED_OFF);
          digitalWrite(UVPin, HIGH);
          digitalWrite(buzzerPin, LOW);
          justTransitioned = false;
        }

        if (switchState == SWITCH_PRESS_SHORT) {
          switchState = SWITCH_NONE;
          if (shieldState == SHIELD_DARK) {
            transitionTo(STATE_ON);
          }
        }
        break;
      }

    case STATE_ON: {
        if (justTransitioned) {
          stopTime = millis() + runTime;
          transitionLEDState(STATE_LED_FLASH_FAST);
          digitalWrite(buzzerPin, HIGH);
          digitalWrite(UVPin, LOW);
          justTransitioned = false;
        }
        if ((millis() > stopTime) || (shieldState == SHIELD_LIGHT) || (switchState == SWITCH_PRESS_SHORT)) {
          transitionTo(STATE_STANDBY);
        }
        break;
      }
    case STATE_SETUP: {
        if (justTransitioned) {
          transitionLEDState(STATE_LED_FLASH_SLOW);
          justTransitioned = false;
        }

        if (switchState == SWITCH_PRESS_SHORT) {
          transitionTo(STATE_STANDBY);
        }
        break;
      }
  }
  switchState = SWITCH_NONE;
}

void readSwitch() {
  onOffSwitch.update();
  if (onOffSwitch.fell()) {
    switchPressed = true;
    switchPressTime = millis();
  } else if (onOffSwitch.rose()) {
    if (switchPressed) {
      if ((millis() - switchPressTime) > switchHoldTime) {
        //Long press
        switchState = SWITCH_PRESS_LONG;
      } else {
        //Short press
        switchState = SWITCH_PRESS_SHORT;
      }
      switchPressed = false;
    }
  }
}

void readPhotoDiodes() {
  int diode_internal = analogRead(photo_diode_internal);
  int diode_external = analogRead(photo_diode_external);

  if (diode_internal < 8 ) {
    shieldState = SHIELD_DARK;
    digitalWrite(ledPinOrange, HIGH);
  } else {
    shieldState = SHIELD_LIGHT;
    digitalWrite(ledPinOrange, LOW);
  }
}

void transitionTo(byte newState) {
  justTransitioned = true;
  state = newState;
}

//The State Machine for the Power LED
void doLEDStateMachine() {
  switch (ledState) {
    case STATE_LED_OFF: {
        if (ledStateJustTransitioned) {
          updateLED(false);

          ledStateJustTransitioned = false;
        }

        break;
      }
    case STATE_LED_ON: {
        if (ledStateJustTransitioned) {
          updateLED(true);

          ledStateJustTransitioned = false;
        }

        break;
      }
    case STATE_LED_FLASH_SLOW: {
        if (ledStateJustTransitioned) {
          //Do nothing
          ledStateJustTransitioned = false;
        }

        doFlash(onFlashInterval);

        break;
      }
    case STATE_LED_FLASH_FAST: {
        if (ledStateJustTransitioned) {
          //Do nothing
          ledStateJustTransitioned = false;
        }

        doFlash(offFlashInterval);

        break;
      }
  }
}

void doFlash(unsigned long interval) {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    updateLED(!ledFlashState);
  }
}


//Actually turn on or off the power led
void updateLED(bool newLEDFlashState) {
  ledFlashState = newLEDFlashState;

  if (ledFlashState) {
    digitalWrite(ledPinRed, HIGH);
  } else {
    digitalWrite(ledPinRed, LOW);
  }
}

void transitionLEDState(byte newLEDState) {
  ledStateJustTransitioned = true;
  ledState = newLEDState;
}