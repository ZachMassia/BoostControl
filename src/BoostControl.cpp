#include <Arduino.h>
#include <Wire.h>

#include <LiquidCrystal.h>

#include <PID_v1.h>
#include <PID_AutoTune_v0.h>

#include <Thread.h>
#include <ThreadController.h>

#include "Globals.h"
#include "ButtonThread.h"
#include "OpenLoop.h"

// Threads
ThreadController controller;
ButtonThread     upBtn(UP_BTN_PIN,     BTN_DEBOUNCE_TIME, BTN_SAMPLE_FREQ);
ButtonThread     downBtn(DOWN_BTN_PIN, BTN_DEBOUNCE_TIME, BTN_SAMPLE_FREQ);
ButtonThread     modeBtn(MODE_BTN_PIN, BTN_DEBOUNCE_TIME, BTN_SAMPLE_FREQ);

// Runtime settings
BoostMode currentBoostMode = Off;

// Objects
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
OpenLoop      openLoop(currentBoostMode, OpenLoopMode, OUTPUT_PIN, OPEN_LOOP_INIT_DUTY_CYCLE);


// Functions
void initButtons();
void toggleBoostMode();
void updateLCDBoostMode();

void onOpenLoopMode();


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

    switch (currentBoostMode) {
        case Off:
            // TODO: Make sure writing pin high makes turbo run off wastegate spring pressure
            digitalWrite(OUTPUT_PIN, HIGH);
            lcd.setCursor(0, 1);
            lcd.print(F("                "));
            break;

        case OpenLoopMode:
            onOpenLoopMode();
            break;

        case ClosedLoopMode:
            lcd.setCursor(0, 1);
            lcd.print(F("                "));
            break;

        default:
            break;
    }
}


void initButtons()
{
    ButtonThread *buttons[] = { &upBtn, &downBtn, &modeBtn };

    for (byte i = 0; i < 3; i++) {
        buttons[i]->setInterval(BTN_THREAD_INTERVAL);
        controller.add(buttons[i]);
    }
    //controller.add(&modeBtn);
}


void toggleBoostMode()
{
    currentBoostMode = (BoostMode)((currentBoostMode + 1) % TotalModeCount);
}


void updateLCDBoostMode()
{
    lcd.home();
    switch (currentBoostMode)
    {
        case Off:
            lcd.print(F("EBC:     Off    "));
            break;
        case OpenLoopMode:
            lcd.print(F("EBC:   Open Loop"));
            break;
        case ClosedLoopMode:
            lcd.print(F("EBC: Closed Loop"));
        default:
            break;
    }
}

void onOpenLoopMode()
{
    if (upBtn.wasPushed()) {
        openLoop.increase();
    } else if (downBtn.wasPushed()) {
        openLoop.decrease();
    }

    openLoop.update();

    lcd.setCursor(0, 1);
    lcd.print(openLoop.getOutputStr());
}
