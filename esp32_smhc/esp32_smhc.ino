#include "ArduinoJson.h"
#include "ESP32_WiFi.h"
#include "OTA.h"
#include "Pins.h"
#include "SmHC.h"
#include "Sensor.h"
#include "Relays.h"
#include "Web.h"

void setup() {
  Serial.begin(115200);

  if (SENSOR_ENABLED) Sensor_setup();

  WiFi_setup();
  OTA_Setup();

  // Smart Home Collector
  Serial.print("Reporting to: ");
  Serial.println(SMCD_HOST);
  // Register device
  SmHC_update("{\"identifier\": \""+ (String)SMCD_IDENTIFIER +"\", \"description\": \""+ (String)OTA_HOST +"\"}", "register");
  SmHC_update("1","get_pins");

  Web_setup();
  Relays_setup();
  Switches_setup();
}

void loop() {
  ArduinoOTA.handle();
  if ( WiFi.status() != WL_CONNECTED || WiFi.status() == WL_NO_SSID_AVAIL) {
    WiFi_setup();
    delay(5000);
  }
  if (SENSOR_ENABLED) Sensor_loop();
  Relays_loop(); // TODO: toggles pins' status unnecessary
  Web_loop();
  SmHC_pins_status("relays");
//  SmHC_pins_status("doors");
  SmHC_pins_status("switches");
}
