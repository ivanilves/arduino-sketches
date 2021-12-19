#define DEBUG
#define ON_PIN 3
#define IR_PIN A6
#define PR_PIN A4
#define IR_TRH 900
#define PR_TRH 50

const int graceMillis = 100;
const int lightMillis = 30000;

void setup() {
  pinMode(ON_PIN, OUTPUT);
  pinMode(IR_PIN, INPUT);
  pinMode(PR_PIN, INPUT);

#ifdef DEBUG
  Serial.begin(9600);
#endif
}

bool on = false;
int ir, pr;
unsigned long t = 0;

void loop() {
  ir = analogRead(IR_PIN);
  pr = analogRead(PR_PIN);

#ifdef DEBUG
  Serial.print("IR: ");
  Serial.print(ir);
  Serial.print(" / PR: ");
  Serial.println(pr);
#endif

  if ((!on and ir < IR_TRH and pr < PR_TRH) or (on and ir < IR_TRH)) {
    on = !on;

    digitalWrite(ON_PIN, on);
    delay(5*graceMillis);

    if (on) {
      t = millis();
    }
  }

  if (on) {
    if (millis() - t > lightMillis) {
      digitalWrite(ON_PIN, !on);
      t = millis();
    }
  }

  delay(graceMillis);
}
