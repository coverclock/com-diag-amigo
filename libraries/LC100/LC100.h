#ifndef _COM_DIAG_AMIGO_LC100_H_
#define _COM_DIAG_AMIGO_LC100_H_

/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include <Arduino.h>
#include <Print.h>
#include <stdint.h>

namespace com {
namespace diag {
namespace amigo {

/**
 * This pure (abstract) class defines the interface that the LC100 software
 * expects to be implemented by any display that it uses.
 */
struct Display
{

  /**
   * The joystick may return one of these stimuli when read.
   */
  enum Read {
    NONE    = '\0',
    LEFT    = 'h',
    DOWN    = 'j',
    UP      = 'k',
    RIGHT   = 'l',
    SELECT  = '\n'
  };

  /**
   * Initialize. This needs to be done only once.
   * @param cols is the number of columns in this display.
   * @param rows is the number of rows in this display.
   */
  virtual void begin(byte cols, byte rows) = 0;

  /**
   * Place the cursor at the home position.
   */
  virtual void home() = 0;

  /**
   * Clear the display and place the cursor at the home position.
   */
  virtual void clear() = 0;

  /**
   * Place the cursor at the specified position. The column and row coordinates
   * are taken modulo of the actual display dimensions.
   * @param col is the zero-based column number.
   * @param row is the zero-based row number.
   */
  virtual void setCursor(byte col, byte row) = 0;

  /**
   * Write a character to the display. The signed value that is returned will
   * typically be one, but can be zero if the specified character was somehow
   * invalid, or negative if an error writing the character occurred.
   * @param ch is the character that is written to the display.
   * @return the number of characters written.
   */
  virtual size_t write(uint8_t ch) = 0;

