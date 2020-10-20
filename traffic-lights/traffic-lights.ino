#define RED_PIN 2
#define YELLOW_PIN 3
#define GREEN_PIN 4

#define NORMAL_DELAY 5000
#define SHORT_DELAY 2000

#define MODE_AUTO 0

byte mode = MODE_AUTO;

void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
}

void redLightOn() {
  digitalWrite(RED_PIN, HIGH);
  digitalWrite(YELLOW_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);
}

void yellowLightOn() {
  digitalWrite(RED_PIN, LOW);
  digitalWrite(YELLOW_PIN, HIGH);
  digitalWrite(GREEN_PIN, LOW);
}

void greenLightOn() {
  digitalWrite(RED_PIN, LOW);
  digitalWrite(YELLOW_PIN, LOW);
  digitalWrite(GREEN_PIN, HIGH);
}

void loop() {
  if (mode == MODE_AUTO) {
    redLightOn();
    delay(NORMAL_DELAY);
    yellowLightOn();
    delay(SHORT_DELAY);
    greenLightOn();
    delay(NORMAL_DELAY);
    yellowLightOn();
    delay(SHORT_DELAY);
  }
}
