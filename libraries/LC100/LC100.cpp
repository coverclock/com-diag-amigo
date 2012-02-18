/**
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA.
 * Licensed under the terms in README.h.
 * Chip Overclock mailto:coverclock@diag.com
 * http://www.diag.com/navigation/downloads/Amigo.html
 */

#include "LC100.h"
#include <string.h>

#define DEBUG (0)

namespace com {
namespace diag {
namespace amigo {
    
/*******************************************************************************
 * UTILITIES
 ******************************************************************************/

#if DEBUG
static void printch(byte ch, boolean eol = false) {
  if ((0x21 <= ch) && (ch <= 0x7e)) {
    Serial.print('\'');
    Serial.write(ch);
    if (eol) { Serial.println('\''); } else { Serial.print('\''); }
  } else {
    Serial.print("0x");
    if (eol) { Serial.println(ch, HEX); } else { Serial.print(ch, HEX); }
  }
}
#endif

byte LC100Base::index(byte row) {
  return ((rowHome + row) % ROWS);
}

byte LC100Base::index(byte col, byte row) {
  return (index(row) * COLS) + (col % COLS);
}

byte LC100Base::one(byte value) {
  return (value > 0) ? (value - 1) : 0; // If 0 or missing, default is 1, and (1 - 1) is 0.
}

void LC100Base::roll(byte row) {
  rowHome = row % ROWS;
}
  
/*******************************************************************************
 * CURSOR
 ******************************************************************************/

void LC100Base::setCursor(byte col, byte row) {
#if DEBUG
  if (debugging) {
    Serial.print("setCursor ");
    Serial.print(col);
    Serial.print(' ');
    Serial.println(row);
  }
#endif
  colCurrent = col % COLS;
  rowCurrent = row % ROWS;
  lcd.setCursor(colCurrent, rowCurrent);
}
  
/*******************************************************************************
 * FRAME BUFFER
 ******************************************************************************/

size_t LC100Base::emit(uint8_t ch) {
#if DEBUG
  if (debugging) {
    Serial.print("emit ");
    printch(ch);
    Serial.print(' ');
    Serial.print(COLS);
    Serial.print(' ');
    Serial.print(ROWS);
    Serial.print(' ');
    Serial.print(colCurrent);
    Serial.print(' ');
    Serial.print(rowCurrent);
    Serial.print(' ');
    Serial.print(rowHome);
    Serial.print(' ');
    Serial.print(index(rowCurrent));
    Serial.print(' ');
    Serial.print(index(colCurrent, rowCurrent));
    Serial.print(' ');
    Serial.println(lineLength[index(rowCurrent)]);
  }
#endif
  size_t result = lcd.write(ch);
  frameBuffer[index(colCurrent++, rowCurrent)] = ch;
  if (ch == ' ') {
    if (lineLength[index(rowCurrent)] == colCurrent) {
      lineLength[index(rowCurrent)] = colCurrent - 1;
    }
  } else {
    if (lineLength[index(rowCurrent)] < colCurrent) {
      lineLength[index(rowCurrent)] = colCurrent;
    }
  }
  if (milliseconds > 0) {
    delay(milliseconds);
  }
  return result;
}

void LC100Base::erase(byte colFrom, byte rowFrom, byte colTo, byte rowTo) {
#if DEBUG
  if (debugging) {
    Serial.print("erase ");
    Serial.print(colFrom);
    Serial.print(' ');
    Serial.print(rowFrom);
    Serial.print(' ');
    Serial.print(colTo);
    Serial.print(' ');
    Serial.println(rowTo);
  }
#endif
  byte colHome = colCurrent;
  byte rowHome = rowCurrent;
  colFrom %= COLS;
  rowFrom %= ROWS;
  colTo %= COLS;
  rowTo %= ROWS;
  for (byte row = rowFrom; row <= rowTo; ++row) {
    setCursor(colFrom, row);
    for (byte col = colFrom; col <= colTo; ++col) {
      emit(' ');
    }
    if (colTo < (COLS - 1)) {
      // Do nothing.
    } else if (colFrom == 0) {
      lineLength[index(row)] = 0;
    } else {
      lineLength[index(row)] = colFrom - 1;
    }
  }
  setCursor(colHome, rowHome);
}

size_t LC100Base::frame(uint8_t ch) {
#if DEBUG
  if (debugging) {
    Serial.print("frame ");
    printch(ch, true);
  }
#endif
  size_t result = 0;
  do {
    if (colCurrent < COLS) {
      // Do nothing.
    } else if (!wrapping) {
      break;
    } else if (rowCurrent < (ROWS - 1)) {
      setCursor(0, rowCurrent + 1);
      lineLength[index(rowCurrent)] = 0;
    } else if (!scrolling) {
      erase(0, rowCurrent, COLS - 1, rowCurrent);
      setCursor(0, rowCurrent);
      lineLength[index(rowCurrent)] = 0;
    } else {
      up();
    }
    result = emit(ch);
  } while (false);
  return result;
}

/*******************************************************************************
 * SCROLLING
 ******************************************************************************/

void LC100Base::down() {
#if DEBUG
  if (debugging) {
    Serial.print("down ");
    Serial.print(rowBegin);
    Serial.print(' ');
    Serial.println(rowEnd);
  }
#endif
  lcd.clear();
  for (byte row = rowEnd; row > rowBegin; --row) {
    lcd.setCursor(0, row);
    for (byte col = 0; col < lineLength[index(row - 1)]; ++col) {
      lcd.write(frameBuffer[index(col, row - 1)]);
    }
  }
  setCursor(0, rowBegin);
  rowHome = (rowHome + ROWS - 1) % ROWS;
  lineLength[index(rowCurrent)] = 0;
}

void LC100Base::up() { 
#if DEBUG
  if (debugging) {
    Serial.print("up ");
    Serial.print(rowBegin);
    Serial.print(' ');
    Serial.println(rowEnd);
  }
#endif
  lcd.clear();
  for (byte row = rowBegin; row < rowEnd; ++row) {
    lcd.setCursor(0, row);
    for (byte col = 0; col < lineLength[index(row + 1)]; ++col) {
      lcd.write(frameBuffer[index(col, row + 1)]);
    }
  }
  setCursor(0, rowEnd);
  rowHome = (rowHome + 1) % ROWS;
  lineLength[index(rowCurrent)] = 0;
}

/*******************************************************************************
 * DISPLAY
 ******************************************************************************/

void LC100Base::home() {
#if DEBUG
  if (debugging) {
    Serial.print("home");
  }
#endif
  colCurrent = 0;
  rowCurrent = 0;
  lcd.home();
}

void LC100Base::clear() {
#if DEBUG
  if (debugging) {
    Serial.println("clear");
  }
#endif
  lcd.clear();
  memset(lineLength, 0, ROWS);
  memset(frameBuffer, ' ', ROWS * COLS);
  rowHome = rowBegin;
  setCursor(rowCurrent, colCurrent); // MS-DOS homes the cursor, but leaving it where it was seems to be the standard.
}

size_t LC100Base::write(uint8_t ch0) {
  size_t result = 1;
  int ch = ch0;
  Action action = CONSUMED;
  byte colTemporary;
  while (ch > 0) {
 #if DEBUG
    if (debugging) {
      Serial.print("write ");
      printch(ch);
      Serial.write(' ');
      Serial.print(levelCurrent);
      Serial.write(' ');
      Serial.write(state[levelCurrent]);
      Serial.write(' ');
      Serial.write(action);
      Serial.write(' ');
      Serial.println(decimal);
    }
#endif
    switch (state[levelCurrent]) {
      case DATA:
        switch (ch) {
          case '\b':
            setCursor((colCurrent + COLS - 1), rowCurrent);
            break;
          case '\t':
            for (byte col = colCurrent + 1; col < COLS; ++col) {
              if (tabSettings[col]) {
                setCursor(col, rowCurrent);
                break;
              }
            }
            break;
          case '\n':
            colTemporary = newlining? 0 : colCurrent;
            if (rowCurrent < (ROWS - 1)) {
              setCursor(colTemporary, rowCurrent + 1);
              lineLength[index(rowCurrent)] = 0;
            } else {
              up();
              setCursor(colTemporary, rowCurrent);
            }
            break;
          case '\v':
            setCursor(colCurrent, rowCurrent + 1);
            break;
          case '\f':
            colCurrent = 0;
            rowCurrent = 0;
            clear();
            break;
          case '\r':
            if (!newlining) {
              setCursor(0, rowCurrent);
            } else if (rowCurrent < (ROWS - 1)) {
              setCursor(0, rowCurrent + 1);
              lineLength[index(rowCurrent)] = 0;
            } else {
              up();
            }
            break;
          case ESC:
            state[++levelCurrent] = ESCAPE;
            break;
          case DEL:
            setCursor(colCurrent + COLS - 1, rowCurrent);
            emit(' ');
            setCursor(colCurrent + COLS - 1, rowCurrent);
            break;
          default:
            result = frame(ch);
            break;
        }
        break;
      case ESCAPE:
        switch (ch) {
          case '[':
            state[levelCurrent] = BRACKET;
            break;
          case '(':
          case ')':
            action = COMPLETE;
            break;
          case 'c':
            wrapping = true;
            scrolling = true;
            action = COMPLETE;
            break;
          case 'D':
            down();
            action = COMPLETE;
            break;
          case 'M':
            up();
            action = COMPLETE;
            break;
          case '7':
            colSaved = colCurrent;
            rowSaved = rowCurrent;
            action = COMPLETE;
            break;
          case '8':
            colCurrent = colSaved;
            rowCurrent = rowSaved;
            action = COMPLETE;
            break;
          case 'H':
            tabSettings[colCurrent] = true;
            action = COMPLETE;
            break;
          default:
            action = UNKNOWN;
            break;
        }
        break;
      case BRACKET:
        switch (ch) {
          case 'A':
            setCursor(colCurrent, rowCurrent + ROWS - 1);
            action = COMPLETE;
            break;
          case 'B':
            setCursor(colCurrent, rowCurrent + 1);
            action = COMPLETE;
            break;
          case 'C':
            setCursor(colCurrent + 1, rowCurrent);
            action = COMPLETE;
            break;
          case 'D':
            setCursor(colCurrent + COLS - 1, rowCurrent);
            action = COMPLETE;
            break;
          case 'H':
            colCurrent = 0;
            rowCurrent = 0;
            lcd.home();
            action = COMPLETE;
            break;
          case 'J':
            erase(colCurrent, rowCurrent, COLS - 1, rowCurrent);
            if (rowCurrent < (ROWS - 1)) {
              erase(0, rowCurrent + 1, COLS - 1, ROWS - 1);
            }
            action = COMPLETE;
            break;
          case 'K':
            erase(colCurrent, rowCurrent, COLS - 1, rowCurrent);
            action = COMPLETE;
            break;
          case 'c':
            Serial.write(ESC);
            Serial.print("[10c");
            action = COMPLETE;
            break;
          case 'g':
            tabSettings[colCurrent] = false;
            action = COMPLETE;
            break;
          case '0':
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          case '8':
          case '9':
            decimal = 0;
            state[levelCurrent++] = FIRST;
            state[levelCurrent] = DECIMAL;
            action = CONTINUE;
            break;
          case '?':
            decimal = 0;
            state[levelCurrent++] = QUESTION;
            state[levelCurrent] = DECIMAL;
            break;
          default:
            action = UNKNOWN;
            break;
        }
        break;
      case FIRST:
        switch (ch) {
          case ';':
            first = decimal;
            decimal = 0;
            state[levelCurrent++] = SECOND;
            state[levelCurrent] = DECIMAL;
            action = CONSUMED;
            break;
          case 'A':
            setCursor(colCurrent, rowCurrent + ROWS - (one(decimal) % ROWS));
            action = COMPLETE;
            break;
          case 'B':
            setCursor(colCurrent, rowCurrent + one(decimal));
            action = COMPLETE;
            break;
          case 'C':
            setCursor(colCurrent + one(decimal), rowCurrent);
            action = COMPLETE;
            break;
          case 'D':
            setCursor(colCurrent + COLS - (one(decimal) % ROWS), rowCurrent);
            action = COMPLETE;
            break;
          case 'J':
            switch (decimal) {
              case 0:
                erase(colCurrent, rowCurrent, COLS - 1, rowCurrent);
                if (rowCurrent < (ROWS - 1)) {
                  erase(0, rowCurrent + 1, COLS - 1, ROWS - 1);
                }
                action = COMPLETE;
                break;
              case 1:
                if (rowCurrent > 0) {
                  erase(0, 0, COLS - 1, rowCurrent - 1);
                }
                erase(0, rowCurrent, colCurrent, rowCurrent);
                action = COMPLETE;
                break;
              case 2:
                clear();
                action = COMPLETE;
                break;
              default:
                action = UNKNOWN;
                break;
            }
            break;
          case 'K':
            switch (decimal) {
              case 0:
                erase(colCurrent, rowCurrent, COLS - 1, rowCurrent);
                action = COMPLETE;
                break;
              case 1:
                erase(0, rowCurrent, colCurrent, rowCurrent);
                action = COMPLETE;
                break;
              case 2:
                erase(0, rowCurrent, COLS - 1, rowCurrent);
                action = COMPLETE;
                break;
              default:
                action = UNKNOWN;
                break;
            }
            break;
          case 'g':
            switch (decimal) {
              case 3:
                for (byte col = 0; col < COLS; ++col) {
                  tabSettings[col] = false;
                }
                break;
              default:
                action = UNKNOWN;
                break;
            }
            break;
          case 'h':
            switch (decimal) {
              case 7:
                wrapping = true;
                action = COMPLETE;
                break;
              case 20:
                newlining = true;
                action = COMPLETE;
                break;
              default:
                action = UNKNOWN;
                break;
            }
            break;
          case 'l':
            switch (decimal) {
              case 7:
                wrapping = false;
                action = COMPLETE;
                break;
              case 20:
                newlining = false;
                action = COMPLETE;
                break;
              default:
                action = UNKNOWN;
                break;
            }
            break;
          case 'n':
            switch (decimal) {
              case 5:
                Serial.write(ESC);
                Serial.print("[0n");
                action = COMPLETE;
                break;
              case 6:
                Serial.write(ESC);
                Serial.print('[');
                Serial.print(rowCurrent);
                Serial.print(';');
                Serial.print(colCurrent);
                Serial.print('R');
                action = COMPLETE;
                break;
              case 7:
                Serial.write(ESC);
                Serial.print("[0n");
                action = COMPLETE;
                break;
              default:
                action = UNKNOWN;
                break;
            }
            break;
          default:
            action = UNKNOWN;
            break;
        }
        break;
      case SECOND:
        switch (ch) {
          case 'r':
            rowBegin = one(first) % ROWS;
            rowEnd = one(decimal) % ROWS;
            action = COMPLETE;
            break;
          case 'H':
          case 'f':
            setCursor(one(decimal), one(first));
            action = COMPLETE;
            break;
          default:
            action = UNKNOWN;
            break;
        }
        break;
      case QUESTION:
        switch (ch) {
          case 'h':
            switch (decimal) {
              case 7:
                wrapping = true;
                action = COMPLETE;
                break;
              default:
                action = UNKNOWN;
                break;
            }
            break;
          case 'l':
            switch (decimal) {
              case 7:
                wrapping = false;
                action = COMPLETE;
                break;
              default:
                action = UNKNOWN;
                break;
            }
            break;
          default:
            action = UNKNOWN;
            break;
        }
        break;
      case DECIMAL:
        switch (ch) {
          case '0':
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          case '8':
          case '9':
            decimal = (decimal * 10) + (ch - '0');
            action = CONSUMED;
            break;
          default:
            action = TERMINAL;
            break;
        }
        break;
      default:
        action = INSANE;
        break;
    }
    switch (action) {
      case INSANE:
        levelCurrent = 1;
        state[0] = DATA;
      case UNKNOWN:
        frame(ch);
      case COMPLETE:
        --levelCurrent;
      case CONSUMED:
        ch = -1;
      case CONTINUE:
        break;
      case TERMINAL:
        --levelCurrent;
        break;
    }
  }
  return result;
}
  
/*******************************************************************************
 * JOYSTICK
 ******************************************************************************/

int LC100Base::read(char * buffer /* [4] */) {
  int length = 0;
  Display::Read sample = lcd.read();
  if (!sampling) {
    while ((sample != Display::NONE) && (lcd.read() == sample)) {
        // Do nothing.
    }
  }
#if DEBUGGING
  if (debugging && (sample != Display::NONE)) {
     Serial.println("read ");
     printch(sample, true);
  }
#endif
  switch (sample) {
    case Display::LEFT:
    case Display::DOWN:
    case Display::UP:
    case Display::RIGHT:
      length = 4;
      buffer[0] = ESC;
      buffer[1] = '[';
      switch (sample) {
        case Display::LEFT:
          buffer[2] = 'D';
          break;
        case Display::DOWN:
          buffer[2] = 'B';
          break;
        case Display::UP:
          buffer[2] = 'A';
          break;
        case Display::RIGHT:
          buffer[2] = 'C';
          break;
        default:
          // Do nothing.
          break;
      }
      buffer[3] = '\0';
      break;
    case Display::SELECT:
      length = 2;
      buffer[0] = '\n';
      buffer[1] = '\0';
      break;
    default:
      // Do nothing.
      break;
  }
  return length;
}
  
/*******************************************************************************
 * INITIALIZATION
 ******************************************************************************/

void LC100Base::begin() {
  memset(lineLength, 0, ROWS);
  memset(frameBuffer, ' ', ROWS * COLS);
  memset(tabSettings, 0, COLS);
  state[levelCurrent] = DATA;
  lcd.begin(COLS, ROWS);
  clear();
}

}
}
}

