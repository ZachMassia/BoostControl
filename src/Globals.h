#ifndef GLOBALS_H
#define GLOBALS_H

// Button settings
#define BTN_DEBOUNCE_TIME   0.08    // Seconds
#define BTN_SAMPLE_FREQ     20      // Hz
#define BTN_THREAD_INTERVAL 50      // Milliseconds (1 / BTN_SAMPLE_FREQ)(1000)

// LCD
#define LCD_W 16
#define LCD_H 2
#define LCD_UPDATE_TIME_MS 250      // How often to refresh the bottom line (mode's output)

// Pins
#define UP_BTN_PIN     8
#define DOWN_BTN_PIN   9
#define MODE_BTN_PIN   10
#define SOLENOID_PIN   11
#define MAP_SENSOR_PIN A0

enum BoostMode { Off, OpenLoopMode, ClosedLoopMode, TotalModeCount};

// MAP Sensor Ranges
// Values based off chart at http://injector-rehab.com/shop/mapsensor.html
#define TWO_BAR_MIN_PSIA   1.3
#define TWO_BAR_MAX_PSIA   30.2
#define THREE_BAR_MIN_PSIA 0.5
#define THREE_BAR_MAX_PSIA 45.7

// Tuning
#define LOG_SPEED_MS                10
#define SENSOR_RATING               3   // 2 or 3 Bar MAP sensor.
#define OPEN_LOOP_INIT_DUTY_CYCLE   195 // 0-255
#define CLOSED_LOOP_INIT_DUTY_CYCLE 180 // 0-255
#define CLOSED_LOOP_THRESHOLD       5   // PSI - Control at what pressure to start the PID control.
#define CLOSED_LOOP_INIT_SETPOINT   13  // PSIG

#endif /* GLOBALS_H */
