#ifndef BUTTONTHREAD_H
#define BUTTONTHREAD_H

#include <Arduino.h>
#include <Thread.h>

class ButtonThread: public Thread
{
private:
    const byte pin;                 // The button's pin #.
    const double debounceTime;      // In seconds.
    const double sampleFrequency;   // In Hz.
    const double maximum;

    unsigned int integrator;    // Will range from 0 to 'maximum'.
    byte output;
    byte lastOutput;
    bool lastPushRead;          // If false, button has been pushed, but not queried.

    void update();      // Read the button and update the integrator.

public:
    ButtonThread(byte _pin, double _debounceTime, double _sampleFrequency);

    void run();         // Override of Thread::run()

    byte getOutput();   // Get the current button state.
    bool wasPushed();   // Check if button has been pushed since last queried.
};

#endif /* BUTTONTHREAD_H */
