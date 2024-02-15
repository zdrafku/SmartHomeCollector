#include "Switches.h"

// 12, 27, 26, 25
// 33, 32, 35?, 34?

void Switches_setup(){
  // prepare GPIOs
  for (byte pin = 0; pin < DEFAULT_PIN_COUNT; pin++) {
    pinMode(switches[pin], OUTPUT);
    // set to OFF / NO
    digitalWrite(switches[pin], switches_default_status[pin]);
  }
}