  /**
   * Read the joystick. The returned value will be an enumerated value
   * indicating movement left, down, up, or right, a select indication, or
   * no input.
   * @return an enumerated value.
   */
  virtual Read read() = 0;

};

/**
 * This is the common base (super) class for the LC100 software that does all
 * of the heavy lifting for any LC100 template instantiation regardless of the
 * display dimensions. It derives from (extends) the Arduino Print class so
 * that any of the usual Print methods can be used by an application. This class
 * is not intended to be used by itself, although there is no reason why you
 * couldn't do so.
 */
class LC100Base
: public Print
{

protected:

  /**
   * Manifest constants.
   */
  enum Constant {
    STATES = 5
  };

  /**
   * States in which the push down automaton may be. DATA is the start state.
   * There is no end state. All of the enumerated values are printable, making
   * it easy to trace the PDA as it executes.
   */
  enum State {
    DATA     = 'D',
    ESCAPE   = 'E',
    DECIMAL  = 'N',
    BRACKET  = 'B',
    FIRST    = 'F',
    SECOND   = 'S',
    QUESTION = 'Q'
  };

  /**
   * Actions which the push down automaton may execute. CONSUMED is the default
   * action. All of the enumerated values are printable, making it easy to trace
   * the PDA as it executes.
   */
  enum Action {
    CONSUMED  = 'C',
    CONTINUE  = 'O',
    COMPLETE  = 'P',
    TERMINAL  = 'T',
    UNKNOWN   = 'U',
    INSANE    = 'I'
  };
 
public:

  /**
   * Special ASCII characters.
   */
  enum Ascii {
    ESC      = '\x1b',
    DEL      = '\x7f'
  };

  /**
   * Ctor.
   * @param display refers to the object that implements the Display interface.
   * @param cols is the number of columns in the display.
   * @param rows is the number of rows in the display.
   * @param lineLengthArray points to an array of dimension [rows] which will
   *        be used to store the length in bytes of the displayed line in each
   *        row of the display.
   * @param frameBufferArray points to an array of dimensions [rows][cols] that
   *        is used as a frame buffer to support scrolling.
   * @param tabSettingsArray points to an array of dimension [cols] that is
   *        used to keep track of the tab settings for the display.
   * @param sample if true causes the joystick value to be returned continuously
   *        as long as a button is pressed; if false, the joystick value is
   *        returned intermittently when the button is released.
   * @param debug enables debug output if debugging was compiled in.
   * @param ms is the number of milliseconds to delay between each individual
   *        update of the display, which can make debugging a lot easier.
   */
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

  /**
   * Dtor.
   */
  virtual ~LC100Base() {}
  
  /**
   * Initialize this object. This only needs to be called once.
   */
  void begin();
   
  /**
   * Place the cursor at the specified position. The column and row coordinates
   * are taken modulo of the actual display dimensions.
   * @param col is the zero-based column number.
   * @param row is the zero-based row number.
   */
  void setCursor(byte col, byte row);
 
  /**
   * Place the cursor at the home position.
   */
  void home();

  /**
   * Scroll the scroll area down, leaving the cursor placed at a blank line at
   * the top of the scroll area. By default, the scroll area is the entire
   * display.
   */
  void down();
  
  /**
   * Scroll the scroll area up, leaving the cursor placed at a blank line at
   * the bottom of the scroll area. By default, the scroll area is the entire
   * display.
   */
  void up();
  
  /**
   * Erase a square bordered by the specified upper left and lower right corners
   * inclusive. Erasing is done by writing blanks into the display left to
   * right, top to bottom. The cursor is placed back in its original position.
   * @param colFrom is the zero-based column of the upper left corner of the
   *        erased square.
   * @param rowFrom is the zero-based row of the upper left corner of the erased
   *        square.
   * @param colTo is the zero-based column of the lower right corner of the
   *        erased square.
   * @param rowTo is the zero-based row of the lower right corner of the erased
   *        square.
   */
  void erase(byte colFrom, byte rowFrom, byte colTo, byte rowTo);
  
  /**
   * Clear the display and place the cursor at the home position.
   */
  void clear();

  /**
   * Read the current joy stick stimulus into a buffer of at least four bytes
   * in length. If the joy stick is indicating movement, the buffer will contain
   * a VT100 (ANSI) arrow escape sequence indicating the direction of movement.
   * The buffer will be nul-terminated, allowing it to be written directly to
   * the serial port. Return the number of bytes placed into the buffer, zero
   * indicating that there is no joy stick stimulus at this time.
   * @param buffer points to a buffer of at least four bytes in length.
   * @return the number of bytes placed in the buffer.
   */
  int read(char * buffer /* [4] */);

  /**
   * Write the current character to the display. This character may be part of
   * a VT100 (ANSI) escape sequence, in which case it is not actually written
   * to the display, but will be executed once the complete escape sequence is
   * captured.
   * @param ch is the character to be written to the display.
   * @return the number of characters processed, which could be zero if the
   *         character is somehow invalid in the context of the current escape
   *         sequence, or negative is an error occurred.
   */
  size_t write(uint8_t ch);
  
  /**
   * Inherit all of the other write methods from the Print class.
   */
  using Print::write;

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

  /**
   * This is the number of columns in the display.
   */
  const byte COLS;

  /**
   * This is the number of rows in the display.
   */
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

  /**
   * Convert a zero-based column and row coordinate into a frame buffer index.
   * @param col is the zero-based column value.
   * @param row is the zero-based row value.
   * @return a frame buffer index.
   */
  byte index(byte col, byte row);

  /**
   * Concert a zero-based row value into an array index.
   * @param row is the zero-based row value.
   * @return an array index.
   */
  byte index(byte row);
  
  /**
   * Emit a character, which both displays it on the actual display and stores
   * it appropriately in the frame buffer. The returned value is the number of
   * characters processed, which is nominally one but may be zero if the
   * character is somehow invalid or negative if an error occurred.
   * @param ch is the character to be emitted.
   * @return the number of characters emitted.
   */
  size_t emit(uint8_t ch);
   
  /**
   * Frame a character appropriately by dealing with line wrapping (if enabled)
   * and screen scrolling (ditto).
   * @param ch is the character to be framed.
   * @return the number of characters framed.
   */
  size_t frame(uint8_t ch);

  /**
   * Convert a one-based column or row value into a zero-based column or row
   * value.
   * @param value is the one-based value.
   * @return the zero-based value.
   */
  byte one(byte value);

};

/**
 * This is the derived (sub) class for the LC100 software that contains the
 * actual data structures whose sizes depend on the actual size of the display.
 * It is templatized so that the display dimensions can be passed as arguments
 * at compile time.
 */
template <byte _COLS_, byte _ROWS_> class LC100
: public LC100Base
{

private:

  byte lineLength[_ROWS_];
  uint8_t frameBuffer[_ROWS_ * _COLS_];
  boolean tabSettings[_COLS_];

public:
  
  /**
   * Ctor.
   * @param display refers to the object that implements the Display interface.
   * @param sample if true causes the joystick value to be returned continuously
   *        as long as a button is pressed; if false, the joystick value is
   *        returned intermittently when the button is released.
   * @param debug enables debug output if debugging was compiled in.
   * @param ms is the number of milliseconds to delay between each individual
   *        update of the display, which can make debugging a lot easier.
   */
  LC100(Display & display, boolean sample = false, boolean debug = false, int ms = 0)
  : LC100Base(display, _COLS_, _ROWS_, lineLength, frameBuffer, tabSettings, sample, debug, ms)
  {}

  /**
   * Dtor.
   */
  virtual ~LC100() {}

};

}
}
}

#endif /* _COM_DIAG_AMIGO_LC100_H_ */

