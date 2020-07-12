#include <Servo.h>

//#define DEBUG

#define STEP_DELAY 9

#define BTN_PIN 7
#define LED_PIN 4

#define SRV_PIN 9
#define SRV_START 5
#define SRV_STOP 125
#define SRV_DELAY 20
#define SRV_PAUSE 3000

Servo srv;

#define TRIG_PIN 5
#define ECHO_PIN 6
#define MAX_DIST 80
#define MIN_DIST 25

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

  ledOn();
  delay(500);
  ledOff();
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

bool isOpen = false;
bool needsToOpen = false;
bool needsToClose = false;
bool manuallyOpen = false;
bool isClosing = false;
bool isOpening = false;

int srvPos = SRV_START;
int distance;

void loop() {
  if (isOpen or isOpening) {
    ledOn();
  } else {
    ledOff();
  }

  distance = calculateDistance();

#ifdef DEBUG
  Serial.print("Distance: ");
  Serial.println(distance);
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
      srvPos++;
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
      srvPos--;
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
