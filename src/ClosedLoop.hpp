#ifndef CLOSEDLOOP_H
#define CLOSEDLOOP_H

#include <Arduino.h>
#include <PID_v1.h>
#include "Globals.h"
#include "ControlMode.hpp"



class ClosedLoop : public ControlMode
{
private:
    byte   mapSensorPin;    // Input sensor pin.
    double mapReading;      // The 0-5V signal from the MAP sensor.
    double dutyCycle;       // The current duty cycle of the solenoid.
    double boostSetpoint;   // The desired boost level.
    double Kp, Ki, Kd;      // PID tuning variables.

    PID pid;    // The PID controller object.


public:
    ClosedLoop(BoostMode const& _currentMode, BoostMode _modeType, byte _solenoidPin, byte _mapSensorPin);

    virtual void onUpBtn();
    virtual void onDownBtn();
    virtual void onDeactivate();
    virtual void onActivate();
    virtual void update();

    virtual const String getOutputStr();
};

#endif /* CLOSEDLOOP_H */
