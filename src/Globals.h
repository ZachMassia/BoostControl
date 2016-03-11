#ifndef GLOBALS_H
#define GLOBALS_H

// Button settings
#define BTN_DEBOUNCE_TIME   0.08    // Seconds
#define BTN_SAMPLE_FREQ     20      // Hz
#define BTN_THREAD_INTERVAL 50      // Milliseconds (1 / BTN_SAMPLE_FREQ)(1000)

// LCD size
#define LCD_W 16
#define LCD_H 2

// Pins
#define UP_BTN_PIN     8
#define DOWN_BTN_PIN   9
#define MODE_BTN_PIN   10
#define OUTPUT_PIN     11
#define MAP_SENSOR_PIN 12

enum BoostMode { Off, OpenLoopMode, ClosedLoopMode, TotalModeCount};

// Tuning
#define OPEN_LOOP_INIT_DUTY_CYCLE 195   // 0-255


#endif /* GLOBALS_H */
