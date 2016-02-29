#include "ClosedLoop.h"

ClosedLoop::ClosedLoop(BoostMode const& _currentMode, BoostMode _modeType, byte _outputPin)
    : ControlMode(_currentMode, _modeType, _outputPin, String(F("EBC: Closed Loop")))
{

}


void ClosedLoop::onUpBtn()
{

}


void ClosedLoop::onDownBtn()
{

}


void ClosedLoop::update()
{
    if (currentMode == modeType) {

    }
}


const String ClosedLoop::getOutputStr()
{
    // TODO: Add proper output
    str = String(F("                "));
    return str;
}
