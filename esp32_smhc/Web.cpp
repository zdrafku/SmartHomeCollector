#include "Web.h"

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

void Web_setup() {
  // Start the server
  server.begin();
}

void Web_loop(){
  unsigned long relay_previousMillis;
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) return;

  // Wait until the client sends some data
  //Serial.println("new client");
  unsigned long timeout = millis() + 3000;
  while (!client.available() && millis() < timeout) {
    //delay(1);
    return;
  }
  if (millis() > timeout) {
    Serial.println("timeout");
    client.flush();
    client.stop();
    return;
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  //client.flush();

  if (req.indexOf(" / ") == 3 || req.indexOf(" /relay/") == 3 || req.indexOf(" /auto/") == 3 || req.indexOf(" /switch/") == 3 || req.indexOf(" /display ") == 3 || req.indexOf(" /door/") == 3) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println();
    client.println("<!DOCTYPE HTML>\n<html>\n\t<head>");
    client.println("\t\t<title>Garage</title>\n"
      "\t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
      "\t\t<link rel=\"stylesheet\" href=\"https://use.fontawesome.com/releases/v5.7.2/css/all.css\"\n"
      "\t\t\tintegrity=\"sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr\" crossorigin=\"anonymous\">\n"
      "\t\t<style>\n"
      "\t\t\thtml {\n"
      "\t\t\t\tfont-family: Arial;\n"
      "\t\t\t\tdisplay: inline-block;\n"
      "\t\t\t\tmargin: 0px auto;\n"
      "\t\t\t\ttext-align: center;\n"
      "\t\t\t}\n"
      "\t\t\th2 { font-size: 3.0rem; }\n"
      "\t\t\tp { font-size: 3.0rem; }\n"
      "\t\t\t.units { font-size: 1.0rem; }\n"
      "\t\t\t.dht-labels {\n"
      "\t\t\t\tfont-size: 1.5rem;\n"
      "\t\t\t\tvertical-align: middle;\n"
      "\t\t\t\tpadding-bottom: 15px;\n"
      "\t\t\t}\n"
      "\t\t</style>");
    client.print("\t\t<meta http-equiv=\"refresh\" content=\"10; url = /");
    client.println("\">\n\t</head>\n\t<body>");
    client.println("\t\t<table align=\"center\">");
//./WriteToCard.cpp:      sprintf(GPS_Lat, "%02u%5u", (int)nmea.location.lat(), (int)(nmea.location.lat()*100000)%100000);
// Uptime:   1d 00:16:11 (87371000)
    client.printf("\t\t\t<tr><td align=\"right\">Uptime: </td><td align=\"left\" colspan=3>%dd %02u:%02u:%02u</td></tr>\n",
                  (int)(millis()/1000/60/60/24), // days
                  (int)(millis()/1000/60/60)%24,    // hours
                  (int)(millis()/1000/60)%60,       // minutes
                  (int)(millis()/1000)%60,          // seconds
                  millis()/1000);                   // uptime in seconds
    client.printf("\t\t\t<tr><td align=\"right\">SSID: </td><td align=\"left\" colspan=3>%s</td></tr>\n", STASSID);
    if (AP_ENABLED) client.printf("\t\t\t<tr><td align=\"right\">AP SSID: </td><td align=\"left\" colspan=3>%s</td></tr>\n", APSSID);
    client.println("\t\t\t<tr><td colspan=4>&nbsp;</td></tr>\n");
//    if (SENSOR_ENABLED) {
//      client.print("\t\t\t<tr>\n\t\t\t\t<td align=\"left\">\n"
//        "\t\t\t\t\t<i class=\"fas fa-thermometer-half\" style=\"color:#059e8a;\"></i>\n"
//        "\t\t\t\t\t<span class=\"dht-labels\">Temperature</span>\n\t\t\t\t</td>\n"
//        "\t\t\t\t<td colspan=3 align=\"left\">\n\t\t\t\t\t<span id=\"temperature\">");
//      client.print(t);
//      client.print("</span>\n"
//        "\t\t\t\t\t<sup class=\"units\">&deg;C</sup>\n"
//        "\t\t\t\t</td>\n\t\t\t</tr>\n"
//        "\t\t\t<tr>\n\t\t\t\t<td align=\"left\">\n"
//        "\t\t\t\t\t<i class=\"fas fa-tint\" style=\"color:#00add6;\"></i>\n"
//        "\t\t\t\t\t<span class=\"dht-labels\">Humidity</span>\n\t\t\t\t</td>\n"
//        "\t\t\t\t<td colspan=3 align=\"left\">\n\t\t\t\t\t<span id=\"humidity\">");
//      client.print((int)h);
//      client.printf("</span>\n"
//        "\t\t\t\t\t<sup class=\"units\">%%<br>%d%% for %dmin</sup>\n"
//        "\t\t\t\t</td>\n\t\t\t</tr>\n", auto_start_humidity, relay_interval[bath_fan]);
//      client.println("\t\t\t<tr><td colspan=4>&nbsp;</td></tr>");
//    }
    client.println("\t\t\t<tr>\n\t\t\t\t<td>&nbsp;</td>\n"
      "\t\t\t\t<td width=\"40px\" align=\"left\">R</td>\n"
      "\t\t\t\t<td width=\"40px\" align=\"left\">SW</td>\n"
      "\t\t\t\t<td width=\"40px\">Auto</td>\n\t\t\t</tr>");

    // Match the request
    String st_relay_num, st_relay_status, st_switch_num, st_switch_status;
    int int_relay_num, int_relay_status, int_switch_num, int_switch_status;

    if ( req.indexOf(" /relay/") == 3 ) {
      st_relay_num = req[11];
      st_relay_status = req[13];
    }
    if ( req.indexOf(" /auto/") == 3 ) {
      st_relay_num = req[10];
      st_relay_status = req[12];
    }
    if ( req.indexOf(" /switch/") == 3 ) {
      st_switch_num = req[12];
      st_switch_status = req[14];
    }

    int_relay_num = st_relay_num.toInt();
    int_relay_status = st_relay_status.toInt();
    int_switch_num = st_switch_num.toInt();
    int_switch_status = st_switch_status.toInt();

    // relays/switches
    for (byte pin = 0; pin < DEFAULT_PIN_COUNT; pin++) {
      if ( relays[pin] == 0 ) continue;
      //if ( pin == 3 ) continue; // garage door
      if ( req.indexOf(" /relay/" + pin) == 3 ) { 
        digitalWrite(relays[int_relay_num], int_relay_status);
        automatically_set[int_relay_num] = false;
      }
      if ( req.indexOf(" /auto/" + pin) == 3 ) { 
        relay_previousMillis = millis();
        automatically_set[int_relay_num] = int_relay_status;
      }
      if ( req.indexOf(" /switch/" + pin) == 3 ) { 
        digitalWrite(switches[int_switch_num], int_switch_status);
        //automatically_set[int_switch_num] = false;
      }
      client.printf("\t\t\t<tr>\n\t\t\t\t<td align=\"left\"><span class=\"dht-labels\">Relay %d (%s)</span></td>", pin + 1, relays_info[pin]);

      // relay status
      if ( digitalRead(relays[pin]) ) {
        client.printf("\t\t\t\t<td><a href=\"/relay/%d/0\"><i class=\"fas fa-lightbulb\" style=\"color:#E4E4E4;\"></i></a> %d</td>\n", pin, relays[pin]);
      } else {
        client.printf("\t\t\t\t<td><a href=\"/relay/%d/1\"><i class=\"fas fa-lightbulb\" style=\"color:#F5F33F;\"></i></a> %d</td>\n", pin, relays[pin]);
      }
      // switch status
      if ( digitalRead(switches[pin]) ) {
        client.printf("\t\t\t\t<td><a href=\"/switch/%d/0\"><i class=\"fas fa-mobile-alt\" style=\"color:#E4E4E4;\"></i></a> %d</td>\n", pin, switches[pin]);
        //client.printf("\t\t\t\t<td><i class=\"fas fa-mobile-alt\" style=\"color:#E4E4E4;\"></i> %d</td>\n", switches[pin]);
      } else {
        client.printf("\t\t\t\t<td><a href=\"/switch/%d/1\"><i class=\"fas fa-mobile\" style=\"color:#F5F33F;\"></i></a> %d</td>\n", pin, switches[pin]);
        //client.printf("\t\t\t\t<td><i class=\"fas fa-mobile\" style=\"color:#F5F33F;\"></i> %d</td>\n", switches[pin]);
      }
      // set automatically
      if ( automatically_set[pin] ) {
        client.printf("\t\t\t\t<td><a href=\"/auto/%d/0\"><i class=\"fas fa-toggle-on\" style=\"color:#00add6;\"></i></a> %d</td>\n", pin, automatically_set[pin]);
      } else {
        client.printf("\t\t\t\t<td><a href=\"/auto/%d/1\"><i class=\"fas fa-toggle-off\" style=\"color:#F78484;\"></i></a> %d</td>\n", pin, automatically_set[pin]);
      }
      client.println("\t\t\t</tr>");
    }
    // door control
    String st_door_cycle_num, st_door_stop_num;
    int int_door_cycle_num, int_door_stop_num;

    if ( req.indexOf(" /door/cycle/") == 3 ) {
      st_door_cycle_num = req[17];
    }
    if ( req.indexOf(" /door/stop/") == 3 ) {
      st_door_stop_num = req[16];
    }

    int_door_cycle_num = st_door_cycle_num.toInt();
    int_door_stop_num = st_door_stop_num.toInt();

    for (byte pin = 0; pin < (sizeof(doors) / sizeof(doors[0])); pin++) {
      if ( req.indexOf(" /door/cycle/" + pin) == 3 ) {
        // up/stop/revers
        digitalWrite(doors[int_door_cycle_num], false);
        delay(500);
        digitalWrite(doors[int_door_cycle_num], true);
      }
//      if ( req.indexOf(" /door/stop/" + pin) == 3 ) {
//        digitalWrite(doors[int_door_cycle_num], false);
//      }

//      if ( req.indexOf(" /door/cycle/3") == 3 ) {
//        // up/stop/revers
//        digitalWrite(doors[pin], false);
//        delay(500);
//        digitalWrite(doors[pin], true);
//      }
      client.printf("\t\t\t<tr>\n\t\t\t\t<td align=\"left\"><span class=\"dht-labels\">Door %d (%s)</span></td>\n", pin + 1, doors_info[pin]);
      client.printf("\t\t\t\t<td colspan=\"2\" align=\"center\"><a href=\"/door/cycle/%d\"><i class=\"fas fa-sync-alt\" style=\"color:#F78484;\"></i></a></td>\n", pin);
      client.printf("\t\t\t\t<td><a href=\"/door/stop/%d\"><i class=\"fas fa-sync-alt\" style=\"color:#E4E4E4;\"></i></a></td>\n\t\t\t</tr>\n", pin);
    }
    client.println("\t\t</table>");
//    client.println("\t\t<p><a href=\"/#restart\">"
//                    "<i class=\"fas fa-meh-rolling-eyes\" style=\"color:#EDEC78;\"></i>"
//                    "<i class=\"fas fa-screwdriver\" style=\"color:gray;\"></i>"
//                    "<i class=\"fas fa-heartbeat\" style=\"color:red;\"></i>"
//                    "<i class=\"fas fa-sync\" style=\"color:green;\"></i>"
//                    "</a></p>");
  } else if (req.indexOf(" /restart ") == 3) {
    ESP.restart();
  } else {
    Serial.println("invalid request");
    client.println("HTTP/1.1 404 Not Found");
    client.println("Content-Type: text/html");
    client.println();
    client.println("<!DOCTYPE HTML>\r\n<html><body>Not found</body></html>");
    return;
  }

  client.print("\t</body>\n</html>");
  // Send the response to the client
  //client.flush();
  delay(1);
}
