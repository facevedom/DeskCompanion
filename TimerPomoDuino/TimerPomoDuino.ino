#include <Arduino.h>
#include <ArduinoJson.h>
#include "config.h"
#include <HTTPClient.h>
#include <OneButton.h>
#include <SH1106Wire.h>
#include <WiFi.h>
#include <Wire.h>

// hardware constants
const char DISPLAY_ADDRESS = 0x3C;
const int SPOTIFY_BUTTON_PIN = 12;

// WiFi and server configuration is read from "config.h"
const char* WIFI_SSID = CONF_WIFI_SSID;
const char* WIFI_PASSWORD = CONF_WIFI_PASSWORD;
const String SPOTIFY_REMOTE_SERVER = CONF_SPOTIFY_REMOTE_SERVER_NAME;

// display initialization
SH1106Wire display(DISPLAY_ADDRESS, SDA, SCL, GEOMETRY_128_64);

// HTTP clients initialization
HTTPClient songRefresherHttpClient;
HTTPClient toggleButtonHttpClient;
HTTPClient nextButtonHttpClient;
HTTPClient previousButtonHttpClient;

// Spotify control button declaration
OneButton spotifyButton = OneButton(
  SPOTIFY_BUTTON_PIN,  // input pin for the button
  true,                // button is active LOW
  true                 // enable internal pull-up resistor
);

// song struct
struct Song {
  String name;
  String artist;
  bool isPlaying;
  double duration;
  double progress;
};

// state holders
StaticJsonDocument<300> doc; // JSON doc to hold the Spotify song data
String queuedSpotifyEvent; // will hold the requested state from Spotify button action
Song nowPlaying; // will hold the current playing song data already parsed
TaskHandle_t httpLoopTask; // secondary loop (for http tasks) task handler

void initializeDisplay() {
  display.init();
  display.flipScreenVertically();
  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
}

void setupWifiConnection() {
  display.drawString(0, 0, "Connecting to WiFi: \n" + String(WIFI_SSID));
  display.display();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int iteration = 1;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);

    display.drawString(iteration*16 - 1, 30, "."); // draw a pseudo progress bar
    display.display();

    /* 
      if 3.5 seconds have passed without being able to connect to wifi, reboot the board.
      every other reboot, the WiFi will get stuck establishing a connection, this is a workaround
      TODO: what about offline mode?
    */ 
    if (iteration == 7) {
      ESP.restart();
    }
    iteration += 1;
  }

  display.clear();
  display.display();  
}

void startHTTPLoopTask() {
  xTaskCreatePinnedToCore(
    httpLoop,      // task function. */
    "httpLoop",    // name of task. */
    10000,         // stack size of task */
    NULL,                   /* parameter of the task */
    0,                      /* priority of the task */
    &httpLoopTask, /* Task handle to keep track of created task */
    0               /* pin task to core 0 */
  );
}

void attachSpotifyEventsButtonEvents() {
  queuedSpotifyEvent = "none";

  // single click to play/pause
  spotifyButton.attachClick([]() {
    queuedSpotifyEvent = "toggle";
  });

  // double click to skip song
  spotifyButton.attachDoubleClick([]() {
    queuedSpotifyEvent = "next";
  });

  // long press to play previous song
  spotifyButton.attachLongPressStart([]() {
    queuedSpotifyEvent = "previous";
  });
}

void setup() {
  Serial.begin(115200);
  
  initializeDisplay();
  setupWifiConnection();
  startHTTPLoopTask();
  attachSpotifyEventsButtonEvents();
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
  songRefresherHttpClient.begin(SPOTIFY_REMOTE_SERVER + "playing");
  toggleButtonHttpClient.begin(SPOTIFY_REMOTE_SERVER + "toggle");
  nextButtonHttpClient.begin(SPOTIFY_REMOTE_SERVER + "next");
  previousButtonHttpClient.begin(SPOTIFY_REMOTE_SERVER + "previous");

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
    spotifyButton.tick();

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