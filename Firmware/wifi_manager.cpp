
#include <WiFi.h>
#include <WebServer.h>
#include <ElegantOTA.h>

extern "C" {
  #include "esp_wpa2.h"
}

// #define WIFI_NU
#define WIFI_REGULAR
// #define WIFI_OPEN

#if defined(WIFI_REGULAR)
const char* ssid = "Addaskjis";
const char* password = "ykjbnx@0718askj?";

#elif defined(WIFI_OPEN)
const char* ssid = "REPLACE_WITH_YOUR_SSID";

#elif defined(WIFI_NU)

#endif

WebServer server(80);

void printMacAddress() {
  Serial.println();
  Serial.print("ESP32 MAC: ");
  Serial.println(WiFi.macAddress());
}

void connectToWiFiAndStartOTA() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true);
  delay(100);

#if defined(WIFI_REGULAR)
  Serial.println("Connecting to regular network...");
  WiFi.begin(ssid, password);

#elif defined(WIFI_OPEN)
  Serial.println("Connecting to open network...");
  WiFi.begin(ssid);

#elif defined(WIFI_NU)
  Serial.println("Connecting to WPA2-Enterprise (Device Northwestern)...");
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)eap_anonymous_id, strlen(eap_anonymous_id));
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)eap_id, strlen(eap_id));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)eap_password, strlen(eap_password));
  esp_wifi_sta_wpa2_ent_enable();
  WiFi.begin(ssid);
#endif

  Serial.print("Connecting");
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries++ < 40) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ \t  Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // Setup OTA
    server.on("/", []() {
      server.send(200, "text/plain", "Hello! Head to /update to upload new firmware.");
    });

    ElegantOTA.begin(&server);
    server.begin();

    Serial.println("\n✅ \t OTA Server started.");
    Serial.print("➡ Visit http://");
    Serial.print(WiFi.localIP());
    Serial.println("/update to upload firmware.");
  } else {
    Serial.println("\n❌ \t  Failed to connect.");
  }
}

void handleOTA() {
  server.handleClient();
}
