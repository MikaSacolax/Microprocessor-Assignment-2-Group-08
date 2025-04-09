// watchdog_timer.c
#include "hardware/watchdog.h"  // Include the Watchdog Timer library

// Function to initialize and enable the watchdog timer
void setup_watchdog_timer() {
    watchdog_enable(9000, true); // Timeout in ms, true for reset on timeout
}

// Function to feed the watchdog timer to prevent a reset
void feed_watchdog() {
    watchdog_update();  // Feed the watchdog to prevent timeout reset
}
