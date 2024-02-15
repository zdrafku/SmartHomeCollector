#include "ESP32_WiFi.h"

int WIFI_TIMEOUT = 30; // seconds

void WiFi_setup() {
  const char* ssid = STASSID;
  const char* password = STAPSK;
  int WiFiCurrentTime = millis();
  // Connect to WiFi network
  Serial.print("\nConnecting to ");
  Serial.print(ssid);

  WiFi.mode(WIFI_STA); // OTA / WiFi Upload
  WiFi.begin(ssid, password);

  if (AP_ENABLED) {
    WiFi.softAP(APSSID, APPASS);
  }

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if ( WiFiCurrentTime*1000 > (millis() - WIFI_TIMEOUT*1000) ) {
      Serial.printf("\nFailed to connect to %s\n", ssid);
      return;
    }
  }
  Serial.println("connected");
  // Print the IP address
  delay(1000);
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}
