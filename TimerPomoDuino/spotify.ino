// Spotify-related functions go here

void initializeSpotifyHTTPClients() {
  songRefresherHttpClient.begin(SPOTIFY_REMOTE_SERVER + "playing");
  toggleButtonHttpClient.begin(SPOTIFY_REMOTE_SERVER + "toggle");
  nextButtonHttpClient.begin(SPOTIFY_REMOTE_SERVER + "next");
  previousButtonHttpClient.begin(SPOTIFY_REMOTE_SERVER + "previous");
}

void attachSpotifyEventsButton() {
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

bool refreshSpotifySongState() {
  int httpCode = songRefresherHttpClient.GET();
  if (httpCode > 0) {
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

void tearDownSpotifyHTTPClients() {
  songRefresherHttpClient.end(); //Free the resources
  toggleButtonHttpClient.end();
  nextButtonHttpClient.end();
  previousButtonHttpClient.end();
}
