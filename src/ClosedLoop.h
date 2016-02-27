#ifndef CLOSEDLOOP_H
#define CLOSEDLOOP_H

#include <Arduino.h>
#include "Globals.h"
#include "ControlMode.h"


class ClosedLoop : public ControlMode
{
private:

public:
    ClosedLoop(BoostMode const& _currentMode, BoostMode _modeType, byte _outputPin);
};

#endif /* CLOSEDLOOP_H */
