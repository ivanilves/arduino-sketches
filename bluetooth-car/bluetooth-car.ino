#define ENA_PIN 10
#define ENB_PIN 11
#define IN1_PIN 3
#define IN2_PIN 2
#define IN3_PIN 5
#define IN4_PIN 4
#define F_LIGHT_PIN 7
#define R_LIGHT_PIN 6
#define BT_RX_PIN 9
#define BT_TX_PIN 8
#define BUZZ_PIN 13

#define CRS_PIN A0
#define CRS_BASE 512
#define CRS_PCT 25

#define MIN_SPEED 96
#define MAX_SPEED 255

#include <SoftwareSerial.h>
SoftwareSerial bt(BT_RX_PIN, BT_TX_PIN);

void setup() {
  pinMode(ENA_PIN, OUTPUT);
  pinMode(ENB_PIN, OUTPUT);
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  pinMode(IN3_PIN, OUTPUT);
  pinMode(IN4_PIN, OUTPUT);
  pinMode(F_LIGHT_PIN, OUTPUT);
  pinMode(R_LIGHT_PIN, OUTPUT);
  pinMode(BT_RX_PIN, INPUT);
  pinMode(BT_TX_PIN, OUTPUT);

  Serial.begin(9600);
  bt.begin(9600);

  info("Initialization complete ;)");
}

byte _spd = MAX_SPEED;
byte _spdc = 10;

char btcmd;

const unsigned int buzzint = 500;
const unsigned int buzzfreq = 1000;
bool buzzer = false;

bool f_light = false;
bool r_light = false;

const unsigned int xint = 200;
bool xtra = false;
bool xtac = true;
unsigned long xtime = millis();

void loop() {
  if (bt.available()) {
    btcmd = (char)bt.read();

    _spd = calculateSpeed(_spdc);

    switch (btcmd) {
      case 'D':
        fullStop();
        break;
      case 'S':
        stopMovement();
        break;
      case 'F':
        goForward(_spd);
        break;
      case 'I':
        goForwardLeft(_spd);
        break;
      case 'G':
        goForwardRight(_spd);
        break;
      case 'B':
        goBackward(_spd);
        break;
      case 'J':
        goBackwardLeft(_spd);
        break;
      case 'H':
        goBackwardRight(_spd);
        break;
      case 'R':
        turnRight(_spd);
        break;
      case 'L':
        turnLeft(_spd);
        break;
      case '0':
        _spdc = 0;
        break;
      case '1':
        _spdc = 1;
        break;
      case '2':
        _spdc = 2;
        break;
      case '3':
        _spdc = 3;
        break;
      case '4':
        _spdc = 4;
        break;
      case '5':
        _spdc = 5;
        break;
      case '6':
        _spdc = 6;
        break;
      case '7':
        _spdc = 7;
        break;
      case '8':
        _spdc = 8;
        break;
      case '9':
        _spdc = 9;
        break;
      case 'q':
        _spdc = 10;
        break;
      case 'V':
        buzzer = true;
        break;
      case 'v':
        buzzer = false;
        break;
      case 'W':
        digitalWrite(F_LIGHT_PIN, HIGH);
        f_light = true;
        break;
      case 'w':
        digitalWrite(F_LIGHT_PIN, LOW);
        f_light = false;
        break;
      case 'U':
        digitalWrite(R_LIGHT_PIN, HIGH);
        r_light = true;
        break;
      case 'u':
        digitalWrite(R_LIGHT_PIN, LOW);
        r_light = false;
        break;
      case 'X':
        xtra = true;
        break;
      case 'x':
        xtra = false;
        break;
      default:
        Serial.print("Unhandled command: "); Serial.println(btcmd);
        break;
    }
  }

  if (buzzer) {
    tone(BUZZ_PIN, buzzfreq);
    delay(buzzint);
    noTone(BUZZ_PIN);

    buzzer = false;
  }

  if (xtra) {
    if (millis() - xtime > xint) {
      xtime = millis();
      xtac = !xtac;
    }

    if (xtac) {
      if (f_light) {
        digitalWrite(F_LIGHT_PIN, HIGH);
      }
      if (r_light) {
        digitalWrite(R_LIGHT_PIN, LOW);
      }
    } else {
      if (f_light) {
        digitalWrite(F_LIGHT_PIN, LOW);
      }
      if (r_light) {
        digitalWrite(R_LIGHT_PIN, HIGH);
      }
    }
  } else {
    if (f_light) {
      digitalWrite(F_LIGHT_PIN, HIGH);
    } else {
      digitalWrite(F_LIGHT_PIN, LOW);
    }
    if (r_light) {
      digitalWrite(R_LIGHT_PIN, HIGH);
    } else {
      digitalWrite(R_LIGHT_PIN, LOW);
    }
  }

  delay(10);
}

