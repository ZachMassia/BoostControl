#include "ClosedLoop.hpp"


ClosedLoop::ClosedLoop(BoostMode const& _currentMode, BoostMode _modeType, byte _solenoidPin,
                       byte _mapSensorPin)
    : ControlMode(_currentMode, _modeType, _solenoidPin, String(F("EBC: Closed Loop")))
    , mapSensorPin(_mapSensorPin)
    , mapReading(0)
    , dutyCycle(0)
    , boostSetpoint(0)
    , Kp(0), Ki(0), Kd(0) // TODO Set default values, or load from PROGMEM
    , pid(&mapReading, &dutyCycle, &boostSetpoint, Kp, Ki, Kd, REVERSE)
{

}


void ClosedLoop::onUpBtn()
{
    boostSetpoint += 0.5;
}


void ClosedLoop::onDownBtn()
{
    boostSetpoint -= 0.5;
}


void ClosedLoop::onDeactivate()
{
    pid.SetMode(MANUAL);
}


void ClosedLoop::onActivate()
{
    pid.SetMode(AUTOMATIC);
}


void ClosedLoop::update()
{
    mapReading = analogRead(mapSensorPin);

    if (pid.Compute()) {
        analogWrite(solenoidPin, dutyCycle);
    }
}


const String ClosedLoop::getOutputStr()
{
    str = String(F("T "));
    str.concat(boostSetpoint);
    str.concat(F("  B "));
    str.concat(mapReading);

    return str;
}
