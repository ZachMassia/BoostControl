#ifndef OPENLOOP_H
#define OPENLOOP_H

#include <Arduino.h>
#include "Globals.h"
#include "ControlMode.h"

class OpenLoop: public ControlMode
{
private:
    byte dutyCycle;
    const byte outputPin;

public:
    OpenLoop(BoostMode const& _currentMode, BoostMode _modeType, byte _dutyCycle, byte outputPin);

    void update();
    const String getOutputStr();

    void increase();
    void decrease();
};

#endif /* OPENLOOP_H */
