#ifndef PINS_H
  #define PINS_H

  #include <Arduino.h>

  #define DEFAULT_PIN_COUNT           20

  extern int relays[DEFAULT_PIN_COUNT];
  extern String relays_info[DEFAULT_PIN_COUNT];
  extern bool relays_default_status[DEFAULT_PIN_COUNT];

  extern int switches[DEFAULT_PIN_COUNT];
  extern String switches_info[DEFAULT_PIN_COUNT];
  extern bool switches_default_status[DEFAULT_PIN_COUNT];

//  #define DEFAULT_SWITCH_STATUS true
//  #define DEFAULT_RELAY_AUTO true
//  #define DEFAULT_RELAY_STATUS true
//  #define DEFAULT_DOOR_STATUS true
//
//  #define SWITCH_COUNT  4
//  #define PINS_COUNT 3
//  #define DOORS_COUNT 1
//
//  extern String pins_db[3] = { {"switches"}, {"relays"}, {"sonsors"} };
//
//  extern int switches[SWITCH_COUNT];
//  extern int relays[PINS_COUNT];
//  extern int doors[DOORS_COUNT];
//
//  extern bool switches_default[SWITCH_COUNT];
//  extern bool relay_default_status[PINS_COUNT];
//
//  extern bool automatically_set[PINS_COUNT];
//
//  extern int relay_interval[PINS_COUNT];
//  extern const int auto_start_humidity; // starts if humidity is above %

#endif
