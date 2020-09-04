#include <WiFi.h>
#include <Wire.h>
#include <HTTPClient.h>
#include "SH1106Wire.h"
#include "ArduinoJson.h"
#include "config.h"

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWD;
const String serverName = SERVER_NAME;

unsigned long lastTime = 0;
unsigned long timerDelay = 1000;

SH1106Wire display(0x3C, SDA, SCL, GEOMETRY_128_64);  // ADDRESS, SDA, SCL

StaticJsonDocument<300> doc;

struct Button {
  const uint8_t PIN;
  bool pressed;
};

Button pauseButton = {12, false};

void IRAM_ATTR pauseSong() {
  pauseButton.pressed = true;
}

void setup() {
  pinMode(pauseButton.PIN, INPUT_PULLUP);
  attachInterrupt(pauseButton.PIN, pauseSong, FALLING);

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  display.init();
  display.flipScreenVertically();
  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0, "Connecting to WiFi: \n" + String(ssid));
  display.display();

  int iteration = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    display.drawString(0 + iteration, 30, ".");
    display.display();
    iteration += 1;
  }

  display.clear();
  display.display();
}

void loop() {
  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
    if (pauseButton.pressed) {
      HTTPClient http2;
      http2.begin(serverName + "/toggle");
      http2.GET();
      http2.end();
      pauseButton.pressed = false;
    }

    HTTPClient http;

    http.begin(serverName + "/playing"); //Specify the URL
    int httpCode = http.GET();                                        //Make the request

    if (httpCode > 0) { //Check for the returning code
      String payload = http.getString();
      //parse json
      DeserializationError error = deserializeJson(doc, payload);
      String name = doc["name"];
      String artist = doc["artist"];
      bool isPlaying = doc["is_playing"];
      double duration = doc["duration"];
      double progress = doc["progress"];
      display.clear();

      if (!isPlaying) {
        display.fillRect(60, 13, 3, 8);
        display.fillRect(65, 13, 3, 8);
      } else {
        double progressPercentage = (progress / duration) * 100;
        display.drawProgressBar(13, 13, 100, 6, uint8_t(progressPercentage));
      }
      display.drawString(0, 0, name);
      display.setTextAlignment(TEXT_ALIGN_RIGHT);
      display.drawString(127, 20, artist);
      display.setTextAlignment(TEXT_ALIGN_LEFT);
    } else {
      // Spotify is not available
      display.clear();
      display.drawString(0, 0, "Error");
    }

    display.display();
    http.end(); //Free the resources
  }

  delay(1000);

}
