//#define DEBUG
#define ON_PIN 3
#define BT_PIN 5
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
  pinMode(BT_PIN, INPUT_PULLUP);

#ifdef DEBUG
  Serial.begin(9600);
#endif
}

bool trig = false;
bool on = false;
bool bt = false;
int ir, pr;
unsigned long t = 0;
char _src = '?';

bool irTrig(int ir, int pr, bool on) {
  if (_src == 'b') {
    return false;
  }

  if ((!on and ir < IR_TRH and pr < PR_TRH) or (on and ir < IR_TRH)) {
    if (on) {
      _src = '?';
    } else {
      _src = 'i';
    }

    return true;
  }

  return false;
}

bool btPush(bool bt, bool on) {
  if (bt) {
    if (on) {
      _src = '?';
    } else {
      _src = 'b';
    }

    return true;
  }

  return false;
}

void loop() {
  ir = analogRead(IR_PIN);
  pr = analogRead(PR_PIN);
  bt = digitalRead(BT_PIN) == LOW;

  trig = (irTrig(ir, pr, on) or btPush(bt, on));

#ifdef DEBUG
  Serial.print("IR: ");
  Serial.print(ir);
  Serial.print(" / PR: ");
  Serial.print(pr);
  Serial.print(" / BT: ");
  Serial.print(bt);
  Serial.print(" / SRC: ");
  Serial.println(_src);
#endif

  if (trig) {
    on = !on;

    digitalWrite(ON_PIN, on);
    delay(5 * graceMillis);

    if (on) {
      t = millis();
    }
  }

  if (_src == 'i') {
    if (on) {
      if (millis() - t > lightMillis) {
        digitalWrite(ON_PIN, !on);
        t = millis();
        _src = '?';
      }
    }
  }

  delay(graceMillis);
}
