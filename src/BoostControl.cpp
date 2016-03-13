#include <Arduino.h>
#include <Wire.h>

#include <LiquidCrystal.h>
#include <Thread.h>
#include <ThreadController.h>

#include "Globals.h"
#include "ButtonThread.hpp"
#include "ControlMode.hpp"
#include "OpenLoop.hpp"
#include "ClosedLoop.hpp"

// Threads
ThreadController controller;
ButtonThread     upBtn(UP_BTN_PIN,     BTN_DEBOUNCE_TIME, BTN_SAMPLE_FREQ);
ButtonThread     downBtn(DOWN_BTN_PIN, BTN_DEBOUNCE_TIME, BTN_SAMPLE_FREQ);
ButtonThread     modeBtn(MODE_BTN_PIN, BTN_DEBOUNCE_TIME, BTN_SAMPLE_FREQ);

// Runtime settings
BoostMode   currentBoostMode = Off;
ControlMode *previousMode  = nullptr;

// Objects
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
OpenLoop      openLoop(currentBoostMode, OpenLoopMode, SOLENOID_PIN, OPEN_LOOP_INIT_DUTY_CYCLE);
ClosedLoop    closedLoop(currentBoostMode, ClosedLoopMode, SOLENOID_PIN, MAP_SENSOR_PIN);


// Functions
void initButtons();
void toggleBoostMode();
void updateLCDBoostMode();

ControlMode *getModePtr();


void setup() // ------------------------------------------------------------------------------------
{
    Serial.begin(9600);
    lcd.begin(LCD_W, LCD_H);

    initButtons();

    updateLCDBoostMode();
}

void loop() // -------------------------------------------------------------------------------------
{
    controller.run();

    if (modeBtn.wasPushed()) {
        toggleBoostMode();
        updateLCDBoostMode();
    }

    ControlMode *currentMode getModePtr();   // Must be after potential call to toggleBoostMode().

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

        lcd.setCursor(0, 1);
        lcd.print(currentMode->getOutputStr());
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


ControlMode *getModePtr()
{
    static ControlMode *modes[] = { nullptr, &openLoop, &closedLoop };

    return modes[currentBoostMode];
}
