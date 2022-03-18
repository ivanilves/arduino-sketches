#include <Servo.h>

#define SRV1A 2
#define SRV2A 3
#define SRV3A 4
#define SRV4A 5

#define SRV1B 7
#define SRV2B 6
#define SRV3B 9
#define SRV4B 8

#define SET90 13

#define SRVA_START 60
#define SRVA_STOP 120

#define SRVB_START 90
#define SRVB_STOP 145

#define STEP_DELAY 10

Servo srv1a;
Servo srv2a;
Servo srv3a;
Servo srv4a;

Servo srv1b;
Servo srv2b;
Servo srv3b;
Servo srv4b;

int pos(int i, byte leg) {
  if (leg % 2 == 0) {
    return 180 - i;
  }

  return i;
}

Servo getServoA(byte leg) {
  switch (leg) {
    case 1:
      return srv1a;
    case 2:
      return srv2a;
    case 3:
      return srv3a;
    case 4:
      return srv4a;
  }
}

Servo getServoB(byte leg) {
  switch (leg) {
    case 1:
      return srv1b;
    case 2:
      return srv2b;
    case 3:
      return srv3b;
    case 4:
      return srv4b;
  }
}

void doStep(byte leg) {
  Servo srva = getServoA(leg);
  Servo srvb = getServoB(leg);

  srvb.write(pos(SRVB_STOP, leg));

  for (int i = SRVA_START; i <= SRVA_STOP; i++) {
    srva.write(pos(i, leg));
    delay(STEP_DELAY);
  }

  srvb.write(pos(SRVB_START, leg));
}

void doCycle() {
  srv1b.write(pos(SRVB_STOP, 1));
  srv4b.write(pos(SRVB_STOP, 4));
  srv2b.write(pos(SRVB_START, 2));
  srv3b.write(pos(SRVB_START, 3));

  for (int i = SRVA_START; i <= SRVA_STOP; i++) {
    srv1a.write(pos(i, 1));
    srv4a.write(pos(i, 4));
    srv2a.write(pos(180 - i, 2));
    srv3a.write(pos(180 - i, 3));
    delay(STEP_DELAY);
  }

  srv1b.write(pos(SRVB_START, 1));
  srv4b.write(pos(SRVB_START, 4));
  srv2b.write(pos(SRVB_STOP, 2));
  srv3b.write(pos(SRVB_STOP, 3));

  for (int i = SRVA_STOP; i >= SRVA_START; i--) {
    srv1a.write(pos(i, 1));
    srv4a.write(pos(i, 4));
    srv2a.write(pos(180 - i, 2));
    srv3a.write(pos(180 - i, 3));
    delay(STEP_DELAY);
  }
}

void doPush() {
  for (int i = SRVA_STOP; i >= SRVA_START; i--) {
    srv1a.write(pos(i, 1));
    srv2a.write(pos(i, 2));
    srv3a.write(pos(i, 3));
    srv4a.write(pos(i, 4));
    delay(STEP_DELAY);
  }
}

void set90() {
  srv1a.write(90);
  srv2a.write(90);
  srv3a.write(90);
  srv4a.write(90);

  srv1b.write(90);
  srv2b.write(90);
  srv3b.write(90);
  srv4b.write(90);
}

void setup() {
  pinMode(SRV1A, OUTPUT);
  pinMode(SRV2A, OUTPUT);
  pinMode(SRV3A, OUTPUT);
  pinMode(SRV4A, OUTPUT);

  pinMode(SRV1B, OUTPUT);
  pinMode(SRV2B, OUTPUT);
  pinMode(SRV3B, OUTPUT);
  pinMode(SRV4B, OUTPUT);

  pinMode(SET90, INPUT);

  srv1a.attach(SRV1A);
  srv2a.attach(SRV2A);
  srv3a.attach(SRV3A);
  srv4a.attach(SRV4A);

  srv1b.attach(SRV1B);
  srv2b.attach(SRV2B);
  srv3b.attach(SRV3B);
  srv4b.attach(SRV4B);

  srv1a.write(pos(SRVA_START, 1));
  srv2a.write(pos(SRVA_START, 2));
  srv3a.write(pos(SRVA_START, 3));
  srv4a.write(pos(SRVA_START, 4));

  srv1b.write(pos(SRVB_START, 1));
  srv2b.write(pos(SRVB_START, 2));
  srv3b.write(pos(SRVB_START, 3));
  srv4b.write(pos(SRVB_START, 4));

  Serial.begin(9600);

  set90();
}

void loop() {
  /*doStep(1);
    doStep(4);
    doStep(2);
    doStep(3);

    delay(STEP_DELAY * 10);

    doPush();*/

  if ( digitalRead(SET90) == HIGH ) {
    set90();
  } else {
    doCycle();
  }

  delay(STEP_DELAY);
}
