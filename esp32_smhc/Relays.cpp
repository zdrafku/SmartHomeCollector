#include "Relays.h"

// GPIOs
// 12, 27, 26, 25
// 33, 32, 35?, 34?
// 33, 32, 13, 04

//int relays[] = {04, 33, 32}; //chip only
//int relays[] = {04, 32, 33};
//String relays_info[] =  {"garage", "inner", "s2 32/27"}; // door - "s3 13/26"
int relay_interval[] = {0, 10, 0}; // 10min
//bool relay_default_status[] = {DEFAULT_RELAY_STATUS, false, false};
bool automatically_set[] = {DEFAULT_RELAY_AUTO, DEFAULT_RELAY_AUTO, DEFAULT_RELAY_AUTO};
int doors[] = {13};
String doors_info[] = {"inner door"};
bool doors_default_status[] = {DEFAULT_DOOR_STATUS};

//const int auto_start_humidity = 42; // starts if humidity is above 40%
//int bath_fan = 2; // 3rd position in "relays" variable

void Relays_setup(){
  // prepare GPIOs
  // relays/switches
  Serial.print("setup relay: ");
  Serial.println(relays[0]);
  Serial.print("Relays size: ");
  Serial.println(sizeof(relays));
  for (byte pin = 0; pin < DEFAULT_PIN_COUNT; pin++) {
    if ( relays[pin] == 0 ) continue;
    pinMode(relays[pin], OUTPUT);
    // set to OFF / NO
    digitalWrite(relays[pin], relays_default_status[pin]);
//    bool automatically_set[pin] = DEFAULT_RELAY_AUTO;
  }
  for (byte pin = 0; pin < (sizeof(doors) / sizeof(doors[0])); pin++) {
    pinMode(doors[pin], OUTPUT);
    // set to OFF / NO
    digitalWrite(doors[pin], relays_default_status[pin]);
  }
}

void Relays_loop(){
  for (byte pin = 0; pin < DEFAULT_PIN_COUNT; pin++) {
    if ( relays[pin] == 0 ) continue;
    if (automatically_set[pin]) {
      if (digitalRead(relays[pin])) {
        digitalWrite(relays[pin], false);
      } else {
        digitalWrite(relays[pin], true);
      }
    }
//    bool automatically_set[pin] = DEFAULT_RELAY_AUTO;
  }
  
//  if (automatically_set[bath_fan] && h >= auto_start_humidity) {
//    // save the last time you updated the relay values
//    relay_previousMillis = millis();
//    digitalWrite(relays[bath_fan], false);
//  } else {
//    if (automatically_set[bath_fan] && millis() - relay_previousMillis >= (relay_interval[bath_fan]*60*1000)) digitalWrite(relays[bath_fan], false);
//  }
}
