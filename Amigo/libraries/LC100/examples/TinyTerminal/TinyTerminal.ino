/**
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA.
 * Licensed under the terms in README.h.
 * Chip Overclock mailto:coverclock@diag.com
 * http://www.diag.com/navigation/downloads/Amigo.html
 */

#include <LiquidCrystal.h>
#include "LC100.h"

#define DEBUG (0)

#if DEBUG

class Mock
: public com::diag::amigo::Display
{

public:

  Mock() {}

  virtual ~Mock() {}

  virtual void begin(byte cols, byte rows) {
    // Do nothing.
  }

  virtual void home() {
    Serial.println("HOME");
  }

  virtual void clear() {
    Serial.println("CLEAR");
  }

  virtual void setCursor(byte col, byte row) {
    Serial.print("CURSOR ");
    Serial.print(col);
    Serial.print(' ');
    Serial.println(row);
  }

  virtual size_t write(uint8_t ch) {
    Serial.print("WRITE ");
    if ((0x21 <= ch) && (ch <= 0x7e)) {
        Serial.print('\'');
        Serial.write((char)ch);
        Serial.println('\'');
     } else {
        Serial.print("0x");
        Serial.println(ch, HEX);
      }
    return 1;
  }

  virtual Read read() {
    char ch = (Serial.available() > 0) ? Serial.read() : '\0';
    Read sample;
    switch (ch) {
      case 'r':
      case 'R':
        sample = RIGHT;
        break;
      case 'u':
      case 'U':
        sample = UP;
        break;
      case 'd':
      case 'D':
        sample = DOWN;
        break;
      case 'l':
      case 'L':
        sample = LEFT;
        break;
      case ' ':
        sample = SELECT;
        break;
      default:
        sample = NONE;
        break;
    }
    return sample;
  }

};

static Mock display;

#else

class DFRobot
: public com::diag::amigo::Display
{

protected:

  LiquidCrystal & lcd;

public:

  DFRobot(LiquidCrystal & hardware) : lcd(hardware) {}

  virtual ~DFRobot() {}

  virtual void begin(byte cols, byte rows) { lcd.begin(cols, rows); }

  virtual void home() { lcd.home(); }

  virtual void clear() { lcd.clear(); }

  virtual void setCursor(byte col, byte row) { lcd.setCursor(col, row); }

  virtual size_t write(uint8_t ch) { return lcd.write(ch); }

  virtual Read read() {
    Read sample;
    int key = analogRead(0);
    if      (key > 1000) sample = NONE;
    else if (key < 50)   sample = RIGHT;
    else if (key < 195)  sample = UP;
    else if (key < 380)  sample = DOWN;
    else if (key < 555)  sample = LEFT;
    else if (key < 790)  sample = SELECT;
    else                 sample = NONE;
    return sample;
  }

};

static LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
static DFRobot display(lcd);

#endif

static const byte COLS = 16;
static const byte ROWS = 2;
static com::diag::amigo::LC100<COLS, ROWS> lc100(display, false, true, 1000 / 8);

#if 0
static void test() {
  for (char row = '0'; row <= '9'; ++row) {
    for (char col = '0'; col <= '7'; ++col) {
      lc100.write(row);
      lc100.write(col);
    }
  }
}
#endif

#if 0
static void test() {
  lc100.clear();
  lc100.home();
  for (char row = '0'; row <= '1'; ++row) {
    for (char col = '0'; col <= '7'; ++col) {
      lc100.write(row);
      lc100.write(col);
    }
    lc100.setCursor(0, 1);
  }
}
#endif

#if 0
static void test() {
  for (char row = '0'; row <= '9'; ++row) {
    for (char col = '0'; col <= '5'; ++col) {
      lc100.write(row);
      lc100.write(col);
    }
    lc100.write('\r');
    lc100.write('\n');
  }
  lc100.write('\f');
  for (char row = '0'; row <= '9'; ++row) {
    for (char col = '0'; col <= '5'; ++col) {
      lc100.write(row);
      lc100.write(col);
    }
    lc100.write('\r');
    lc100.write('\n');
  }
}
#endif

#if 0
static void test() {
  lc100.setCursor(0, 1);
  for (char row = '0'; row <= '9'; ++row) {
    for (char col = '0'; col <= '7'; ++col) {
      lc100.write(row);
      lc100.write(col);
    }
    lc100.up();
  }
}
#endif

#if 0
static void test() {
  lc100.setCursor(0, 0);
  for (char row = '0'; row <= '9'; ++row) {
    for (char col = '0'; col <= '7'; ++col) {
      lc100.write(row);
      lc100.write(col);
    }
    lc100.down();
  }
}
#endif

#if 0
static char TEST[] = {
  lc100.ESC, '[', 'H',
  '0',
  lc100.ESC, '[', '1', ';', '1', '6', 'H',
  '1',
  lc100.ESC, '[', '2', ';', '1', '6', 'H',
  '2',
  lc100.ESC, '[', '2', ';', '1', 'H',
  '3'
};
static void test() {
  for (byte ii = 0; ii < (sizeof(TEST)/sizeof(TEST[0])); ++ii) {
    lc100.write(TEST[ii]);
  }
}
#endif

#if 0
static void test() {
  for (byte row = 0; row < ROWS; ++row) {
    for (byte col = 0; col < COLS; col += 2) {
      lc100.write('0' + row);
      lc100.write('0' + (col / 2));
    }
  }
  lc100.erase(1, 0, COLS - 2, 1);
}
#endif

#if 0
static char TEST[] = {
  lc100.ESC, '[', '1', ';', '8', 'H',
  lc100.ESC, '[', 'K',
  lc100.ESC, '[', '2', ';', '8', 'H',
  lc100.ESC, '[', '1', 'K',
};
static void test() {
  for (byte row = 0; row < ROWS; ++row) {
    for (byte col = 0; col < COLS; col += 2) {
      lc100.write('0' + row);
      lc100.write('0' + (col / 2));
    }
  }
  for (byte ii = 0; ii < (sizeof(TEST)/sizeof(TEST[0])); ++ii) {
    lc100.write(TEST[ii]);
  }
}
#endif

#if 0
static char TEST[] = {
  lc100.ESC, '[', '2', ';', '1', '4', 'H',
  lc100.ESC, '[', '1', 'J',
};
static void test() {
  for (byte row = 0; row < ROWS; ++row) {
    for (byte col = 0; col < COLS; col += 2) {
      lc100.write('0' + row);
      lc100.write('0' + (col / 2));
    }
  }
  for (byte ii = 0; ii < (sizeof(TEST)/sizeof(TEST[0])); ++ii) {
    lc100.write(TEST[ii]);
  }
}
#endif

#if 0
static char TEST[] = {
  lc100.ESC, '[', '1', ';', '3', 'H',
  lc100.ESC, '[', 'J',
};
static void test() {
  for (byte row = 0; row < ROWS; ++row) {
    for (byte col = 0; col < COLS; col += 2) {
      lc100.write('0' + row);
      lc100.write('0' + (col / 2));
    }
  }
  for (byte ii = 0; ii < (sizeof(TEST)/sizeof(TEST[0])); ++ii) {
    lc100.write(TEST[ii]);
  }
}
#endif

#if 1
static void test() {
}
#endif

void setup() {
  Serial.begin(9600);
  lc100.begin();
  test();
}

void loop () {
  while (Serial.available() > 0) {
    lc100.write(Serial.read());
  }
  char buffer[4];
  if (lc100.read(buffer) > 0) {
    Serial.print(buffer);
  }
}

