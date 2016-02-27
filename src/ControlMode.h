#ifndef CONTROLMODE_H
#define CONTROLMODE_H

#include <Arduino.h>
#include "Globals.h"


class ControlMode
{
protected:
    String str;

    const BoostMode &currentMode;
    const BoostMode modeType;
    const byte outputPin;

public:
    ControlMode(BoostMode const& _currentMode, BoostMode _modeType, byte _outputPin)
        : currentMode(_currentMode)
        , modeType(_modeType)
        , outputPin(_outputPin)
        {}

    const String getOutputStr();
};

#endif /* CONTROLMODE_H */
