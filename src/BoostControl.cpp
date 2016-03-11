#include <Arduino.h>
#include <Wire.h>

#include <LiquidCrystal.h>
#include <Thread.h>
#include <ThreadController.h>

#include "Globals.h"
#include "ButtonThread.h"
#include "ControlMode.h"
#include "OpenLoop.h"
#include "ClosedLoop.h"

// Threads
ThreadController controller;
ButtonThread     upBtn(UP_BTN_PIN,     BTN_DEBOUNCE_TIME, BTN_SAMPLE_FREQ);
ButtonThread     downBtn(DOWN_BTN_PIN, BTN_DEBOUNCE_TIME, BTN_SAMPLE_FREQ);
ButtonThread     modeBtn(MODE_BTN_PIN, BTN_DEBOUNCE_TIME, BTN_SAMPLE_FREQ);

// Runtime settings
BoostMode   currentBoostMode = Off;
ControlMode *currentModePtr  = NULL;

// Objects
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
OpenLoop      openLoop(currentBoostMode, OpenLoopMode, OUTPUT_PIN, OPEN_LOOP_INIT_DUTY_CYCLE);
ClosedLoop    closedLoop(currentBoostMode, ClosedLoopMode, OUTPUT_PIN, MAP_SENSOR_PIN);


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

    ControlMode *mode = getModePtr();   // Must be after potential call to toggleBoostMode().

    if (mode != currentModePtr) { // We've just changed boost modes.
        currentModePtr->onDeactivate();
        mode->onActivate();
        currentModePtr = mode;
    }

    if (mode == NULL) {
        // TODO: Make sure writing pin high makes turbo run off wastegate spring pressure
        digitalWrite(OUTPUT_PIN, HIGH);

        lcd.setCursor(0, 1);
        lcd.print(F("                "));
    } else {
        if (upBtn.wasPushed()) {
            currentModePtr->onUpBtn();
        }
        if (downBtn.wasPushed()) {
            currentModePtr->onDownBtn();
        }

        currentModePtr->update();

        lcd.setCursor(0, 1);
        lcd.print(currentModePtr->getOutputStr());
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

    if (mode == NULL) {
        lcd.print(F("EBC:     Off    "));
    } else {
        lcd.print(mode->headerStr);
    }
}


ControlMode *getModePtr()
{
    static ControlMode *modes[] = { NULL, &openLoop, &closedLoop };

    return modes[currentBoostMode];
}
