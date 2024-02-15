#ifndef SENSOR_H
  #define SENSOR_H

  #define SENSOR_ENABLED  false
  #include <DHT.h>
  
  #define DHTPIN 5     // Digital pin connected to the DHT sensor
  
  // Uncomment the type of sensor in use:
  #define DHTTYPE    DHT11     // DHT 11
  //#define DHTTYPE    DHT22     // DHT 22 (AM2302)
  //#define DHTTYPE    DHT21     // DHT 21 (AM2301)
  
  extern float t;
  extern float h;
//  extern unsigned long previousMillis; // will store last time DHT was updated
//  extern const long interval; // Updates DHT readings every 10 seconds
  
  void Sensor_setup();
  void Sensor_loop();
#endif
