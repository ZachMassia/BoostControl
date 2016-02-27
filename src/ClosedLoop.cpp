#include "ClosedLoop.h"

ClosedLoop::ClosedLoop(BoostMode const& _currentMode, BoostMode _modeType, byte _outputPin)
    : ControlMode(_currentMode, _modeType, _outputPin)
{
    
}
