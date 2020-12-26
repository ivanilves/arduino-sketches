#define LED0_PIN 3
#define LED1_PIN 4
#define LED2_PIN 7
#define LED3_PIN 8
#define LED4_PIN 11
#define LED5_PIN 10
#define LED6_PIN 9
#define LED7_PIN 6
#define LED8_PIN 5

#define BUZZ_PIN 2
#define ECHO_PIN A0
#define TRIG_PIN A1

#define PLAY_DISTANCE 30
#define MODE_LOOPS 3
#define LOOP_DELAY 50
#define PLAY_COOLDOWN_DELAY 10000

/*
   You should be able to fine-tune this sketch only by changing the "define"s above.
   Go below this comment if you want to make deeper changes (you are welcome anyway)
*/

const int LED_PINS[] = { LED0_PIN, LED1_PIN, LED2_PIN, LED3_PIN, LED4_PIN, LED5_PIN, LED6_PIN, LED7_PIN, LED8_PIN };
const int LED_PAIRS[][2] = {
  { LED0_PIN, LED0_PIN },
  { LED1_PIN, LED8_PIN },
  { LED2_PIN, LED7_PIN },
  { LED3_PIN, LED6_PIN },
  { LED4_PIN, LED5_PIN },
  { LED3_PIN, LED6_PIN },
  { LED2_PIN, LED7_PIN },
  { LED1_PIN, LED8_PIN },
  { LED0_PIN, LED0_PIN },
};
const int LED_PIN_NUM = sizeof(LED_PINS) / sizeof(LED_PINS[0]);
const int LED_PAIR_NUM = sizeof(LED_PAIRS) / sizeof(LED_PAIRS[0]);

const int SINGLE = 0;
const int PAIRED = 1;
const int X_PLAY = 2;

// Mutable global state variables (handle with care)
int mode = SINGLE;
int _modeLoop = 0;
bool _loopFinished = false;
int _activeLed = -1;
int _activeLedPair = -1;

void setup() {
  for (int l = 0; l < LED_PIN_NUM; l++) {
    pinMode(LED_PINS[l], OUTPUT);
  }

  pinMode(BUZZ_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);

  allLedsOn(); delay(500); allLedsOff();
}

int calculateDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  int duration = pulseIn(ECHO_PIN, HIGH);

  return duration * 0.034 / 2;
}

void allLedsOn() {
  for (int l = 0; l < LED_PIN_NUM; l++) {
    digitalWrite(LED_PINS[l], HIGH);
  }
}

void allLedsOff() {
  for (int l = 0; l < LED_PIN_NUM; l++) {
    digitalWrite(LED_PINS[l], LOW);
  }
}

void singleLedOn(int led) {
  for (int l = 0; l < LED_PIN_NUM; l++) {
    if (l == led) {
      digitalWrite(LED_PINS[l], HIGH);
    }
    else {
      digitalWrite(LED_PINS[l], LOW);
    }
  }
}

int nextLed() {
  if (_activeLed < LED_PIN_NUM - 1) {
    _activeLed++;
  } else {
    _loopFinished = true;
    _activeLed = 0;
  }

  return _activeLed;
}

void ledPairOn(int pair) {
  digitalWrite(LED_PAIRS[pair][0], HIGH);
  digitalWrite(LED_PAIRS[pair][1], HIGH);
}

void activeLedPairOff() {
  digitalWrite(LED_PAIRS[_activeLedPair][0], LOW);
  digitalWrite(LED_PAIRS[_activeLedPair][1], LOW);
}

int nextLedPair() {
  if (_activeLedPair < LED_PAIR_NUM - 1) {
    _activeLedPair++;
  } else {
    _loopFinished = true;
    _activeLedPair = 0;
  }

  return _activeLedPair;
}

int nextMode() {
  if (!_loopFinished) {
    return mode;
  }

  _loopFinished = false;

  if (_modeLoop < MODE_LOOPS) {
    _modeLoop++;

    return mode;
  }

  _modeLoop = 0;

  if (mode == SINGLE) {
    return PAIRED;
  }

  return SINGLE;
}

void loop() {
  int distance = calculateDistance();

  if (distance < PLAY_DISTANCE and distance > 5) {
    mode = X_PLAY;
  } else {
    mode = nextMode();
  }

  switch (mode) {
    case SINGLE:
      singleLedOn(nextLed());
      break;
    case PAIRED:
      activeLedPairOff();
      ledPairOn(nextLedPair());
      break;
    case X_PLAY:
      allLedsOn();
      randomSeed(millis());
      sing(random(1, 4));
      delay(PLAY_COOLDOWN_DELAY);
      mode = SINGLE;
      break;
    default:
      // blink leds to signal error
      allLedsOn();
      tone(BUZZ_PIN, 250);
      delay(250);
      allLedsOff();
      noTone(BUZZ_PIN);
      break;
  }

  delay(LOOP_DELAY);
}
