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

// countdown struct
struct Countdown {
  unsigned long duration;
  unsigned long startTime;
  unsigned int displayableMinutes;
  unsigned int displayableSeconds;
  bool isRunning;
};

// state holders
Song nowPlaying; // will hold the current playing song data already parsed
Countdown activeCountdown;
String queuedSpotifyEvent; // will hold the requested state from Spotify button action
TaskHandle_t httpLoopTask; // secondary loop (for http tasks) task handler
StaticJsonDocument<300> doc; // JSON doc to hold the Spotify song data

int SPOTIFY_SONG_REFRESH_FRECUENCY = 750; // how often will the Spotify song metadata be refreshed

void initializeDisplay() {
  display.init();
  display.flipScreenVertically();
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
}

void setupWifiConnection() {
  display.drawString(0, 0, "Connecting to WiFi: \n" + String(WIFI_SSID));
  display.display();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int iteration = 1;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);

    display.drawString(iteration*16 - 1, 40, "."); // draw a pseudo progress bar
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

void setup() {
  Serial.begin(115200);
  
  initializeDisplay();
  setupWifiConnection();
  startHTTPLoopTask();
  attachSpotifyEventsButton();

  Serial.println("Setup complete");

  startCountdown(15);
}

void httpLoop(void * pvParameters) {
  initializeSpotifyHTTPClients();

  unsigned long lastSongRefresh;
  // HTTP loop
  while (true) {
    if (WiFi.status() == WL_CONNECTED) {
      // check for queued events from the Spotify button
      if (queuedSpotifyEvent != "none") {
        runSpotifyEvent();
      }

      // periodically refresh the Spotify song status
      if ((millis() - lastSongRefresh) > SPOTIFY_SONG_REFRESH_FRECUENCY) {
        refreshSpotifySongState();
        lastSongRefresh = millis();
      }
    } else {
      Serial.println("WiFi connection lost!");
    }
  }

  tearDownSpotifyHTTPClients();
}

double convertMinutesToMillis(int minutes) {
  return(minutes * 60000);
}

void startCountdown(int minutes) {
  activeCountdown.duration = convertMinutesToMillis(minutes);
  activeCountdown.startTime = millis();
  activeCountdown.displayableMinutes = minutes;
  activeCountdown.displayableSeconds = 0;
  activeCountdown.isRunning = true;
}

void displayCurrentPlayingSong() {
  display.setFont(ArialMT_Plain_10);

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
}

bool oddIteration = false;
double lastDisplayedSeconds = 0;
double lastDisplayedMinutes = 0;
void loop() {  
  display.clear();
  spotifyButton.tick();

  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
    displayCurrentPlayingSong();
  }

  if (activeCountdown.isRunning) {
    long elapsedTime = millis() - activeCountdown.startTime;

    if (millis() - lastDisplayedSeconds >= 1000) {
      activeCountdown.displayableSeconds = 59 - (int)((elapsedTime / 1000)%60);
      oddIteration = !oddIteration;
      lastDisplayedSeconds = millis();
    }

    if (millis() - lastDisplayedMinutes >= 60000) {
      activeCountdown.displayableMinutes -= 1;
      lastDisplayedMinutes = millis();
    }

    if (elapsedTime >= activeCountdown.duration) {
      activeCountdown.isRunning = false;
    }
  }

  display.setFont(ArialMT_Plain_16);

  String prettyMinutes;
  String prettySeconds;
  String dots;

  if (oddIteration) {
    dots = ":";
  } else {
    dots = " ";
  }

  if (activeCountdown.displayableMinutes < 10) {
    prettyMinutes = "0" + (String)activeCountdown.displayableMinutes;
  } else {
    prettyMinutes = (String)activeCountdown.displayableMinutes;
  }

  if (activeCountdown.displayableSeconds < 10) {
    prettySeconds = "0" + (String)activeCountdown.displayableSeconds;
  } else {
    prettySeconds = (String)activeCountdown.displayableSeconds;
  }

  String displayableCountdownProgress = prettyMinutes + dots + prettySeconds;
  display.drawString(0, 40, displayableCountdownProgress);

  display.display();
}
