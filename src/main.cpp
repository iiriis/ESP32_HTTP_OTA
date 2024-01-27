
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino.h>
#include <Update.h>
#include "fw_version.h"

const char* ssid = ":)";
const char* password = "B003";

const char* firmware_url = "<server_IP>:<PORT_NO>/path_to_firmware.bin";
const char* firmware_ver_url = "<server_IP>:<PORT_NO>/path_to_fw_version.h";

void OTA_UPDATE();
String check_FW_Version_And_Perform_OTA();


void setup() {
  Serial.begin(115200);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

    Serial.println("Connected to WiFi");
    Serial.println(WiFi.localIP());

    check_FW_Version_And_Perform_OTA();
}

void loop() {
    Serial.println("Firmware version " FW_VERSION);
    delay(1000);
}


String check_FW_Version_And_Perform_OTA()
{
  String s;
    // Perform OTA update
  HTTPClient http;
  http.begin(firmware_ver_url);
  int httpCode = http.GET();

  Serial.println("Current Firmware Version: " FW_VERSION);

  if(httpCode == 200)
  {
    s = http.getString();
    s=s.substring(s.indexOf('"')+1, s.lastIndexOf('"'));
    
    Serial.println("Firmware Version in server: "+ s);

    if(!s.equals(FW_VERSION)){
      Serial.println("Upgrading to Version " FW_VERSION);
      OTA_UPDATE();
    }
  }
  return s;
}

void OTA_UPDATE()
{
    // Perform OTA update
  HTTPClient http;
  http.begin(firmware_url);
  int httpCode = http.GET();
  Serial.println(httpCode);

  if (httpCode == 200) {
    // Get the length of the firmware file
    int contentLength = http.getSize();
    bool canBegin = Update.begin(contentLength);
    
    if (canBegin) {
      Serial.println("Begin OTA. This may take 2 - 5 mins to complete. Things might be quiet for a while.. Patience!");

      // Retrieve the firmware .bin file and write it to flash
      WiFiClient *updateStream = http.getStreamPtr();
      size_t written = Update.writeStream(*updateStream);
      
      if (written == contentLength) {
        Serial.println("Written : " + String(written) + " successfully");
      } else {
        Serial.println("Written only : " + String(written) + "/" + String(contentLength) + ". Retry?");
      }
      
      if (Update.end()) {
        Serial.println("OTA finished!");
        if (Update.isFinished()) {
          Serial.println("Update successfully completed. Rebooting.");
          ESP.restart();
        } else {
          Serial.println("Update not finished? Something went wrong!");
        }
      } else {
        Serial.println("Error Occurred. Error #: " + String(Update.getError()));
      }
    } else {
      Serial.println("Not enough space for OTA Update");
    }
  } else {
    Serial.println("Error on HTTP request");
  }

  http.end();
}

