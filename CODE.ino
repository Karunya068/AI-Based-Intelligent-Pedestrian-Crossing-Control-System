#include <Servo.h>
#include <LedControl.h>

// ----------- Pins -----------
const int PIN_LED_R = 2;
const int PIN_LED_Y = 3;
const int PIN_LED_G = 4;
const int PIN_BUZZ  = 5;

const int PIN_SERVO1 = 9;
const int PIN_SERVO2 = 10;

// MAX7219 (DIN, CLK, CS)
LedControl lc = LedControl(11, 13, 7, 1);

Servo gate1, gate2;

// Adjust angles if needed
const int OPEN_ANGLE  = 0;
const int CLOSE_ANGLE = 90;

// ----------- 8x8 Patterns -----------
byte DIGITS[10][8] = {
  {B00111100,B01100110,B01101110,B01110110,B01100110,B01100110,B00111100,B00000000}, //0
  {B00011000,B00111000,B00011000,B00011000,B00011000,B00011000,B00111100,B00000000}, //1
  {B00111100,B01100110,B00000110,B00001100,B00110000,B01100000,B01111110,B00000000}, //2
  {B00111100,B01100110,B00000110,B00011100,B00000110,B01100110,B00111100,B00000000}, //3
  {B00001100,B00011100,B00101100,B01001100,B01111110,B00001100,B00001100,B00000000}, //4
  {B01111110,B01100000,B01111100,B00000110,B00000110,B01100110,B00111100,B00000000}, //5
  {B00111100,B01100110,B01100000,B01111100,B01100110,B01100110,B00111100,B00000000}, //6
  {B01111110,B00000110,B00001100,B00011000,B00110000,B00110000,B00110000,B00000000}, //7
  {B00111100,B01100110,B01100110,B00111100,B01100110,B01100110,B00111100,B00000000}, //8
  {B00111100,B01100110,B01100110,B00111110,B00000110,B01100110,B00111100,B00000000}  //9
};

byte CHAR_E[8] = {B01111110,B01100000,B01100000,B01111100,B01100000,B01100000,B01111110,B00000000};
byte CHAR_G[8] = {B00111100,B01100110,B01100000,B01101110,B01100110,B01100110,B00111100,B00000000};
byte CHAR_Y[8] = {B01100110,B01100110,B00111100,B00011000,B00011000,B00011000,B00011000,B00000000};

void showPattern(const byte p[8]) {
  for (int row = 0; row < 8; row++) lc.setRow(0, row, p[row]);
}

void showDigit(int d) {
  if (d < 0) d = 0;
  if (d > 9) d = 9;
  showPattern(DIGITS[d]);
}

void setLights(bool r, bool y, bool g) {
  digitalWrite(PIN_LED_R, r ? HIGH : LOW);
  digitalWrite(PIN_LED_Y, y ? HIGH : LOW);
  digitalWrite(PIN_LED_G, g ? HIGH : LOW);
}

void gateOpen() {
  gate1.write(OPEN_ANGLE);
  gate2.write(OPEN_ANGLE);
}

void gateClose() {
  gate1.write(CLOSE_ANGLE);
  gate2.write(CLOSE_ANGLE);
}

String line = "";

void setup() {
  pinMode(PIN_LED_R, OUTPUT);
  pinMode(PIN_LED_Y, OUTPUT);
  pinMode(PIN_LED_G, OUTPUT);
  pinMode(PIN_BUZZ, OUTPUT);

  gate1.attach(PIN_SERVO1);
  gate2.attach(PIN_SERVO2);

  Serial.begin(9600);

  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);

  // default safe state
  setLights(true, false, false);
  gateOpen();
  digitalWrite(PIN_BUZZ, LOW);
  showDigit(0);
}

void applyMode(char mode, int t) {
  if (mode == 'R') {                 // RED countdown
    setLights(true, false, false);
    gateOpen();                      // pedestrians can cross
    digitalWrite(PIN_BUZZ, LOW);
    showDigit(t % 10);
  }
  else if (mode == 'E') {            // EXTEND
    setLights(true, false, false);
    gateClose();                     // prevent new entry
    digitalWrite(PIN_BUZZ, HIGH);
    showPattern(CHAR_E);
  }
  else if (mode == 'G') {            // GREEN
    setLights(false, false, true);
    gateClose();                     // vehicles moving
    digitalWrite(PIN_BUZZ, LOW);
    showPattern(CHAR_G);
  }
  else if (mode == 'Y') {            // YELLOW
    setLights(false, true, false);
    gateClose();
    digitalWrite(PIN_BUZZ, LOW);
    showPattern(CHAR_Y);
  }
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      // Expect format: X,T  (example: R,7)
      char mode = line.length() ? line.charAt(0) : 'R';
      int comma = line.indexOf(',');
      int t = 0;
      if (comma > 0) t = line.substring(comma + 1).toInt();

      applyMode(mode, t);
      line = "";
    } else if (c != '\r') {
      line += c;
      if (line.length() > 32) line = "";
    }
  }
}