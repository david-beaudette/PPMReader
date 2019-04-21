/*
PPMReader library based on https://github.com/Hasi123/read-any-ppm
Works with Servo library

Paweł (DzikuVx) Spychalski 2017
https://quadmeup.com

License: GNU GPL v3
*/

#include "PPMReader.h"
#include "Arduino.h"

#define NO_UPDATE_THRESHOLD 500 //if no update in this number of ms, raise alarm

volatile int PPMReader::ppm[PPMREADER_PMM_CHANNEL_COUNT];
volatile bool ppmReaderUseTimer = false;
volatile uint32_t lastPacketUpdate = 0; 

PPMReader::PPMReader(int pin, int interrupt, bool useTimer)
{
  _pin = pin;
  _interrupt = interrupt;
  ppmReaderUseTimer = useTimer;

  pinMode(_pin, INPUT);
  attachInterrupt(_interrupt, PPMReader::handler, CHANGE);
}

int PPMReader::get(uint8_t channel)
{
  return ppm[channel];
}

bool PPMReader::isReceiving(void) {
  if (millis() - lastPacketUpdate > NO_UPDATE_THRESHOLD) {
    return false;
  } else {
    return true;
  }
}

void PPMReader::start(void) {
  attachInterrupt(_interrupt, PPMReader::handler, CHANGE);
}

void PPMReader::stop(void) {
  detachInterrupt(_interrupt);
}

void PPMReader::handler()
{
  static unsigned int pulse;
  static unsigned long counter;
  static byte channel;
  static unsigned long previousCounter = 0;
  static unsigned long currentMicros = 0;
  int tmpVal;

	currentMicros = micros();
	counter = currentMicros - previousCounter;
	previousCounter = currentMicros;

  if (counter < 450) { 
    // Low state of 400 us between each channel
    pulse = counter;
  }
  else if (counter > 2500)
  { 
    // Long high state between channel blocks
    channel = 0;
    lastPacketUpdate = millis();
  }
  else
  { // Servo values will end up here
    tmpVal = counter + pulse;
    if (tmpVal > 900 && tmpVal < 2200) {
      ppm[channel] = tmpVal;;
    }
    channel++;
  }
}