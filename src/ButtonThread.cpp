#include "ButtonThread.h"

ButtonThread::ButtonThread(byte _pin, double _debounceTime, double _sampleFrequency)
    : pin(_pin)
    , debounceTime(_debounceTime)
    , sampleFrequency(_sampleFrequency)
    , maximum(debounceTime * sampleFrequency)
    , output(LOW)
    , lastOutput(LOW)
    , lastPushRead(true)
{
    // Make sure the pin is in digital input mode.
    pinMode(pin, INPUT);

    //setInterval(1 / sampleFrequency); // Convert Hz to milliseconds. (T = 1/f)
}

void ButtonThread::update()
{
    byte input = digitalRead(pin);
    lastOutput = output;

    // Update the integrator based on the input signal.
    if (input == LOW && integrator > 0) {
        integrator--;
    }
    else if (integrator < maximum) {
        integrator++;
    }

    // Update the output state based on the integrator.
    if (integrator == 0) {
        output = LOW;
    }
    else if (integrator >= maximum) {
        output = HIGH;
        integrator = maximum;

        if (lastOutput == LOW) { // Avoid button repeat.
            lastPushRead = false;
        }
    }
}

void ButtonThread::run()
{
    update();
    runned();
}

byte ButtonThread::getOutput()
{
    return output;
}

bool ButtonThread::wasPushed()
{
    if (!lastPushRead) { // Last push has not yet been queried.
        lastPushRead = true;
        return true;
    }
    return false;
}
