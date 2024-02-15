#ifndef RELAYS_H
  #define RELAYS_H

  #include "Pins.h"
//  #include "Sensor.h"
  #include "Switches.h"

  //#include "Web.h"

  #define PINS_COUNT 3
  #define DOORS_COUNT 1
  #define DEFAULT_RELAY_AUTO true
  #define DEFAULT_RELAY_STATUS true
  #define DEFAULT_DOOR_STATUS true

  extern int relay_interval[PINS_COUNT];
  extern bool automatically_set[PINS_COUNT];
  extern int doors[DOORS_COUNT];
  extern String doors_info[DOORS_COUNT];

//  extern const int auto_start_humidity; // starts if humidity is above %
//  extern int bath_fan; //  position in "relays" variable
//  extern unsigned long relay_previousMillis; // will store last time ralay was reset
  
  void Relays_setup();
  void Relays_loop();
#endif
