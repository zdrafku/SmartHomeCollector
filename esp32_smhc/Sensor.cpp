#include "Sensor.h"

DHT dht(DHTPIN, DHTTYPE);

// current temperature & humidity, updated in loop()
float t = 0.0;
float h = 0.0;

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0; // will store last time DHT was updated
unsigned long relay_previousMillis = 0; // will store last time ralay was reset

// Updates DHT readings every 10 seconds
const long interval = 10;

void Sensor_setup() {
  dht.begin();
}

void Sensor_loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval * 1000) {
    // save the last time you updated the DHT values
    previousMillis = currentMillis;
    // Read temperature as Celsius (the default)
    float newT = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    //float newT = dht.readTemperature(true);
    // if temperature read failed, don't change t value
    if (isnan(newT)) {
      Serial.println("Failed to read from DHT sensor!");
    } else {
      if (newT <= 100) t = newT;
      Serial.println(t);
    }
    // Read Humidity
    float newH = dht.readHumidity();
    // if humidity read failed, don't change h value 
    if (isnan(newH)) {
      Serial.println("Failed to read from DHT sensor!");
    } else {
      if (newH <= 100) h = newH;
      Serial.println(h);
    }
  }
}
