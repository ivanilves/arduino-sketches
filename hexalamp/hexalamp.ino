#define DEBUG
#define ONOFF_PIN 3
#define IRSIG_PIN A6
#define ONOFF_TRH 900

void setup() {
  pinMode(ONOFF_PIN, OUTPUT);
  pinMode(IRSIG_PIN, INPUT);

#ifdef DEBUG
  Serial.begin(9600);
#endif
}

bool on = false;
int av;
unsigned long t = 0;

void loop() {
  av = analogRead(IRSIG_PIN);

#ifdef DEBUG
  Serial.println(av);
#endif

  if (av < ONOFF_TRH) {
    on = !on;

    digitalWrite(ONOFF_PIN, on);
    delay(500);

    if (on) {
      t = millis();
    }
  }

  if (on) {
    if (millis() - t > 30000) {
      digitalWrite(ONOFF_PIN, !on);
      t = millis();
    }
  }

  delay(50);
}
