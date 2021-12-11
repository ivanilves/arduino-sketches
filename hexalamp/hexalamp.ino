#define ONOFF_PIN 2
#define IRSIG_PIN A0
#define ONOFF_TRH 800

void setup() {
  pinMode(ONOFF_PIN, OUTPUT);
  pinMode(IRSIG_PIN, INPUT);
}

int av;

void loop() {
  av = analogRead(IRSIG_PIN);


  if (av <= ONOFF_TRH) {
    digitalWrite(ONOFF_PIN, HIGH);
  } else {
    digitalWrite(ONOFF_PIN, LOW);
  }


  delay(100);
}
