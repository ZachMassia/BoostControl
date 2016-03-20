#include "OpenLoop.hpp"

OpenLoop::OpenLoop(const BoostMode &_currentMode, BoostMode _modeType, byte _solenoidPin,
                   byte _dutyCycle, double _atmPSI)
    : ControlMode(_currentMode, _modeType, _solenoidPin, String(F("EBC:   Open Loop")), _atmPSI)
    , dutyCycle(_dutyCycle)
{

}


void OpenLoop::update()
{
    analogWrite(solenoidPin, dutyCycle);
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


void OpenLoop::onUpBtn() { dutyCycle++; }


void OpenLoop::onDownBtn() { dutyCycle--; }
