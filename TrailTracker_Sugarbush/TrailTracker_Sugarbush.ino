

#include "TrailTracker_Sugarbush.h"

unsigned long currentCheck = FetchInterval + 1;
unsigned long lastCheck = 0;

void setup() {
  
  pinMode(buttonUI, INPUT_PULLUP);

  Serial.begin(115200);
  Serial.println(F("Good Morning"));
  /*// Setup WIFI connection
  WiFi.mode(WIFI_STA);
  WiFi.begin(Wifi_Username, Wifi_Password);

  // Wait for WIFI connection
  Serial.print(F("Waiting for WiFi to connect..."));
  while ((WiFi.status() != WL_CONNECTED)) {
    Serial.print(".");
  }
  Serial.println(F("\n connected"));
  Serial.print(F("IP:"));
  Serial.println(WiFi.localIP());*/

  // Init LED strips
  stripLincoln.begin();
  stripLincoln.setPixelColor(0,63,63,63);
  stripLincoln.show();

  //stripGadd.begin();
  //stripCastlerock.begin();
  //stripNorthLynx.begin();
  //stripEllen.begin();
  //stripInverness.begin();

}

void loop() {
  // Instantiate structures for each peak
  struct Trail Lincoln[LincolnTrailCount + LincolnLiftCount]; 
  //struct Trail Gadd[GaddTrailCount + GaddLiftCount];
  //struct Trail Castlerock[CastlerockTrailCount + CastlerockLiftCount];
  //struct Trail NorthLynx[NorthLynxTrailCount + NorthLynxLiftCount];
  //struct Trail Ellen[EllenTrailCount + EllenTrailCount];
  //struct Trail Inverness[InvernessTrailCount + InvernessTrailCount];
  connectWiFi();
  getHTTPS();
  Serial.print(F("No Error!\n"));
  Serial.print(F("Lincoln Peak\n"));
  parseTrailData(doc, Lincoln, LincolnTrailCount, 2, "Trails", LincolnTrailNames);
  parseLiftData(doc, Lincoln, LincolnTrailCount, 2, "Lifts", LincolnLiftCount);
  handleLEDStatusUpdate(Lincoln, stripLincoln, DisplayStatus, LincolnTrailCount, LincolnLiftCount);

  int w = 1;
  while(w > 0){
    //Serial.println("Looping woooo");
    if (digitalRead(buttonUI) == 0){ // If user button is pressed, move the counter over one place until it wraps around
      Serial.println(F("Button Pressed"));
      if (DisplayQuery < 3){
        DisplayQuery++;
      }
      else{
        DisplayQuery = 0;
      }
    }

    if (DisplayQuery - DisplayStatus != 0){
      DisplayStatus = DisplayQuery; // Move counter to next mode
      // Handle LED updates
      Serial.println(F("Updated LEDs"));
      handleLEDStatusUpdate(Lincoln, stripLincoln, DisplayStatus, LincolnTrailCount, LincolnLiftCount);
      Serial.println(F("Done update"));
      //handleLEDStatusUpdate(Gadd, stripGadd, DisplayStatus, GaddTrailCount, GaddLiftCount);
      //handleLEDStatusUpdate(Castlerock, stripCastlerock, DisplayStatus, CastlerockTrailCount, CastlerockLiftCount);
      //handleLEDStatusUpdate(NorthLynx, stripNorthLynx, DisplayStatus, NorthLynxTrailCount, NorthLynxLiftCount);
      //handleLEDStatusUpdate(Ellen, stripEllen, DisplayStatus, EllenTrailCount, EllenLiftCount);
      //handleLEDStatusUpdate(Inverness, stripInverness, DisplayStatus, InvernessTrailCount, InvernessLiftCount);
    }
    //Serial.println();
  }

  //Serial.println("Waiting 60mins before the next round...");
  //delay(FetchInterval);
}

void connectWiFi(void){
  // Setup WIFI connection
  WiFi.mode(WIFI_STA);
  WiFi.begin(Wifi_Username, Wifi_Password);

  // Wait for WIFI connection
  Serial.print(F("Waiting for WiFi to connect..."));
  int time = millis();
  while ((WiFi.status() != WL_CONNECTED)) {
    if((millis() - time) > 10000){ // If it's been more than 10 seconds, try reconnecting
      Serial.println(WiFi.status());
      /*switch (WiFi.status()) {
        case WL_NO_SSID_AVAIL: Serial.println("[WiFi] SSID not found"); break;
        case WL_CONNECT_FAILED:
          Serial.print("[WiFi] Failed - WiFi not connected! Reason: ");
          return;
          break;
        case WL_CONNECTION_LOST: Serial.println("[WiFi] Connection was lost"); break;
        case WL_SCAN_COMPLETED:  Serial.println("[WiFi] Scan is completed"); break;
        case WL_DISCONNECTED:    Serial.println("[WiFi] WiFi is disconnected"); break;
        case WL_CONNECTED:
          Serial.println("[WiFi] WiFi is connected!");
          Serial.print("[WiFi] IP address: ");
          Serial.println(WiFi.localIP());
          return;
          break;
        default:
          Serial.print("[WiFi] WiFi Status: ");
          Serial.println(WiFi.status());
          break;
      }*/
      Serial.println(F("Reconnecting"));
      connectWiFi(); // Recursive call. 
    }
    
    Serial.print(".");
  }
  Serial.println(F("\n connected"));
  Serial.print(F("IP:"));
  Serial.println(WiFi.localIP());
}

