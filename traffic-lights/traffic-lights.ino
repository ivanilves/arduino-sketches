#define PIN_RED 2
#define PIN_YELLOW 3
#define PIN_GREEN 4
#define PIN_MODE_YELLOW 12

#define COLOR_RED 0
#define COLOR_YELLOW 1
#define COLOR_GREEN 2

#define DELAY_RED 5000
#define DELAY_YELLOW 1500
#define DELAY_GREEN 5000
#define DELAY_BLINK 250
#define DELAY_BUTTON_PRESS 250

#define MODE_AUTO 0
#define MODE_YELLOW 1

unsigned long lastColorSwitchTime = millis();
unsigned long lastModeSwitchTime = millis();

unsigned int mode = MODE_AUTO;
unsigned int color = COLOR_RED;
unsigned int prevColor;
bool on = true;

void setup() {
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_YELLOW, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_MODE_YELLOW, INPUT_PULLUP);
}

void setRedLight(bool on) {
  digitalWrite(PIN_YELLOW, LOW);
  digitalWrite(PIN_GREEN, LOW);

  if (on) {
    digitalWrite(PIN_RED, HIGH);
  } else {
    digitalWrite(PIN_RED, LOW);
  }
}

void setYellowLight(bool on) {
  digitalWrite(PIN_RED, LOW);
  digitalWrite(PIN_GREEN, LOW);

  if (on) {
    digitalWrite(PIN_YELLOW, HIGH);
  } else {
    digitalWrite(PIN_YELLOW, LOW);
  }
}

void setGreenLight(bool on) {
  digitalWrite(PIN_RED, LOW);
  digitalWrite(PIN_YELLOW, LOW);

  if (on) {
    digitalWrite(PIN_GREEN, HIGH);
  } else {
    digitalWrite(PIN_GREEN, LOW);
  }
}

void setAllLightsOn() {
  digitalWrite(PIN_RED, HIGH);
  digitalWrite(PIN_YELLOW, HIGH);
  digitalWrite(PIN_GREEN, HIGH);
}

void setLight(int color, bool on) {
  switch (color) {
    case COLOR_RED:
      setRedLight(on);
      break;
    case COLOR_YELLOW:
      setYellowLight(on);
      break;
    case COLOR_GREEN:
      setGreenLight(on);
      break;
    default:
      setAllLightsOn();
      break;
  }
}

bool yellowButtonPressed() {
  return digitalRead(PIN_MODE_YELLOW) == LOW;
}

void loop() {
  if (millis() - lastModeSwitchTime > DELAY_BUTTON_PRESS) {
    if (yellowButtonPressed()) {
      if (mode != MODE_YELLOW) {
        mode = MODE_YELLOW;
        color = COLOR_YELLOW;
      } else {
        mode = MODE_AUTO;
        color = COLOR_RED;
      }
      on = true;
      lastColorSwitchTime = millis();
      lastModeSwitchTime = millis();
    }
  }

  switch (mode) {
    case MODE_AUTO:
      switch (color) {
        case COLOR_RED:
          if (millis() - lastColorSwitchTime > DELAY_RED) {
            lastColorSwitchTime = millis();
            color = COLOR_YELLOW;
            prevColor = COLOR_RED;
          }
          break;
        case COLOR_YELLOW:
          if (millis() - lastColorSwitchTime > DELAY_YELLOW) {
            lastColorSwitchTime = millis();
            if (prevColor == COLOR_GREEN) {
              color = COLOR_RED;
            } else {
              color = COLOR_GREEN;
            }
            prevColor = COLOR_YELLOW;
          }
          break;
        case COLOR_GREEN:
          if (millis() - lastColorSwitchTime > DELAY_GREEN) {
            lastColorSwitchTime = millis();
            color = COLOR_YELLOW;
            prevColor = COLOR_GREEN;
          }
          break;
        default:
          setAllLightsOn();
          break;
      }
      break;
    case MODE_YELLOW:
      if (millis() - lastColorSwitchTime > DELAY_BLINK) {
        lastColorSwitchTime = millis();
        if (on) {
          on = false;
        } else {
          on = true;
        }
      }
      break;
  }

  setLight(color, on);

  delay(10);
}
