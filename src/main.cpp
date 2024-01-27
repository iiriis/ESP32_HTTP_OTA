#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino.h>
#include <Update.h>
#include "fw_version.h"

// Replace with your network credentials
const char* ssid = ":)";
const char* password = "B003";

// URLs for the firmware .bin and version file hosted on your server
const char* firmware_url = "<server_IP>:<PORT_NO>/path_to_firmware.bin";
const char* firmware_ver_url = "<server_IP>:<PORT_NO>/path_to_fw_version.h";

void OTA_UPDATE();
String check_FW_Version_And_Perform_OTA();

void setup() {
  Serial.begin(115200);
  
  // Connect to Wi-Fi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  // Notify successful Wi-Fi connection and show IP address
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  // Check firmware version and perform OTA if a new version is found
  check_FW_Version_And_Perform_OTA();
}

void loop() {
  // Print the current firmware version; this will run continuously after setup
  Serial.println("Firmware version " FW_VERSION);
  delay(1000);
}

// Function to check the firmware version from the server and initiate OTA update if necessary
String check_FW_Version_And_Perform_OTA()
{
  String serverVersion;
  // Create an HTTP client
  HTTPClient http;
  http.begin(firmware_ver_url); // Specify the URL for firmware version
  int httpCode = http.GET(); // Make the request

  Serial.println("Current Firmware Version: " FW_VERSION);

  if(httpCode == 200) // Check the returning status code
  {
    // Get the response, parse it to get the version number
    serverVersion = http.getString();
    serverVersion = serverVersion.substring(serverVersion.indexOf('"')+1, serverVersion.lastIndexOf('"'));
    
    // Print the firmware version from the server
    Serial.println("Firmware Version in server: "+ serverVersion);

    // If the server version is different from the current version, perform OTA update
    if(!serverVersion.equals(FW_VERSION)){
      Serial.println("Upgrading to Version " FW_VERSION);
      OTA_UPDATE();
    }
  }
  return serverVersion; // Return the server version string
}

// Function to perform the OTA update
void OTA_UPDATE()
{
  HTTPClient http;
  http.begin(firmware_url); // Specify the firmware binary URL
  int httpCode = http.GET(); // Make the request

  // Check the returning status code
  if (httpCode == 200) { // If file is found on server
    // Get the size of the firmware file
    int contentLength = http.getSize();
    // Begin the OTA update process
    bool canBegin = Update.begin(contentLength);
    
    if (canBegin) {
      Serial.println("Begin OTA. This may take 2 - 5 mins to complete. Things might be quiet for a while.. Patience!");

      // Stream the firmware file from the server and store it to flash
      WiFiClient *updateStream = http.getStreamPtr();
      size_t written = Update.writeStream(*updateStream);
      
      // Check if the full file has been written
      if (written == contentLength) {
        Serial.println("Written : " + String(written) + " successfully");
      } else {
        Serial.println("Written only : " + String(written) + "/" + String(contentLength) + ". Retry?");
      }
      
      // End the OTA update process
      if (Update.end()) {
        Serial.println("OTA finished!");
        // If update is completed, restart the ESP32
        if (Update.isFinished()) {
          Serial.println("Update successfully completed. Rebooting.");
          ESP.restart();
        } else {
          Serial.println("Update not finished? Something went wrong!");
        }
      } else {
        // If there was an error, print it
        Serial.println("Error Occurred. Error #: " + String(Update.getError()));
      }
    } else {
      // If there wasn't enough space for OTA update, notify
      Serial.println("Not enough space for OTA Update");
    }
  } else {
    // If the HTTP request failed for some other reason, print the error code
    Serial.println("Error on HTTP request");
  }

  // Close the HTTP connection
  http.end();
}
