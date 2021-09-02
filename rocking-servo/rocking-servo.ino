#include <Servo.h>

#define START_ANGLE 90
#define STOP_ANGLE 120
#define LOOP_DELAY 250
#define SRV_PIN 9
#define SPD_PIN A0

const int startAngle = START_ANGLE;
const int stopAngle = STOP_ANGLE;
const int loopDelay = LOOP_DELAY;

const int srvPin = SRV_PIN;
const int spdPin = SPD_PIN;

const int stepAngle = 1;

Servo srv;

void setup() {
  Serial.begin(9600);

  srv.attach(srvPin);
  srv.write(startAngle);
}

int getSpd() {
  int a = analogRead(spdPin);

  if (a < 24) {
    return 0;
  }

  return ((a - 24) / 100);
}

int getStepDelay(int spd) {
  return (10 - spd) * 2;
}

int pos = startAngle;
int spd = 0;
bool fw = true;

void loop() {
  spd = getSpd();

  if (spd == 0) {
    srv.write(startAngle);
    delay(loopDelay);
    return;
  }

  if (fw) {
    pos += stepAngle;
  } else {
    pos -= stepAngle;
  }

  srv.write(pos);

  if (pos >= stopAngle) {
    delay(loopDelay);
    fw = false;
  } else {
    if (pos <= startAngle) {
      delay(loopDelay);
      fw = true;
    }
  }

  delay(getStepDelay(spd));
}
