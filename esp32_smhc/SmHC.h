#ifndef SMHC_H
  #define SMHC_H

  #include "ArduinoJson.h"
  #include "ESP32_WiFi.h"
  #include "OTA.h"
  #include "Pins.h"

  #define SMCD_IDENTIFIER   WiFi.macAddress()
  #define SMCD_HOST         "local.smhc.cf"
  #define SMCD_PORT         8123

  void SmHC_update(String, String);
  void SmHC_pins_status(String);
  void SmHC_DHT_sensor(int, int);
#endif
