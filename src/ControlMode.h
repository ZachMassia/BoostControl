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

public:
    ControlMode(BoostMode const& _currentMode, BoostMode _modeType)
        : currentMode(_currentMode)
        , modeType(_modeType)
        {}

    const String getOutputStr();
};

#endif /* CONTROLMODE_H */