void info(String txt) {
  Serial.print("INFO: "); Serial.println(txt);
}

void goForward(byte mspd) {
  int crs = analogRead(CRS_PIN);

  byte rspd = crs2cR(crs) * mspd / 100;
  byte lspd = crs2cL(crs) * mspd / 100;

  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, HIGH);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, HIGH);
  analogWrite(ENA_PIN, rspd);
  analogWrite(ENB_PIN, lspd);
}

void goForwardRight(byte mspd) {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, HIGH);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, HIGH);
  analogWrite(ENA_PIN, mspd);
  analogWrite(ENB_PIN, 0);
}

void goForwardLeft(byte mspd) {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, HIGH);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, HIGH);
  analogWrite(ENA_PIN, 0);
  analogWrite(ENB_PIN, mspd);
}

void goBackward(byte mspd) {
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, HIGH);
  digitalWrite(IN4_PIN, LOW);
  analogWrite(ENA_PIN, mspd);
  analogWrite(ENB_PIN, mspd);
}

void goBackwardRight(byte mspd) {
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, HIGH);
  digitalWrite(IN4_PIN, LOW);
  analogWrite(ENA_PIN, mspd);
  analogWrite(ENB_PIN, 0);
}

void goBackwardLeft(byte mspd) {
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, HIGH);
  digitalWrite(IN4_PIN, LOW);
  analogWrite(ENA_PIN, 0);
  analogWrite(ENB_PIN, mspd);
}

void turnRight(byte mspd) {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, HIGH);
  digitalWrite(IN3_PIN, HIGH);
  digitalWrite(IN4_PIN, LOW);
  analogWrite(ENA_PIN, mspd);
  analogWrite(ENB_PIN, mspd);
}

void turnLeft(byte mspd) {
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, HIGH);
  analogWrite(ENA_PIN, mspd);
  analogWrite(ENB_PIN, mspd);
}

void stopMovement() {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, LOW);
  analogWrite(ENA_PIN, 0);
  analogWrite(ENB_PIN, 0);
}

void fullStop() {
  stopMovement();

  buzzer = false;

  f_light = false;
  r_light = false;

  xtra = false;
}

byte calculateSpeed(byte spdc) {
  if (spdc == 10) {
    return MAX_SPEED;
  }

  byte cspd = MIN_SPEED + (MAX_SPEED - MIN_SPEED) / 10 * spdc;

  return cspd;
}

byte crs2cR(int crs) {
  if (crs == 0) {
    return 100;
  }

  if (crs >= 500) {
    crs = CRS_BASE;
  }

  float q = CRS_BASE - crs;
  float p = q / CRS_BASE * CRS_PCT;

  return 100 - p;
}

byte crs2cL(int crs) {
  if (crs == 0) {
    return 100;
  }

  if (crs < 524) {
    crs = CRS_BASE;
  }

  float q = crs - CRS_BASE;
  float p = q / CRS_BASE * CRS_PCT;

  return 100 - p;
}
