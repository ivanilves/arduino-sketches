#define ENA_PIN 10
#define ENB_PIN 11
#define IN1_PIN 2
#define IN2_PIN 3
#define IN3_PIN 5
#define IN4_PIN 4

#define INI_SPEED 128
#define MIN_SPEED 32
#define MAX_SPEED 255

void setup() {
  pinMode(ENA_PIN, OUTPUT);
  pinMode(ENB_PIN, OUTPUT);
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  pinMode(IN3_PIN, OUTPUT);
  pinMode(IN4_PIN, OUTPUT);
}

int current_speed = 0;

bool accelerate = true;

void loop() {
  if (current_speed == 0) {
    digitalWrite(IN1_PIN, HIGH);
    digitalWrite(IN2_PIN, LOW);
    digitalWrite(IN3_PIN, HIGH);
    digitalWrite(IN4_PIN, LOW);
    analogWrite(ENA_PIN, INI_SPEED);
    analogWrite(ENB_PIN, INI_SPEED);
  }

  if (current_speed == MAX_SPEED) {
    accelerate = false;
  }

  if (current_speed == MIN_SPEED) {
    accelerate = true;
  }

  if (accelerate) {
    current_speed += 1;
  } else {
    current_speed -= 1;
  }

  analogWrite(ENA_PIN, current_speed);
  analogWrite(ENB_PIN, current_speed);

  delay(50);
}
