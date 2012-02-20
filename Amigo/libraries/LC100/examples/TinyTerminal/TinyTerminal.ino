/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include <LiquidCrystal.h>
#include "LC100.h"

/**
 * @def DEBUG
 * If DEBUG is defined, TinyTerminal replaces the interface to the actual
 * LCD hardware with a mock object that implements the same interface. The
 * mock object just traces itself on the serial output.
 */
#define DEBUG (0)

#if DEBUG

/**
 * This is a mock display that merely traces itself on the serial output and
 * implements the movement keys used by the visual editor (vi).
 */
class Mock
: public com::diag::amigo::Display
{

public:

  /**
   * Ctor.
   */
  Mock() {}

  /**
   * Dtor.
   */
  virtual ~Mock() {}

  /**
   * Initialize. This needs to be done only once.
   * @param cols is the number of columns in this display.
   * @param rows is the number of rows in this display.
   */
  virtual void begin(byte cols, byte rows) {
    Serial.print("BEGIN ");
    Serial.print(cols);
    Serial.print(' ');
    Serial.println(rows);
  }

  /**
   * Place the cursor at the home position.
   */
  virtual void home() {
    Serial.println("HOME");
  }

  /**
   * Clear the display and place the cursor at the home position.
   */
  virtual void clear() {
    Serial.println("CLEAR");
  }

  /**
   * Place the cursor at the specified position.
   * @param col is the zero-based column number.
   * @param row is the zero-based row number.
   */
  virtual void setCursor(byte col, byte row) {
    Serial.print("CURSOR ");
    Serial.print(col);
    Serial.print(' ');
    Serial.println(row);
  }

  /**
   * Write a character to the display. The signed value that is returned will
   * typically be one, but can be zero if the specified character was somehow
   * invalid, or negative if an error writing the character occurred.
   * @param ch is the character that is written to the display.
   * @return the number of characters written.
   */
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

  /**
   * Read the joystick. The returned value will be an enumerated value
   * indicating movement left, down, up, or right, a select indication, or
   * no input.
   * @return an enumerated value.
   */
  virtual Read read() {
    char ch = (Serial.available() > 0) ? Serial.read() : '\0';
    Read sample;
    switch (ch) {
	  case 'h':
	  case 'H':
		  sample = LEFT;
		  break;
      case 'j':
      case 'J':
        sample = DOWN;
        break;
      case 'k':
      case 'K':
        sample = UP;
        break;
      case 'l':
      case 'L':
        sample = RIGHT;
        break;
      case ' ':
      case '\n':
      case '\t':
        sample = SELECT;
        break;
      default:
        sample = NONE;
        break;
    }
    return sample;
  }

};

/**
 * This is the mock display object.
 */
static Mock display;

#else

/**
 * DFRobot implements an LC100 interface to the DFRobot shield which has a
 * 16 column by 2 row liquid crystal display (LCD) and a two-dimensional
 * joystick that provides the stimuli up, down, left, right, and select.
 */
class DFRobot
: public com::diag::amigo::Display
{

protected:

  LiquidCrystal & lcd;

public:

  /**
   * Ctor.
   */
  DFRobot(LiquidCrystal & hardware) : lcd(hardware) {}

  /**
   * Dtor.
   */
  virtual ~DFRobot() {}

  /**
   * Initialize. This needs to be done only once.
   * @param cols is the number of columns in this display.
   * @param rows is the number of rows in this display.
   */
  virtual void begin(byte cols, byte rows) { lcd.begin(cols, rows); }

  /**
   * Place the cursor at the home position.
   */
  virtual void home() { lcd.home(); }

  /**
   * Clear the display and place the cursor at the home position.
   */
  virtual void clear() { lcd.clear(); }

  /**
   * Place the cursor at the specified position.
   * @param col is the zero-based column number.
   * @param row is the zero-based row number.
   */
  virtual void setCursor(byte col, byte row) { lcd.setCursor(col, row); }

  /**
   * Write a character to the display. The signed value that is returned will
   * typically be one, but can be zero if the specified character was somehow
   * invalid, or negative if an error writing the character occurred.
   * @param ch is the character that is written to the display.
   * @return the number of characters written.
   */
  virtual size_t write(uint8_t ch) { return lcd.write(ch); }

  /**
   * Read the joystick. The returned value will be an enumerated value
   * indicating movement left, down, up, or right, a select indication, or
   * no input.
   * @return an enumerated value.
   */
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

/**
 * This is the object that actually controls the DFRobot shield using the
 * standard class from the Arduino library.
 */
static LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

/**
 * This is the object the implements the interface between the standard Arduino
 * class and the LC100 software.
 */
static DFRobot display(lcd);

#endif

/**
 * The DFRobot shield has sixteen columns.
 */
static const byte COLS = 16;

/**
 * The DFRobot shield as two rows.
 */
static const byte ROWS = 2;

/**
 * This is the LC100 object that provides a frame buffer of COLS columns and
 * ROWS rows to support scrolling, and which implements a large subset of the
 * VT100 escape sequences.
 */
static com::diag::amigo::LC100<COLS, ROWS> lc100(display, false, true, 1000 / 8);

#if 0
/**
 * Line wrapping and screen scrolling unit test.
 */
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
/**
 * Line placement unit test.
 */
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
/**
 * Line ending unit test.
 */
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
/**
 * Up scrolling unit test.
 */
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
/**
 * Down scrolling unit test.
 */
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
/**
 * Cursor control unit test.
 */
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
/**
 * Erasure unit test.
 */
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
/**
 * Line erasure unit test.
 */
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
/**
 * First screen erasure unit test.
 */
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
/**
 * Second screen erasure unit test.
 */
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
/**
 * No unit test.
 */
static void test() {
}
#endif

/**
 * Setup is executed once by the Arduino run-time software. This function
 * initializes the serial port, the LC100 software (which in turn initializes
 * the underlying display hardware), and runs a unit test which may or may not
 * do anything.
 */
void setup() {
  Serial.begin(9600);
  lc100.begin();
  test();
}

/**
 * Loop is executed continuously by the Arduino run-time software, which also
 * does some other housekeeping each iteration, like polling the serial port
 * for activity. The timing of the execution of this function is asynchronous.
 * This function checks for input on the serial port and if it exists passes
 * all of the buffered input characters to the LC100 software. It also checks
 * for input from the LC100 software from the joystick and if it exists passes
 * it to the serial port.
 */
void loop () {
  while (Serial.available() > 0) {
    lc100.write(Serial.read());
  }
  char buffer[4];
  if (lc100.read(buffer) > 0) {
    Serial.print(buffer);
  }
}

