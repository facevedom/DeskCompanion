#include <WiFi.h>
#include <Wire.h>
#include <HTTPClient.h>
#include "SH1106Wire.h"
#include "ArduinoJson.h"
#include "config.h"
#include <Arduino.h>
#include <OneButton.h>

TaskHandle_t httpLoopTask;

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWD;
const String serverName = SERVER_NAME;

SH1106Wire display(0x3C, SDA, SCL, GEOMETRY_128_64);  // ADDRESS, SDA, SCL

StaticJsonDocument<300> doc;

HTTPClient songRefresherHttpClient;
HTTPClient toggleButtonHttpClient;
HTTPClient nextButtonHttpClient;
HTTPClient previousButtonHttpClient;

OneButton btn = OneButton(
  12,  // Input pin for the button
  true,        // Button is active LOW
  true         // Enable internal pull-up resistor
);

String queuedSpotifyEvent = "none";

// will die
struct Button {
  const uint8_t PIN;
  bool pressed;
  unsigned long lastPressed;
};

struct Song {
  String name;
  String artist;
  bool isPlaying;
  double duration;
  double progress;
};

//Button pauseButton = {12, false, 0};
Song nowPlaying;

//void IRAM_ATTR pauseSong() {
//  pauseButton.pressed = true;
//}

void setup() {
  //pinMode(pauseButton.PIN, INPUT_PULLUP);
  //attachInterrupt(pauseButton.PIN, pauseSong, FALLING);
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

  xTaskCreatePinnedToCore(
    httpLoop,      /* Task function. */
    "httpLoop",    /* name of task. */
    10000,                  /* Stack size of task */
    NULL,                   /* parameter of the task */
    0,                      /* priority of the task */
    &httpLoopTask, /* Task handle to keep track of created task */
    0);                     /* pin task to core 0 */

    // Single Click event attachment
    btn.attachClick([]() {
      queuedSpotifyEvent = "toggle";
    });

    // Double Click event attachment with lambda
    btn.attachDoubleClick([]() {
      queuedSpotifyEvent = "next";
    });

    btn.attachLongPressStart([]() {
      queuedSpotifyEvent = "previous";
    });
}

bool refreshSongState() {
  //Serial.println("Refreshing state");
  int httpCode = songRefresherHttpClient.GET();    //Make the request
  if (httpCode > 0) { //Check for the returning code
    DeserializationError error = deserializeJson(doc, songRefresherHttpClient.getString());
    nowPlaying.name = doc["name"].as<String>();
    nowPlaying.artist = doc["artist"].as<String>();
    nowPlaying.isPlaying = doc["is_playing"].as<bool>();
    nowPlaying.duration = doc["duration"].as<double>();
    nowPlaying.progress = doc["progress"].as<double>();
  } else {
      Serial.println("error getting to Spotify");
  }
}

void runSpotifyEvent() {
  if (queuedSpotifyEvent == "toggle") {
    toggleButtonHttpClient.GET();
    String payload = toggleButtonHttpClient.getString();
    Serial.println(payload);
  } else if (queuedSpotifyEvent == "next") {
    nextButtonHttpClient.GET();
    String payload = nextButtonHttpClient.getString();
    Serial.println(payload);
  } else if(queuedSpotifyEvent == "previous") {
    previousButtonHttpClient.GET();
    String payload = previousButtonHttpClient.getString();
    Serial.println(payload);          
  }
  queuedSpotifyEvent = "none";
}

void httpLoop(void * pvParameters) {
  unsigned long lastRefreshed;
  songRefresherHttpClient.begin(serverName + "playing");
  toggleButtonHttpClient.begin(serverName + "toggle");
  nextButtonHttpClient.begin(serverName + "next");
  previousButtonHttpClient.begin(serverName + "previous");

  while (true) {
    if (WiFi.status() == WL_CONNECTED) {
      if (queuedSpotifyEvent != "none") {
        Serial.println("Event detected");
        runSpotifyEvent();
      }

      if ((millis() - lastRefreshed) > 750) {
        refreshSongState();
        lastRefreshed = millis();
      }
    } else {
      // handle no WiFi connection
    }
  }

  songRefresherHttpClient.end(); //Free the resources
  toggleButtonHttpClient.end();
  nextButtonHttpClient.end();
  previousButtonHttpClient.end();
}



void loop() {  
  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
    btn.tick();

    display.clear();
    if (!nowPlaying.isPlaying) {
      display.fillRect(60, 13, 3, 8);
      display.fillRect(65, 13, 3, 8);
    } else {
      double progressPercentage = (nowPlaying.progress / nowPlaying.duration) * 100;
      display.drawProgressBar(13, 13, 100, 6, uint8_t(progressPercentage));
    }

    display.drawString(0, 0, nowPlaying.name);
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.drawString(127, 20, nowPlaying.artist);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.display();
  }
}