#include "SmHC.h"
// serialize/deserialize JSON code generator https://arduinojson.org/v6/assistant/

int relays[] = {};
String relays_info[] =  {};
bool relays_default_status[] = {};

int switches[] = {}; //{25, 12, 27, 26};
String switches_info[] = {}; //{"garage", "inner", "r2 27/32", "r3 26/35"};
bool switches_default_status[] = {}; //{DEFAULT_SWITCH_STATUS, false, false, false};

void SmHC_update(String device_data, String endpoint) {
  Serial.println("Sent JSON data: " + endpoint + " - " + device_data);
  
  WiFiClient client;
  if (!client.connect(SMCD_HOST, SMCD_PORT)) {
    Serial.println("Remote connection failed");
  }
  if (client.connected()) {
    client.println("{\"device\": {\"identifier\": \""+ (String)SMCD_IDENTIFIER +"\"}, \""+ endpoint +"\": "+ device_data +"}");

    String server_response;
    while (client.connected()) {
      server_response += client.readStringUntil('\r');
    }

    if ( server_response.length() ) {
      Serial.println("Received Data: "+ server_response);

      // server_response is hash
      StaticJsonDocument<0> filter;
      filter.set(true);
      StaticJsonDocument<2048> doc;
      DeserializationError error = deserializeJson(doc, server_response, DeserializationOption::Filter(filter));

      if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
      }

      int pin_rindex = 0;
      int pin_sindex = 0;
      for (JsonPair item : doc.as<JsonObject>()) {
        const char* item_key = item.key().c_str(); // "13", "32", "33", "55", "04"

        const char* value_pin = item.value()["pin"]; // "13", "32", "33", "55", "04"
        int value_pin_defaults = item.value()["pin_defaults"]; // 1, 0, 0, 2, 1
        const char* value_description = item.value()["description"]; // "inner door", "inner", "s2 32/27", "test ...
        int value_type = item.value()["type"]; // 1 - Relay ; 2 - Switch ; 3 - Sensor

        if ( value_type == 1 ) {
          // Serial.println("Pin type: Relay");
          relays[pin_rindex] = atoi(value_pin);
          relays_info[pin_rindex] = value_description;
          relays_default_status[pin_rindex] = value_pin_defaults;
          pin_rindex++;
        } else if ( value_type == 2 ) {
          // Serial.println("Pin type: Switch");
          switches[pin_sindex] = atoi(value_pin);
          switches_info[pin_sindex] = value_description;
          switches_default_status[pin_sindex] = value_pin_defaults;
          pin_sindex++;
        } else {
          return;
        }
      }

      // server_response is array
//      StaticJsonDocument<512> doc;
//      DeserializationError error = deserializeJson(doc, server_response);
//
//      if (error) {
//        Serial.print("deserializeJson() failed: ");
//        Serial.println(error.c_str());
//        //return;
//      }
//
//      JsonArray root_0 = doc[0];
//      const char* root_0_0 = root_0[0]; // "04"
//      const char* root_0_1 = root_0[1]; // "garage"
//      int root_0_2 = root_0[2]; // 1
//
//      JsonArray root_1 = doc[1];
//      const char* root_1_0 = root_1[0]; // "13"
//      const char* root_1_1 = root_1[1]; // "inner door"
//      int root_1_2 = root_1[2]; // 1
//
//      Serial.println(root_1_1);
    }

    client.stop();  // DISCONNECT FROM THE SERVER
  }
}

int relays_last_state[DEFAULT_PIN_COUNT];
int relays_current_state[DEFAULT_PIN_COUNT];
int switches_last_state[DEFAULT_PIN_COUNT];
int switches_current_state[DEFAULT_PIN_COUNT];

int *pins_arr;
String *pins_info;
bool *pins_default_status;
int *pins_current_state;
int *pins_last_state;

void SmHC_pins_status(String pins_type) {
  if ( pins_type == "relays" ) {
    pins_arr = relays;
    pins_info = relays_info;
    pins_default_status = relays_default_status;
    pins_current_state = relays_current_state;
    pins_last_state = relays_last_state;
  } else if ( pins_type == "switches" ) {
    pins_arr = switches;
    pins_info = switches_info;
    pins_default_status = switches_default_status;
    pins_current_state = switches_current_state;
    pins_last_state = switches_last_state;
  } else {
    return;
  }

  for (byte pin = 0; pin < DEFAULT_PIN_COUNT; pin++) {
    if ( (int)pins_arr[pin] == 0 ) continue;
    pins_current_state[pin] = digitalRead(pins_arr[pin]);
    if ( pins_current_state[pin] != pins_last_state[pin] ) {
      Serial.print("Pin #: "+ pins_info[pin]);
      Serial.println(pins_arr[pin]);
      pins_last_state[pin] = pins_current_state[pin];
      String smhc_data = "{\"pin\": ";
      smhc_data += pins_arr[pin];
      smhc_data += ", \"status\": ";
      smhc_data += pins_current_state[pin];
      smhc_data += "}";
      SmHC_update(smhc_data, "pins");
    }
  }
}

void SmHC_DHT_sensor(int temp, int hum) {
  String smhc_data = "{\"temperature\": ";
  smhc_data += temp;
  smhc_data += ", \"humidity\": ";
  smhc_data += hum;
  smhc_data += "}";
  SmHC_update(smhc_data, "sensor");
}
