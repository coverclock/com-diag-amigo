#ifndef _COM_DIAG_AMIGO_H_
#define _COM_DIAG_AMIGO_H_

/**
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA.
 * Licensed under the terms in README.h.
 * Chip Overclock mailto:coverclock@diag.com
 * http://www.diag.com/navigation/downloads/Amigo.html
 */

#include <Arduino.h>
#include <Print.h>
#include <stdint.h>

namespace com {
namespace diag {
namespace amigo {

struct Display
{

  enum Read {
    NONE    = '\0',
    LEFT    = 'h',
    DOWN    = 'j',
    UP      = 'k',
    RIGHT   = 'l',
    SELECT  = '\n'
  };
  
  virtual void begin(byte cols, byte rows) = 0;

  virtual void home() = 0;

  virtual void clear() = 0;

  virtual void setCursor(byte col, byte row) = 0;

  virtual size_t write(uint8_t ch) = 0;

  virtual Read read() = 0;

};

class LC100Base
: public Print
{

protected:

  enum Constant {
    STATES = 5
  };

  enum State {
    DATA     = 'D',
    ESCAPE   = 'E',
    DECIMAL  = 'N',
    BRACKET  = 'B',
    FIRST    = 'F',
    SECOND   = 'S',
    QUESTION = 'Q'
  };

  enum Action {
    CONSUMED  = 'C',
    CONTINUE  = 'O',
    COMPLETE  = 'P',
    TERMINAL  = 'T',
    UNKNOWN   = 'U',
    INSANE    = 'I'
  };
 
public:

  enum Ascii {
    ESC      = '\x1b',
    DEL      = '\x7f'
  };

  LC100Base(Display & display, byte cols, byte rows, byte * lineLengthArray, uint8_t * frameBufferArray, boolean * tabSettingsArray, boolean sample = false, boolean debug = false, int ms = 0)
  : Print()
  , milliseconds(ms)
  , debugging(debug)
  , sampling(sample)
  , scrolling(true)
  , wrapping(true)
  , newlining(false)
  , lcd(display)
  , COLS(cols)
  , ROWS(rows)
  , rowHome(0)
  , colCurrent(0)
  , rowCurrent(0)
  , rowBegin(0)
  , rowEnd((rows > 0) ? rows - 1 : 0)
  , colSaved(0)
  , rowSaved(0)
  , first(0)
  , decimal(0)
  , levelCurrent(0)
  , lineLength(lineLengthArray)
  , frameBuffer(frameBufferArray)
  , tabSettings(tabSettingsArray)
  {}

  virtual ~LC100Base() {}
  
  void begin();
   
  void setCursor(byte col, byte row);
 
  void home();

  void down();
  
  void up();
  
  void erase(byte colFrom, byte rowFrom, byte colTo, byte rowTo);
  
  void clear();

  int read(char * buffer /* [4] */);

  size_t write(uint8_t ch);
  
  using Print::write;

  void roll(byte row); // Mostly just for unit testing.

private:

  int milliseconds;

  State state[STATES];

  boolean debugging;
  boolean sampling;
  boolean scrolling;
  boolean wrapping;
  boolean newlining;

  Display & lcd;
  
public:
 
  const byte COLS;
  const byte ROWS;
  
private:

  byte rowHome;
  byte colCurrent;
  byte rowCurrent;
  byte rowBegin;
  byte rowEnd;
  byte colSaved;
  byte rowSaved;
  byte first;
  byte decimal;
  byte levelCurrent;

  byte * lineLength;
  uint8_t * frameBuffer;
  boolean * tabSettings;
  
protected:

  byte index(byte col, byte row);

  byte index(byte row);
  
  size_t emit(uint8_t ch);
   
  size_t frame(uint8_t ch);

  byte one(byte value);

};

template <byte _COLS_, byte _ROWS_> class LC100
: public LC100Base
{

private:

  byte lineLength[_ROWS_];
  uint8_t frameBuffer[_ROWS_ * _COLS_];
  boolean tabSettings[_COLS_];

public:
  
  LC100(Display & display, boolean sample = false, boolean debug = false, int ms = 0)
  : LC100Base(display, _COLS_, _ROWS_, lineLength, frameBuffer, tabSettings, sample, debug, ms)
  {}

  virtual ~LC100() {}

};

}
}
}

#endif /* _COM_DIAG_AMIGO_H_ */

