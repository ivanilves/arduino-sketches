#include <Servo.h>

#define DEBUG

#ifdef DEBUG
#define STEP_DELAY 500
#else
#define STEP_DELAY 50
#endif

#define BTN_PIN 2
#define LED_PIN 13

#define SRV_PIN 9
#define SRV_START 5
#define SRV_STOP 125
#define SRV_STEP 5
#define SRV_DELAY 5
#define SRV_PAUSE 3000

Servo srv;

#define TRIG_PIN 3
#define ECHO_PIN 4
#define MAX_DIST 80
#define MIN_DIST 25

bool isOpen = false;
bool needsToOpen = false;
bool needsToClose = false;
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

#ifdef DEBUG
  Serial.begin(9600);
#endif

  srv.attach(SRV_PIN);
  srv.write(SRV_START);
}

void printState() {
  Serial.println("--- STATE ---");

  Serial.print("Servo position: ");
  Serial.println(srvPos);

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println("cm");
  
  printFlag("isOpen", isOpen);
  printFlag("needsToOpen", needsToOpen);
  printFlag("needsToClose", needsToClose);
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
  if (isOpen or isOpening) {
    ledOn();
  } else {
    ledOff();
  }

  distance = calculateDistance();

#ifdef DEBUG
  printState();
#endif

  if (!isOpening and !isClosing) {
    if (buttonPressed()) {
      if (isOpen) {
        needsToClose = true;
        manuallyOpen = false;
      } else {
        needsToOpen = true;
        manuallyOpen = true;
      }
    }
  }

  if (needsToClose) {
    isClosing = true;
    isOpening = false;
  }

  if (needsToOpen) {
    isClosing = false;
    isOpening = true;
  }

  if (isOpening) {
    if (srvPos < SRV_STOP) {
      srvPos += SRV_STEP;
      srv.write(srvPos);
      delay(SRV_DELAY);
    } else {
      needsToOpen = false;
      isOpening = false;
      isOpen = true;
    }
  }

  if (isClosing) {
    if (srvPos > SRV_START) {
      srvPos -= SRV_STEP;
      srv.write(srvPos);
      delay(SRV_DELAY);
    } else {
      needsToClose = false;
      isClosing = false;
      isOpen = false;
    }
  }
  
  delay(STEP_DELAY);
}
