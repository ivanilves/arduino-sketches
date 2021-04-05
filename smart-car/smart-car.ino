#define DEBUG

#define SENSOR_DELAY 10

#define MAX_QUIRKS 3
#define MAX_TURNS 10

#define ENA_PIN 10
#define ENB_PIN 11
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

#define RPCT 100
#define LPCT 100

const byte initSpd = 128; // speed to start on the first loop to gain acceleration
const byte moveSpd = 72; // speed to continue after first loop passed and we gained initial acceleration

const int turnDist = 40; // distance in cm on which normal turn will be triggered to avoid collision
const int spinDist = 15; // distance in cm on which obstacle is considered too close and radical spin or pulling back will be triggered

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
char pdir;

// quirk counter - triggers "circuir breaker" on reaching MAX_QUIRKS
int qc = 0;

// turns counter - triggers "direction adjustment" on reaching MAX_TURNS
int tc = 0;

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

void debug(char dir, byte spd, int cdist, int rdist, int ldist) {
  Serial.print(" > "); Serial.print(dir);
  Serial.print(" @ "); Serial.print(spd);
  Serial.print(" | C: "); Serial.print(cdist);
  Serial.print(" | R: "); Serial.print(rdist);
  Serial.print(" | L: "); Serial.print(ldist);
  Serial.print(" | TC: "); Serial.print(tc);
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

char chooseDirection(int cdist, int rdist, int ldist) {
  // below or equal "0" is a sensor quirk ...
  if ((cdist <= 0) or (rdist <= 0) or (ldist <= 0)) {
    // ... continue with previously chosen direction before reaching MAX_QUIRKS
    if (qc < MAX_QUIRKS) {
      qc++;
      return pdir;
    }
    // ... pull back after reaching MAX_QUIRKS
    qc = 0;
    return _BCK;
  } else {
    // reset quirk counter if sensor quirk is not the case
    qc = 0;
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

  // don't keep turning right or left more than one loop step
  if ((pdir == _RHT) or (pdir == _LFT)) {
    return _FWD;
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

char adjustDirection(char dir) {
  // not moving forward, neither turning currently
  if (dir != _FWD and dir != _RHT and dir != _LFT) {
    tc = 0;
    return dir;
  }

  // moving forward currently after anything BUT turning
  if (dir == _FWD and (pdir != _RHT and pdir != _LFT)) {
    tc = 0;
    return dir;
  }

  //  turning currently after anything BUT moving forward or turning
  if ((dir == _RHT or dir == _LFT) and (pdir != _FWD and pdir != _RHT and pdir != _LFT)) {
    tc = 0;
    return dir;
  }

  tc++;

  // too many turn/forward sequences: pull back!
  if (tc >= MAX_TURNS) {
    tc = 0;
    return _BCK;
  }

  // by default: preserve direction
  return dir;
}

int getSpeed(char dir) {
  if (dir == pdir) {
    return moveSpd;
  }

  if (dir == _RHT) {
    return moveSpd;
  }

  if (dir == _LFT) {
    return moveSpd;
  }

  return initSpd;
}

void goForward(int spd) {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, HIGH);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, HIGH);
  analogWrite(ENA_PIN, spd * LPCT / 100);
  analogWrite(ENB_PIN, spd * RPCT / 100);

  digitalWrite(LEDR_PIN, LOW);
  digitalWrite(LEDL_PIN, LOW);
}

void goRight(int spd) {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, HIGH);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, HIGH);
  analogWrite(ENA_PIN, spd);
  analogWrite(ENB_PIN, 0);

  digitalWrite(LEDR_PIN, HIGH);
  digitalWrite(LEDL_PIN, LOW);
}

void goLeft(int spd) {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, HIGH);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, HIGH);
  analogWrite(ENA_PIN, 0);
  analogWrite(ENB_PIN, spd);

  digitalWrite(LEDR_PIN, LOW);
  digitalWrite(LEDL_PIN, HIGH);
}

void spinRight(int spd) {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, HIGH);
  digitalWrite(IN3_PIN, HIGH);
  digitalWrite(IN4_PIN, LOW);
  analogWrite(ENA_PIN, spd);
  analogWrite(ENB_PIN, spd);

  digitalWrite(LEDR_PIN, HIGH);
  digitalWrite(LEDL_PIN, LOW);
}

void spinLeft(int spd) {
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, HIGH);
  analogWrite(ENA_PIN, spd);
  analogWrite(ENB_PIN, spd);

  digitalWrite(LEDR_PIN, LOW);
  digitalWrite(LEDL_PIN, HIGH);
}

void goBackward(int spd) {
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, HIGH);
  digitalWrite(IN4_PIN, LOW);
  analogWrite(ENA_PIN, spd);
  analogWrite(ENB_PIN, spd);

  digitalWrite(LEDR_PIN, HIGH);
  digitalWrite(LEDL_PIN, HIGH);
}

void stopMovement(int spd) {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, LOW);
  analogWrite(ENA_PIN, 0);
  analogWrite(ENB_PIN, 0);
}

void loop() {
  int cd = cDist();
  delay(SENSOR_DELAY);
  int rd = rDist();
  delay(SENSOR_DELAY);
  int ld = lDist();
  delay(SENSOR_DELAY);

  char dir = chooseDirection(cd, rd, ld);

  dir = adjustDirection(dir);

  byte spd = getSpeed(dir);

#ifdef DEBUG
  debug(dir, spd, cd, rd, ld);
#endif

  switch (dir) {
    case _FWD:
      goForward(spd);
      break;
    case _RHT:
      goRight(spd);
      break;
    case _LFT:
      goLeft(spd);
      break;
    case _SPR:
      spinRight(spd);
      break;
    case _SPL:
      spinLeft(spd);
      break;
    case _BCK:
      goBackward(spd);
      break;
    default:
      stopMovement(spd);
      break;
  }

  if (dir == _BCK) {
    delay(50);
  }

  prd = rd;
  pcd = cd;
  pld = ld;
  pdir = dir;
}
