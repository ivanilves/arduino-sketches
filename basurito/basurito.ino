#include <Servo.h>
#include <LowPower.h>

#define OPENING_DELAY 10
#define CLOSING_DELAY 10

#define BTN_PIN 2
#define LED_PIN 13

#define SRV_PIN 9
#define SRV_CLOSED 45
#define SRV_OPEN 135

Servo srv;

int srvPos = SRV_CLOSED;

#define TRIG_PIN 3
#define ECHO_PIN 4

#define MAX_DIST 50
#define MIN_DIST 20

int distance;

bool isOpen = false;
bool isManual = false;
bool isOpening = false;
bool isClosing = false;

bool isMoving() {
  return isOpening || isClosing;
}

bool isClosed() {
  return !isOpen && !isMoving();
}

bool isManuallyOpen() {
  return isOpen && isManual;
}

bool isAutoOpen() {
  return isOpen && !isManual;
}

void setup() {
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  pinMode(SRV_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  srv.attach(SRV_PIN);
  srv.write(SRV_CLOSED);
}

void ledOn() {
  digitalWrite(LED_PIN, HIGH);
}

void ledOff() {
  digitalWrite(LED_PIN, LOW);
}

bool buttonPressed() {
  return digitalRead(BTN_PIN) == HIGH;
}

int calculateDistance() {
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  int duration = pulseIn(ECHO_PIN, HIGH);

  return duration * 0.034 / 2;
}

void loop() {
  // With lid closed we check for the ultrasound signal to see if we need to open lid
  if (isClosed()) {
    distance = calculateDistance();

    // If distance to obstacle (trash bag or hand) is within the range, signal opening
    if (distance >= MIN_DIST and distance <= MAX_DIST) {
      isOpening = true;
      isManual = false;
    } else {
      // ... if not, just sleep waiting for the better times ...
      LowPower.powerDown(SLEEP_500MS, ADC_OFF, BOD_OFF);
    }
  }

  // While lid not moving we also accept the button-triggered manual open/close directives
  if (!isMoving()) {
    if (buttonPressed()) {
      if (isOpen) {
        isOpen = false;
        isManual = true;
        isOpening = false;
        isClosing = true;
      } else {
        isManual = true;
        isOpening = true;
        isClosing = false;
      }
    }
  }

  // Keep opening the lid until we open it
  if (isOpening) {
    if (srvPos < SRV_OPEN) {
      srvPos++;
      srv.write(srvPos);
    } else {
      isOpen = true;
      isOpening = false;
      isClosing = false;
      srvPos = SRV_OPEN;
      srv.write(srvPos);
    }
    delay(OPENING_DELAY);
  }

  // Keep closing the lid until we close it
  if (isClosing) {
    if (srvPos > SRV_CLOSED) {
      srvPos--;
      srv.write(srvPos);
    } else {
      isOpen = false;
      isManual = false;
      isOpening = false;
      isClosing = false;
      srvPos = SRV_CLOSED;
      srv.write(srvPos);
      delay(CLOSING_DELAY);
      LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
    }
    delay(CLOSING_DELAY);
  }

  // After being open automatically (by sensor trigger), sleep and signal opening again ;)
  if (isAutoOpen()) {
    LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);
    isOpen = false;
    isClosing = true;
  }

  // If we are moving the lid, light the led on
  // ... if we are not, put the led light off ...
  if (isMoving()) {
    ledOn();
  } else {
    ledOff();
  }
}
