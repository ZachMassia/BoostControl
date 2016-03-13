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
    const byte solenoidPin;

public:
    const String headerStr;

    ControlMode(BoostMode const& _currentMode, BoostMode _modeType, byte _solenoidPin, String _headerStr)
        : currentMode(_currentMode)
        , modeType(_modeType)
        , solenoidPin(_solenoidPin)
        , headerStr(_headerStr)
        {}

    virtual void onUpBtn()      {}
    virtual void onDownBtn()    {}
    virtual void onDeactivate() {}
    virtual void onActivate()   {}

    virtual void update() = 0;

    virtual const String getOutputStr() = 0;
};

#endif /* CONTROLMODE_H */
