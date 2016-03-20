#include "ClosedLoop.hpp"
#include "Utils.hpp"


ClosedLoop::ClosedLoop(BoostMode const& _currentMode, BoostMode _modeType, byte _solenoidPin,
                       byte _mapSensorPin, double _atmPSI)
    : ControlMode(_currentMode, _modeType, _solenoidPin, String(F("EBC: Closed Loop")), _atmPSI)
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
    if (boostSetpoint > 0)
        boostSetpoint -= 0.5;
}


void ClosedLoop::onDeactivate()
{
    pid.SetMode(MANUAL);
}


void ClosedLoop::onActivate()
{
    // TODO: Should solenoid be set to initial duty cycle as soon as we enter
    //       closed loop mode, or only when at (SETPOINT - THRESHOLD)?
}


void ClosedLoop::update()
{
    // Calculate the current intake presure in PSIG.
    mapReading = mapVoltageToPSIA(SENSOR_RATING, analogRead(mapSensorPin)) - atmPSI;

    double cutoff = boostSetpoint - CLOSED_LOOP_THRESHOLD;

    if (mapReading < cutoff) {
        // We haven't reached the threshold, close the solenoid.
        pid.SetMode(MANUAL);
        analogWrite(solenoidPin, 0);
    }
    else if (mapReading > cutoff && mapReading < (cutoff + 1)) {
        // Allow for 1 PSI above threshold before handing off control to PID.
        pid.SetMode(MANUAL);
        // TODO: Should this duty cycle be recalculated at some point?
        analogWrite(solenoidPin, CLOSED_LOOP_INIT_DUTY_CYCLE);
    }
    else {
        pid.SetMode(AUTOMATIC);
        if (pid.Compute()) {
            analogWrite(solenoidPin, dutyCycle);
        }
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
