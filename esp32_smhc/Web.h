#ifndef WEB_H
  #define WEB_H

  #if defined(ESP8266)
    #include <ESP8266WiFi.h>  //ESP8266
  #endif /* ESP8266 */
  #if defined(ESP32)
    #include <WiFi.h> //ESP32
  #endif /* ESP32 */
  
  #include "SmHC.h"
//  #include "Sensor.h"
  #include "Pins.h"
  #include "Relays.h"
  #include "Switches.h"
  #include "ESP32_WiFi.h"

  extern String st_relay_num, st_relay_status, st_switch_num, st_switch_status;
  extern int int_relay_num, int_relay_status, int_switch_num, int_switch_status;
  extern String req;

  void Web_setup();
  void Web_loop();
#endif
