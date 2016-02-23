#include "OpenLoop.h"

OpenLoop::OpenLoop(const BoostMode &_currentMode, BoostMode _modeType, byte _outputPin, byte _dutyCycle)
    : ControlMode(_currentMode, _modeType)
    , dutyCycle(_dutyCycle)
    , outputPin(_outputPin)
{

}

void OpenLoop::update()
{
    if (currentMode == modeType) {
        analogWrite(outputPin, dutyCycle);
    } else {
        analogWrite(outputPin, 0);
    }
}

const String OpenLoop::getOutputStr()
{
    str = String(F("D "));
    str.concat(dutyCycle);
    str.concat(F(" B "));
    str.concat(F("12.7")); // TODO: Replace with actual boost level.
    str.concat(F(" PSI"));

    return str;
}

void OpenLoop::increase() { dutyCycle++; }

void OpenLoop::decrease() { dutyCycle--; }
