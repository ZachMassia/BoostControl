#ifndef OPENLOOP_H
#define OPENLOOP_H

#include <Arduino.h>
#include "Globals.h"
#include "ControlMode.h"

class OpenLoop: public ControlMode
{
private:
    byte dutyCycle;


public:
    OpenLoop(BoostMode const& _currentMode, BoostMode _modeType, byte _dutyCycle, byte outputPin);

    virtual void onUpBtn();
    virtual void onDownBtn();
    virtual void update();

    virtual const String getOutputStr();
};

#endif /* OPENLOOP_H */
