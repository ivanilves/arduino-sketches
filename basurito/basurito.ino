#include <Servo.h>

//#define DEBUG

#ifdef DEBUG
#define STEP_DELAY 250
#else
#define STEP_DELAY 10
#endif

#define BTN_PIN 2
#define LED_PIN 13

#define SRV_PIN 9
#define SRV_START 40
#define SRV_STOP 135
#define SRV_INC 1
#define SRV_DEC 1
#define SRV_KEEP_OPEN 5000
#define SRV_KEEP_CLOSED 1000

Servo srv;

#define TRIG_PIN 3
#define ECHO_PIN 4
#define MAX_DIST 80
#define MIN_DIST 25

bool isOpen = false;
bool manuallyOpen = false;
bool isOpening = false;
bool isClosing = false;

int srvPos = SRV_START;
int distance;

void setup() {
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  pinMode(SRV_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  srv.attach(SRV_PIN);
  srv.write(SRV_START);

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
  distance = calculateDistance();

  if (isOpen and !manuallyOpen) {
    delay(SRV_KEEP_OPEN);
    isOpen = false;
    isClosing = true;
  }

  if (!isOpen and !isOpening and !isClosing) {
    if (distance >= MIN_DIST and distance <= MAX_DIST) {
      isOpening = true;
    }
  }

  if (isOpening) {
    ledOn();
  } else {
    ledOff();
  }

  if (!isOpening and !isClosing) {
    if (buttonPressed()) {
      if (isOpen) {
        isOpen = false;
        isClosing = true;
        manuallyOpen = false;
      } else {
        isOpening = true;
        manuallyOpen = true;
      }
    }
  }

  if (isOpening) {
    if (srvPos < SRV_STOP) {
      srvPos += SRV_INC;
      srv.write(srvPos);
    } else {
      srvPos = SRV_STOP;
      srv.write(srvPos);
      isOpening = false;
      isOpen = true;
    }
  }

  if (isClosing) {
    if (srvPos > SRV_START) {
      srvPos -= SRV_DEC;
      srv.write(srvPos);
    } else {
      srvPos = SRV_START;
      srv.write(srvPos);
      isClosing = false;
      isOpen = false;
      manuallyOpen = false;
      delay(SRV_KEEP_CLOSED);
    }
  }

  delay(STEP_DELAY);

#ifdef DEBUG
  printState();
#endif
}
