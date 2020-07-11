#include <Servo.h>

#define START 0
#define STOP 180

#define STEP 5
#define DELAY 100

Servo srv;

void setup() {
 srv.attach(9);
 srv.write(START);
}

int pos = START;
bool fw = true;

void loop() {
  if (fw) {
    pos += STEP;
  } else {
    pos-= STEP;
  }

  srv.write(pos);

  if (pos >= STOP) {
    delay(200);
    fw = false;
  } else {
    if (pos <= START) {
      delay(200);
      fw = true;
    }
  }
  
  delay(DELAY);
}
