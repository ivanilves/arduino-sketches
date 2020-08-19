#include <Servo.h>
#include <LowPower.h>

//#define DEBUG

#define INIT_DELAY 1000
#define OPENING_DELAY 10
#define CLOSING_DELAY 10

#define BTN_PIN 2
#define LED_PIN 13

#define SRV_PIN 9
#define SRV_CLOSED 45
#define SRV_OPEN 145

Servo srv;

#define TRIG_PIN 3
#define ECHO_PIN 4
#define MAX_DIST 80
#define MIN_DIST 25

bool isOpen = false;
bool manuallyOpen = false;
bool isOpening = false;
bool isClosing = false;

int srvPos = SRV_CLOSED;
int distance;

unsigned long statusTime = millis();
const int statusDelta = 250;

void setup() {
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  pinMode(SRV_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  srv.attach(SRV_PIN);
  srv.write(SRV_CLOSED);

  delay(INIT_DELAY);

#ifdef DEBUG
  Serial.begin(9600);
#endif
}

void printState() {
  Serial.println("--- STATE ---");

  Serial.print("Servo position: ");
  Serial.println(srvPos);

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println("cm");

  printFlag("isOpen", isOpen);
  printFlag("manuallyOpen", manuallyOpen);
  printFlag("isOpening", isOpening);
  printFlag("isClosing", isClosing);

  Serial.println("--- END ---");
}

void printFlag(String flagName, bool flagValue) {
  Serial.print(flagName);
  Serial.print(": ");
  Serial.println(flagValue);
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
  if (isOpening or isClosing) {
    ledOn();
  } else {
    ledOff();
  }

  if (isOpen and !manuallyOpen) {
    isOpen = false;
    isClosing = true;
    LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);
  }

  if (!isOpen and !isOpening and !isClosing) {
    distance = calculateDistance();

    if (distance >= MIN_DIST and distance <= MAX_DIST) {
      isOpening = true;
      manuallyOpen = false;
    }
  }

  if (!isOpening and !isClosing) {
    if (buttonPressed()) {
      if (isOpen) {
        isOpen = false;
        manuallyOpen = false;
        isClosing = true;
      } else {
        isOpening = true;
        manuallyOpen = true;
      }
    }
  }

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

  if (isClosing) {
    if (srvPos > SRV_CLOSED) {
      srvPos--;
      srv.write(srvPos);
    } else {
      isOpen = false;
      manuallyOpen = false;
      isOpening = false;
      isClosing = false;
      srvPos = SRV_CLOSED;
      srv.write(srvPos);
    }
    delay(CLOSING_DELAY);
  }

  if (!isOpening and !isClosing) {
    LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
  }

#ifdef DEBUG
  if (millis() - statusTime > statusDelta) {
    statusTime = millis();
    printState();
  }
#endif
}
