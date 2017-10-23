/*
PPMReader library based on https://github.com/Hasi123/read-any-ppm
Works with Servo library

Paweł (DzikuVx) Spychalski 2017
https://quadmeup.com

License: GNU GPL v3
*/

#include "PPMReader.h"
#include "Arduino.h"

#define CPU_SPEED_MULTIPLIER (F_CPU/8000000)

volatile int PPMReader::ppm[PPMREADER_PMM_CHANNEL_COUNT];
volatile bool ppmReaderUseTimer = false;

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

void PPMReader::start(void) {
    attachInterrupt(_interrupt, PPMReader::handler, CHANGE);
}

void PPMReader::stop(void) {
    detachInterrupt(_interrupt);
}

static void PPMReader::handler()
{
    static unsigned int pulse;
    static unsigned long counter;
    static byte channel;
    static unsigned long previousCounter = 0;
    static unsigned long currentMicros = 0;

    if (ppmReaderUseTimer) {
        counter = TCNT1 * CPU_SPEED_MULTIPLIER;
        TCNT1 = 0;
    } else {
        currentMicros = micros();
        counter = currentMicros - previousCounter;
        previousCounter = currentMicros;
    }

    if (counter < 510) { //must be a pulse
        pulse = counter;
    }
    else if (counter > 1910)
    { //sync
        channel = 0;
    }
    else
    { //servo values between 810 and 2210 will end up here
        ppm[channel] = counter + pulse;
        channel++;
    }
}