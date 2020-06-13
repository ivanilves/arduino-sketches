#include <Servo.h>

#define SRV_PIN 9
#define SRV_START 5
#define SRV_STOP 125
#define SRV_STEP 1
#define SRV_DELAY 20
#define SRV_PAUSE 5000

Servo srv;

#define TRIG_PIN 10
#define ECHO_PIN 11
#define MAX_DIST 10
#define MIN_DIST 5


void setup() {
  Serial.begin(9600);

  srv.attach(SRV_PIN);
  srv.write(SRV_START);
}

int calculateDistance(int duration) {
  return duration * 0.034 / 2;
}

int duration;
int distance;
int pos = SRV_START;
bool fw = true;

void loop() {  
  /*digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  distance = calculateDistance(duration);

  Serial.print("Distance: ");
  Serial.println(distance);
*/
/*  if (distance <= MAX_DIST and distance >= MIN_DIST) {
*/
    if (fw) {
      pos += SRV_STEP;
    } else {
      pos -= SRV_STEP;
    }

    srv.write(pos);

    if (pos >= SRV_STOP) {
      delay(SRV_PAUSE);
      fw = false;
    } else {
      if (pos <= SRV_START) {
        delay(SRV_PAUSE);
        fw = true;
      }
    }
  /*}*/

  delay(SRV_DELAY);
  
}
