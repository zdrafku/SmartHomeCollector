#include "OTA.h"

// OTA / WiFi Upload
void OTA_Setup() {
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // Hostname defaults to esp32-[MAC]
  ArduinoOTA.setHostname(OTA_HOST);

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");
  ArduinoOTA.setPassword(OTA_PASS);

  ArduinoOTA.onStart([]() {
    Serial.println("OTA Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA End");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("OTA Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("OTA Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("OTA Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("OTA Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("OTA Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("OTA End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("OTA Ready");
}

//WebServer OTA_server(8080);
//
//const char* serverIndex =
//"<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
//"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
//  "<input type='file' name='update'>"
//  "<input type='submit' value='Update'>"
//"</form>"
//"<div id='prg'>progress: 0%</div>"
//"<script>"
//  "$('form').submit(function(e){"
//    "e.preventDefault();"
//    "var form = $('#upload_form')[0];"
//    "var data = new FormData(form);"
//    " $.ajax({"
//      "url: '/update',"
//      "type: 'POST',"
//      "data: data,"
//      "contentType: false,"
//      "processData:false,"
//      "xhr: function() {"
//        "var xhr = new window.XMLHttpRequest();"
//        "xhr.upload.addEventListener('progress', function(evt) {"
//          "if (evt.lengthComputable) {"
//            "var per = evt.loaded / evt.total;"
//            "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
//          "}"
//        "}, false);"
//        "return xhr;"
//      "},"
//      "success:function(d, s) {"
//        "console.log('success!')"
//      "},"
//      "error: function (a, b, c) {"
//      "}"
//    "});"
//  "});"
//"</script>";
//
//void OTA_WebUpdate(){
//  OTA_server.on("/serverIndex", HTTP_GET, []() {
//    OTA_server.sendHeader("Connection", "close");
//    OTA_server.send(200, "text/html", serverIndex);
//  });
//  /*handling uploading firmware file */
//  OTA_server.on("/update", HTTP_POST, []() {
//    OTA_server.sendHeader("Connection", "close");
//    OTA_server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
//    ESP.restart();
//  }, []() {
//    HTTPUpload& upload = OTA_server.upload();
//    if (upload.status == UPLOAD_FILE_START) {
//      Serial.printf("Update: %s\n", upload.filename.c_str());
//      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
//        Update.printError(Serial);
//      }
//    } else if (upload.status == UPLOAD_FILE_WRITE) {
//      /* flashing firmware to ESP*/
//      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
//        Update.printError(Serial);
//      }
//    } else if (upload.status == UPLOAD_FILE_END) {
//      if (Update.end(true)) { //true to set the size to the current progress
//        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
//      } else {
//        Update.printError(Serial);
//      }
//    }
//  });
//}
