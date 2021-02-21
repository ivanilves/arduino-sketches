//#define DEBUG

#define ENA_PIN 10
#define ENB_PIN 9
#define IN1_PIN 2
#define IN2_PIN 3
#define IN3_PIN 5
#define IN4_PIN 4

#define LEDR_PIN 6
#define LEDL_PIN 7

#define ECHOR_PIN A0
#define TRIGR_PIN A1
#define ECHOC_PIN A2
#define TRIGC_PIN A3
#define ECHOL_PIN A4
#define TRIGL_PIN A5

#define SPD 255

const int turnDist = 30; // distance in cm on which normal turn will be triggered to avoid collision
const int spinDist = 7; // distance in cm on which obstacle is considered too close and radical spin or pulling back will be triggered

const char _FWD = 'f';
const char _RHT = 'r';
const char _LFT = 'l';
const char _SPR = 'R';
const char _SPL = 'L';
const char _BCK = 'b';

// sensor distances and movement direction saved from the previous loop
int pcd;
int prd;
int pld;
int pdir;

void setup() {
#ifdef DEBUG
  Serial.begin(9600);
#endif

  pinMode(ENA_PIN, OUTPUT);
  pinMode(ENB_PIN, OUTPUT);
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  pinMode(IN3_PIN, OUTPUT);
  pinMode(IN4_PIN, OUTPUT);

  pinMode(LEDR_PIN, OUTPUT);
  pinMode(LEDL_PIN, OUTPUT);

  pinMode(ECHOR_PIN, INPUT);
  pinMode(TRIGR_PIN, OUTPUT);
  pinMode(ECHOC_PIN, INPUT);
  pinMode(TRIGC_PIN, OUTPUT);
  pinMode(ECHOL_PIN, INPUT);
  pinMode(TRIGL_PIN, OUTPUT);
}

void debug(char dir, int cdist, int rdist, int ldist) {
  Serial.print(" > "); Serial.print(dir);
  Serial.print(" | C: "); Serial.print(cdist);
  Serial.print(" | R: "); Serial.print(rdist);
  Serial.print(" | L: "); Serial.print(ldist);
  Serial.println("");
}

int calculateDistance(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  int duration = pulseIn(echo, HIGH);

  return duration * 0.034 / 2;
}

int cDist() {
  return calculateDistance(TRIGC_PIN, ECHOC_PIN);
}

int rDist() {
  return calculateDistance(TRIGR_PIN, ECHOR_PIN);
}

int lDist() {
  return calculateDistance(TRIGL_PIN, ECHOL_PIN);
}

byte chooseDirection(int cdist, int rdist, int ldist) {
  // "0" is a sensor quirk: continue with previously chosen direction in this case
  if ((cdist == 0) or (rdist == 0) or (ldist == 0)) {
    return pdir;
  }

  // we got stuck: spin left or right ...
  if ((cdist == pcd) and (rdist == prd) and (ldist == pld)) {
    // ... or just pull back if we already were spinning
    if ((pdir == _SPR) or (pdir == _SPL)) {
      return _BCK;
    }
    if (rdist < ldist) {
      return _SPL;
    }
    return _SPR;
  }

  // obstacle too close to center sensor: pull back to avoid collision ...
  if (cdist < spinDist) {
    return _BCK;
  }
  // ... or spin left or right in case of proximity with lateral sensors ...
  if (rdist < spinDist) {
    // ... or just pull back if we were already spinning left
    if (pdir == _SPL) {
      return _BCK;
    }
    return _SPL;
  }
  if (ldist < spinDist) {
    // ... or just pull back if we were already spinning right
    if (pdir == _SPR) {
      return _BCK;
    }
    return _SPR;
  }

  // when obstacle detected: decide to turn left or right
  if (cdist < turnDist) {
    if (rdist < ldist) {
      return _LFT;
    }
    return _RHT;
  }
  if (rdist < turnDist) {
    return _LFT;
  }
  if (ldist < turnDist) {
    return _RHT;
  }

  // no obstacles: move forward by default
  return _FWD;
}

void goForward() {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, HIGH);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, HIGH);
  analogWrite(ENA_PIN, SPD);
  analogWrite(ENB_PIN, SPD);

  digitalWrite(LEDR_PIN, LOW);
  digitalWrite(LEDL_PIN, LOW);
}

void goRight() {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, HIGH);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, HIGH);
  analogWrite(ENA_PIN, SPD);
  analogWrite(ENB_PIN, 0);

  digitalWrite(LEDR_PIN, HIGH);
  digitalWrite(LEDL_PIN, LOW);
}

void goLeft() {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, HIGH);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, HIGH);
  analogWrite(ENA_PIN, 0);
  analogWrite(ENB_PIN, SPD);

  digitalWrite(LEDR_PIN, LOW);
  digitalWrite(LEDL_PIN, HIGH);
}

void spinRight() {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, HIGH);
  digitalWrite(IN3_PIN, HIGH);
  digitalWrite(IN4_PIN, LOW);
  analogWrite(ENA_PIN, SPD);
  analogWrite(ENB_PIN, SPD);

  digitalWrite(LEDR_PIN, HIGH);
  digitalWrite(LEDL_PIN, LOW);
}

void spinLeft() {
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, HIGH);
  analogWrite(ENA_PIN, SPD);
  analogWrite(ENB_PIN, SPD);

  digitalWrite(LEDR_PIN, LOW);
  digitalWrite(LEDL_PIN, HIGH);
}

void goBackward() {
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, HIGH);
  digitalWrite(IN4_PIN, LOW);
  analogWrite(ENA_PIN, SPD);
  analogWrite(ENB_PIN, SPD);

  digitalWrite(LEDR_PIN, HIGH);
  digitalWrite(LEDL_PIN, HIGH);
}

void stopMovement() {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, LOW);
  analogWrite(ENA_PIN, 0);
  analogWrite(ENB_PIN, 0);
}

void loop() {
  int cd = cDist();
  delay(33);
  int rd = rDist();
  delay(33);
  int ld = lDist();
  delay(33);

  int dir = chooseDirection(cd, rd, ld);

#ifdef DEBUG
  debug(dir, cd, rd, ld);
#endif

  prd = rd;
  pcd = cd;
  pld = ld;
  pdir = dir;

  switch (dir) {
    case _FWD:
      goForward();
      break;
    case _RHT:
      goRight();
      break;
    case _LFT:
      goLeft();
      break;
    case _SPR:
      spinRight();
      break;
    case _SPL:
      spinLeft();
      break;
    case _BCK:
      goBackward();
      break;
    default:
      stopMovement();
      break;
  }

  delay(100);
  stopMovement();
  delay(100);
}
