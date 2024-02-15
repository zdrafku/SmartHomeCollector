#if defined(ESP8266)
  #include <ESP8266WiFi.h>  //ESP8266
#endif /* ESP8266 */
#if defined(ESP32)
  #include <WiFi.h> //ESP32
#endif /* ESP32 */

#include "config.h"

void WiFi_setup();
