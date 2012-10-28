/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

unsigned int days = 0;
unsigned int hours = 0;
unsigned int minutes = 0;
unsigned int seconds = 0;

unsigned long then = 0;
unsigned long now = 0;

void report() {
  then = now;

  Serial.print(days);
  Serial.print(':');
  if (hours < 10) { Serial.print('0'); }
  Serial.print(hours);
  Serial.print(':');
  if (minutes < 10) { Serial.print('0'); }
  Serial.print(minutes);
  Serial.print(':');
  if (seconds < 10) { Serial.print('0'); }
  Serial.println(seconds);

  if (seconds < 59) {
    ++seconds;
  } else if (minutes < 59) {
    seconds = 0;
    ++minutes;
  } else if (hours < 23) {
    seconds = 0;
    minutes = 0;
    ++hours;
  } else {
    seconds = 0;
    minutes = 0;
    hours = 0;
    ++days;
  }
}

void setup() {
  Serial.begin(9600);
  now = millis();
  report();
}

void loop() {
  for (now = millis(); (now - then) < 1000; now = millis()) {
    delay(1);
  }
  report();
}
