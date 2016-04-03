#include <Arduino.h>
#include <Wire.h>

#include <LiquidCrystal.h>
#include <Thread.h>
#include <ThreadController.h>

#include "Globals.h"
#include "Utils.hpp"
#include "ButtonThread.hpp"
#include "ControlMode.hpp"
#include "OpenLoop.hpp"
#include "ClosedLoop.hpp"

// Threads
ThreadController controller;
ButtonThread     upBtn(UP_BTN_PIN,     BTN_DEBOUNCE_TIME, BTN_SAMPLE_FREQ);
ButtonThread     downBtn(DOWN_BTN_PIN, BTN_DEBOUNCE_TIME, BTN_SAMPLE_FREQ);
ButtonThread     modeBtn(MODE_BTN_PIN, BTN_DEBOUNCE_TIME, BTN_SAMPLE_FREQ);
Thread           lcdThread;

// Runtime settings
bool        loggingEnabled   = true;
BoostMode   currentBoostMode = Off;
ControlMode *previousMode    = nullptr;
double      atmPSI           = 0.0;

// Objects
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
OpenLoop*      openLoop = nullptr;
ClosedLoop*    closedLoop = nullptr;


// Functions
void initButtons();
void toggleBoostMode();
void updateLCDBoostMode();
void updateLCDModeOutput();
void logSensors();

ControlMode *getModePtr();


void setup() // ------------------------------------------------------------------------------------
{
    Serial.begin(500000);
    lcd.begin(LCD_W, LCD_H);

    initButtons();

    lcdThread.onRun(updateLCDModeOutput);
    lcdThread.setInterval(LCD_UPDATE_TIME_MS);
    controller.add(&lcdThread);

    updateLCDBoostMode();

    // Grab the current atmospheric pressure.
    atmPSI = mapVoltageToPSIA(SENSOR_RATING, analogRead(MAP_SENSOR_PIN));

    openLoop   = new OpenLoop(currentBoostMode, OpenLoopMode, SOLENOID_PIN, OPEN_LOOP_INIT_DUTY_CYCLE, atmPSI);
    closedLoop = new ClosedLoop(currentBoostMode, ClosedLoopMode, SOLENOID_PIN, MAP_SENSOR_PIN, atmPSI);

    Serial.println(closedLoop->getLogStrFormat());
}

void loop() // -------------------------------------------------------------------------------------
{
    controller.run();

    if (modeBtn.wasPushed()) {
        toggleBoostMode();
        updateLCDBoostMode();
    }

    ControlMode *currentMode = getModePtr();   // Must be after potential call to toggleBoostMode().

    if (currentMode != previousMode) { // We've just changed boost modes.
        if (previousMode != nullptr) {
            previousMode->onDeactivate();
        }
        if (currentMode != nullptr) {
            currentMode->onActivate();
        }
        previousMode = currentMode;
    }

    if (currentMode == nullptr) {
        // TODO: Make sure writing pin high makes turbo run off wastegate spring pressure
        digitalWrite(SOLENOID_PIN, HIGH);

        lcd.setCursor(0, 1);
        lcd.print(F("                "));
    } else {
        if (upBtn.wasPushed()) {
            currentMode->onUpBtn();
        }
        if (downBtn.wasPushed()) {
            currentMode->onDownBtn();
        }

        currentMode->update();
        logSensors();
    }
}


void initButtons()
{
    ButtonThread *buttons[] = { &upBtn, &downBtn, &modeBtn };

    for (byte i = 0; i < 3; i++) {
        buttons[i]->setInterval(BTN_THREAD_INTERVAL);
        controller.add(buttons[i]);
    }
}


void toggleBoostMode()
{
    currentBoostMode = (BoostMode)((currentBoostMode + 1) % TotalModeCount);
}


void updateLCDBoostMode()
{
    ControlMode *mode = getModePtr();

    lcd.home();

    if (mode == nullptr) {
        lcd.print(F("EBC:     Off    "));
    } else {
        lcd.print(mode->headerStr);
    }
}


void updateLCDModeOutput()
{
    ControlMode *mode = getModePtr();

    if (mode == nullptr) {
        return;
    }

    lcd.setCursor(0, 1);
    lcd.print(mode->getOutputStr());
}


void logSensors()
{
    if (loggingEnabled && currentBoostMode == OpenLoopMode) {
        Serial.println(closedLoop->getLogStr());
    }
}


ControlMode *getModePtr()
{
    ControlMode *modes[] = { nullptr, openLoop, closedLoop };

    return modes[currentBoostMode];
}