void getHTTPS(void){
  HTTPClient https;
  int httpCode = 0;
  while (httpCode != 200){ // Keep in the function until request is granted
    Serial.print(F("[HTTPS] begin...\n"));
    if (https.begin("https://mtnpowder.com/feed/v3.json?bearer_token=NcCvnKYGAOLTfkvAuQm6Z03zvHUSo64ctInVBbhUcr4&resortId%5B%5D=70")){
      Serial.print(F("[HTTPS] GET...\n"));
      // start connection and send HTTP header
      httpCode = https.GET();
      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf(F("[HTTPS] GET... code: %d\n"), httpCode);
        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) { 
          const String& payload = https.getString(); // Store the data returned by server
          //Serial.println(payload);
          // Convert data to something parseable
          StaticJsonDocument<200> parseFilter;
          parseFilter["Resorts"][0]["MountainAreas"] = true;
          Serial.println(payload);
          Serial.println(F("Deserialize"));
          DeserializationError error = deserializeJson(doc, payload, DeserializationOption::Filter(parseFilter));
          if (error){ // Check for error in deserialization
            Serial.print(F("Error! Deserialize failed!\n"));
            Serial.println(error.f_str());
          }       
        }
      } 
      else { // Catch for website returning something != 200
        Serial.printf(F("[HTTPS] GET... failed, error: %s\n"), https.errorToString(httpCode).c_str()); 
        Serial.printf(F("HTTPS Error Code: %d\n"), httpCode);
        delay(10000);
      }
      https.end();
      Serial.println(doc["Resorts"][0]["MountainAreas"][2]["Trails"][0]["Name"].as<String>());
  
    } 
    else {
      Serial.printf(F("[HTTPS] Unable to connect\n"));
    }
  }
}

void offLED(Adafruit_NeoPixel strip, int LEDcount){
  for (int i = 0; i < LEDcount; i++){
    strip.setPixelColor(i, 0, 0, 0);
  }
  strip.show();
}

void onLED(Adafruit_NeoPixel strip, int LEDposition, int red, int green, int blue){
  strip.setPixelColor(LEDposition, red, green, blue);
  strip.show();
}

void handleLEDStatusUpdate(struct Trail* Peak, Adafruit_NeoPixel strip, int status, int trailCount, int liftCount){
  //offLED(strip, (trailCount + liftCount)); // Shut everything off
  switch (status){
    case 0: // Status
      for (int i = 0; i < (trailCount + liftCount); i++){
        Serial.println(F("Update"));
        switch (Peak[i].Status){
          case 0: // Open
            strip.setPixelColor(i, 0, Brightness, 0);
            break;
          case 1: // Closed
            strip.setPixelColor(i, Brightness, 0, 0);
            break;
          case 2: // Open for Hiking
            strip.setPixelColor(i, Brightness, 2, 0);
            break;
        }
      }
      Serial.println(F("Show"));
      strip.show();
      Serial.println(F("Break"));
      break;

    case 1: // Rating
      for (int i = 0; i < (trailCount + liftCount); i++){
        Serial.println(Peak[i].Difficulty);
        switch (Peak[i].Difficulty){
          case 0: // Easy
            strip.setPixelColor(i, 0, Brightness, 0);
            break;
          case 1: // Intermediate
            strip.setPixelColor(i, 0, 0, Brightness);
            break;
          case 2: // Difficult
            strip.setPixelColor(i, Brightness/3, Brightness/3, Brightness/3);
            break;
          case 3: // Expert
            strip.setPixelColor(i, 0, 0, 0);
            break;
          default:
            strip.setPixelColor(i, 0, 0, 0);
            break;
        }
      }
      strip.show();
      break;

    case 2: // Grooming
      for (int i = 0; i < (trailCount + liftCount); i++){
        switch (Peak[i].Grooming){
          case 1:
            strip.setPixelColor(i, 0, Brightness, 0);
            break;
          default:
            strip.setPixelColor(i, 0, 0, 0);
            break;
        }
      }
      strip.show();
      break;
    
    case 3: // Snowmaking
      for (int i = 0; i < (trailCount + liftCount); i++){
        switch (Peak[i].Snowmaking){
          case 1:
            strip.setPixelColor(i, Brightness/3, Brightness/3, Brightness/3);
            break;
          default:
            strip.setPixelColor(i, 0, 0, 0);
            break;
        }
      }
      strip.show();
      break;
  }
}

void parseTrailData(DynamicJsonDocument doc, struct Trail* Peak, int peakCount, int peakNumber, const char* featureType, const char** featureNames){
  for (int i = 0; i < peakCount; i++){ 
    Serial.println(doc["Resorts"][0]["MountainAreas"][peakNumber][featureType][i]["Status"].as<String>());
    // Status
    if (strcmp(doc["Resorts"][0]["MountainAreas"][peakNumber][featureType][i]["Status"].as<const char*>(), "Open") == 0){
      Peak[i].Status = 0; // If trail is open, put 0
    }
    else if (strcmp(doc["Resorts"][0]["MountainAreas"][peakNumber][featureType][i]["Status"].as<const char*>(), "Closed") == 0){
      Peak[i].Status = 1; // If trail is closed, put 1
    }
    else if (strcmp(doc["Resorts"][0]["MountainAreas"][peakNumber][featureType][i]["Status"].as<const char*>(), "Open for Hiking") == 0){
      Peak[i].Status = 2; // If trail is open for hiking, put 2
    }
    //Serial.println(LincolnTrailNames[i]);
    //Serial.println(Peak[i].Status);

    // Difficulty
    Serial.println(doc["Resorts"][0]["MountainAreas"][peakNumber][featureType][i]["Difficulty"].as<String>());
    if (strcmp(doc["Resorts"][0]["MountainAreas"][peakNumber][featureType][i]["Difficulty"].as<const char*>(), "Easy") == 0){
      Peak[i].Difficulty = 0; // Beginner
    }
    else if (strcmp(doc["Resorts"][0]["MountainAreas"][peakNumber][featureType][i]["Difficulty"].as<const char*>(), "Intermediate") == 0){
      Peak[i].Difficulty = 1; // Intermediate
    }
    else if (strcmp(doc["Resorts"][0]["MountainAreas"][peakNumber][featureType][i]["Difficulty"].as<const char*>(), "Difficult") == 0){
      Peak[i].Difficulty = 2; // Advanced
    }
    else if (strcmp(doc["Resorts"][0]["MountainAreas"][peakNumber][featureType][i]["Difficulty"].as<const char*>(), "Expert") == 0){
      Peak[i].Difficulty = 3; // Expert
    }
    //Serial.println(Peak[i].Difficulty);

    // Grooming
    if (strcmp(doc["Resorts"][0]["MountainAreas"][peakNumber][featureType][i]["Grooming"].as<const char*>(), "No") == 0){
      Peak[i].Grooming = 0; // If no grooming
    }
    else if (strcmp(doc["Resorts"][0]["MountainAreas"][peakNumber][featureType][i]["Grooming"].as<const char*>(), "Yes") == 0){
      Peak[i].Grooming = 1; // If grooming
    }
    //Serial.println(Peak[i].Grooming);

    // Snowmaking
    if (strcmp(doc["Resorts"][0]["MountainAreas"][peakNumber][featureType][i]["SnowMaking"].as<const char*>(), "No") == 0){
      Peak[i].Snowmaking = 0; // If no snowmaking
    }
    else if (strcmp(doc["Resorts"][0]["MountainAreas"][peakNumber][featureType][i]["SnowMaking"].as<const char*>(), "Yes") == 0){
      Peak[i].Snowmaking = 1; // If snowmaking
    }
    //Serial.println(Peak[i].Snowmaking);
  }
}

void parseLiftData(DynamicJsonDocument doc, struct Trail* Lift, int peakCount, int peakNumber, const char*featureType, int liftCount){
  for (int i = 0; i < liftCount; i++){
    Serial.println(doc["Resorts"][0]["MountainAreas"][peakNumber][featureType][i]["Status"].as<String>());
    if (strcmp(doc["Resorts"][0]["MountainAreas"][peakNumber][featureType][i]["Status"].as<const char*>(), "Open") == 0){
      Lift[peakCount + i].Status = 0; // If lift open
    }
    else if (strcmp(doc["Resorts"][0]["MountainAreas"][peakNumber][featureType][i]["Status"].as<const char*>(), "Closed") == 0){
      Lift[peakCount + i].Status = 1; // If lift closed
    }
    else {
      Lift[peakCount + i].Status = 1;
    }
    //Serial.println(Lift[peakCount + i].Status);
  }
}