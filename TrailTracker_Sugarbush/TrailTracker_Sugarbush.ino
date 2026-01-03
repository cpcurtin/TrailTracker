
#include "TrailTracker_Sugarbush.h"

void offLED(Adafruit_NeoPixel, int);
void onLED(Adafruit_NeoPixel, int, int, int, int);
void parseTrailData(DynamicJsonDocument, struct Trail*, int, int, const char, const char*);
void parseLiftData(DynamicJsonDocument, struct Trail*, int, int, const char, int);

void setup() {
  
  pinMode(buttonUI, INPUT_PULLUP);

  Serial.begin(115200);
  Serial.println("Good Morning");
  // Setup WIFI connection
  WiFi.mode(WIFI_STA);
  WiFi.begin(Wifi_Username, Wifi_Password);

  // Wait for WIFI connection
  Serial.print("Waiting for WiFi to connect...");
  while ((WiFi.status() != WL_CONNECTED)) {
    Serial.print(".");
  }
  Serial.println(" connected");

  // Init LED strips
  stripLincoln.begin();
  stripLincoln.setPixelColor(0,127,127,127);
  stripLincoln.show();

  stripGadd.begin();
  stripCastlerock.begin();
  stripNorthLynx.begin();
  stripEllen.begin();
  stripInverness.begin();

}

void loop() {
  struct Trail Lincoln[LincolnTrailCount + LincolnLiftCount]; 
  struct Trail Gadd[GaddTrailCount + GaddLiftCount];
  struct Trail Castlerock[CastlerockTrailCount + CastlerockLiftCount];
  struct Trail NorthLynx[NorthLynxTrailCount + NorthLynxLiftCount];
  struct Trail Ellen[EllenTrailCount + EllenTrailCount];
  struct Trail Inverness[InvernessTrailCount + InvernessTrailCount];

  /*for (int i = 0; i < LincolnTrailCount; i++){
    const char* name = &&LincolnTrailNames[i];
    strcpy(Lincoln[i].Name, name);
  }*/
  //Handle HTTP requests & recording status
    HTTPClient https;
    Serial.print("[HTTPS] begin...\n");
    if (https.begin("https://mtnpowder.com/feed/v3.json?bearer_token=NcCvnKYGAOLTfkvAuQm6Z03zvHUSo64ctInVBbhUcr4&resortId%5B%5D=70")){
      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      int httpCode = https.GET();
      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) { 
          const String& payload = https.getString(); // Store the data returned by server
          //Serial.println(payload);
          // Convert data to something parseable
          DynamicJsonDocument doc(101000);
          DeserializationError error = deserializeJson(doc, payload);
          if (error){ // Check for error in deserialization
            Serial.print("Error! Deserialize failed!\n");
            Serial.println(error.f_str());
          }
          else{
            Serial.print("No Error!\n");
            Serial.print("Lincoln Peak\n");

            parseTrailData(doc, Lincoln, LincolnTrailCount, 2, "Trails", LincolnTrailNames);
            parseLiftData(doc, Lincoln, LincolnTrailCount, 2, "Lifts", LincolnLiftCount);
            for (int i = 0; i < (LincolnTrailCount + LincolnLiftCount); i++){
              if (Lincoln[i].Status == 0){ // If Open
                stripLincoln.setPixelColor(i, 0, Brightness, 0);
              }
              else if (Lincoln[i].Status == 1){ // If Not Open
                stripLincoln.setPixelColor(i, Brightness, 0, 0);
              }
              else if (Lincoln[i].Status == 2){ // If Open for Hiking
                stripLincoln.setPixelColor(i, Brightness, 2, 0);
              }
            }
            stripLincoln.show();

            /*Serial.print("Lincoln Lifts\n");
            for (int i = 0; i < LincolnLiftCount; i++){
              Serial.println(doc["Resorts"][0]["MountainAreas"][2]["Lifts"][i]["Name"].as<String>());
              //LincolnLiftStatus[i] = doc["Resorts"][0]["MountainAreas"][2]["Lifts"][i]["Status"].as<const char*>();
              Serial.println(LincolnLiftStatus[i]);
            }*/

            Serial.print("Gadd Peak\n");
            parseTrailData(doc, Gadd, GaddTrailCount, 3, "Trails", GaddTrailNames);
            parseLiftData(doc, Gadd, GaddTrailCount, 3, "Lifts", GaddLiftCount);
            for (int i = 0; i < (GaddTrailCount + GaddLiftCount); i++){
              if (Gadd[i].Status == 0){ // If Open
                stripGadd.setPixelColor(i, 0, Brightness, 0);
              }
              else if (Gadd[i].Status == 1){ // If Not Open
                stripGadd.setPixelColor(i, Brightness, 0, 0);
              }
              else if (Gadd[i].Status == 2){ // If Open for Hiking
                stripGadd.setPixelColor(i, Brightness, 2, 0);
              }
            }
            stripGadd.show();

            /*Serial.print("Gadd Lifts\n");
            for (int i = 0; i < GaddLiftCount; i++){
              Serial.println(doc["Resorts"][0]["MountainAreas"][3]["Lifts"][i]["Name"].as<String>());
              //GaddLiftStatus[i] = doc["Resorts"][0]["MountainAreas"][3]["Lifts"][i]["Status"].as<const char*>();
              Serial.println(GaddLiftStatus[i]);
            }*/

            Serial.print("Castlerock Peak\n");
            parseTrailData(doc, Castlerock, CastlerockTrailCount, 4, "Trails", CastlerockTrailNames);
            parseLiftData(doc, Castlerock, CastlerockTrailCount, 4, "Trails", CastlerockLiftCount);
            for (int i = 0; i < (CastlerockTrailCount + CastlerockLiftCount); i++){
              if (Castlerock[i].Status == 0){ // If Open
                stripCastlerock.setPixelColor(i, 0, Brightness, 0);
              }
              else if (Castlerock[i].Status == 1){ // If Not Open
                stripCastlerock.setPixelColor(i, Brightness, 0, 0);
              }
              else if (Castlerock[i].Status == 2){ // If Open for Hiking
                stripCastlerock.setPixelColor(i, Brightness, 2, 0);
              }
            }
            stripCastlerock.show();

            /*Serial.print("Castlerock Lifts\n");
            for (int i = 0; i < CastlerockLiftCount; i++){
              Serial.println(doc["Resorts"][0]["MountainAreas"][4]["Lifts"][i]["Name"].as<String>());
              //CastlerockLiftStatus[i] = doc["Resorts"][0]["MountainAreas"][4]["Lifts"][i]["Status"].as<const char*>();
              Serial.println(CastlerockLiftStatus[i]);
            }*/

            Serial.print("North Lynx Peak\n");
            parseTrailData(doc, NorthLynx, NorthLynxTrailCount, 5, "Trails", NorthLynxTrailNames);
            parseLiftData(doc, NorthLynx, NorthLynxTrailCount, 5, "Lifts", NorthLynxLiftCount);
            for (int i = 0; i < (NorthLynxTrailCount + NorthLynxLiftCount); i++){
              if (NorthLynx[i].Status == 0){ // If Open
                stripNorthLynx.setPixelColor(i, 0, Brightness, 0);
              }
              else if (NorthLynx[i].Status == 1){ // If Not Open
                stripNorthLynx.setPixelColor(i, Brightness, 0, 0);
              }
              else if (NorthLynx[i].Status == 2){ // If Open for Hiking
                stripNorthLynx.setPixelColor(i, Brightness, 2, 0);
              }
            }
            stripNorthLynx.show();

            /*Serial.print("North Lynx Lifts\n");
            for (int i = 0; i < NorthLynxLiftCount; i++){
              Serial.println(doc["Resorts"][0]["MountainAreas"][5]["Lifts"][i]["Name"].as<String>());
              //NorthLynxLiftStatus[i] = doc["Resorts"][0]["MountainAreas"][5]["Lifts"][i]["Status"].as<const char*>();
              Serial.println(NorthLynxLiftStatus[i]);
            }*/

            Serial.print("Mt. Ellen\n");
            parseTrailData(doc, Ellen, EllenTrailCount, 6, "Trails", EllenTrailNames);
            parseLiftData(doc, Ellen, EllenTrailCount, 6, "Lifts", EllenLiftCount);
            for (int i = 0; i < (EllenTrailCount + EllenLiftCount); i++){
              if (Ellen[i].Status == 0){ // If Open
                stripEllen.setPixelColor(i, 0, Brightness, 0);
              }
              else if (Ellen[i].Status == 1){ // If Not Open
                stripEllen.setPixelColor(i, Brightness, 0, 0);
              }
              else if (Ellen[i].Status == 2){ // If Open for Hiking
                stripEllen.setPixelColor(i, Brightness, 2, 0);
              }
            }
            stripEllen.show();

            /*Serial.print("Ellen Lifts\n");
            for (int i = 0; i < EllenLiftCount; i++){
              Serial.println(doc["Resorts"][0]["MountainAreas"][6]["Lifts"][i]["Name"].as<String>());
              //EllenLiftStatus[i] = doc["Resorts"][0]["MountainAreas"][6]["Lifts"][i]["Status"].as<const char*>();
              Serial.println(EllenLiftStatus[i]);
            }*/

            Serial.print("Inverness Peak\n");
            parseTrailData(doc, Inverness, InvernessTrailCount, 7, "Trails", InvernessTrailNames);
            parseLiftData(doc, Inverness, InvernessTrailCount, 7, "Lifts", InvernessLiftCount);
            for (int i = 0; i < (InvernessTrailCount + InvernessLiftCount); i++){
              if (Inverness[i].Status == 0){ // If Open
                stripInverness.setPixelColor(i, 0, Brightness, 0);
              }
              else if (Inverness[i].Status == 1){ // If Not Open
                stripInverness.setPixelColor(i, Brightness, 0, 0);
              }
              else if (Inverness[i].Status == 2){ // If Open for Hiking
                stripInverness.setPixelColor(i, Brightness, 2, 0);
              }
            }
            stripInverness.show();

            /*Serial.print("Inverness Lifts\n");
            for (int i = 0; i < InvernessLiftCount; i++){
              Serial.println(doc["Resorts"][0]["MountainAreas"][7]["Lifts"][i]["Name"].as<String>());
              //InvernessLiftStatus[i] = doc["Resorts"][0]["MountainAreas"][7]["Lifts"][i]["Status"].as<const char*>();
              Serial.println(InvernessLiftStatus[i]);
            }*/
          }         
        }
      } 
      else { // Catch for website returning something != 200
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str()); 
        Serial.printf("HTTPS Error Code: %d\n", httpCode);
      }
      https.end();
    } 
    else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  // Handle LED updates


  Serial.println();
  Serial.println("Waiting 60mins before the next round...");
  delay(FetchInterval);
}

void offLED(Adafruit_NeoPixel &strip, int LEDcount){
  for (int i = 0; i < LEDcount; i++){
    strip.setPixelColor(i, 0, 0, 0);
  }
  strip.show();
}

void onLED(Adafruit_NeoPixel &strip, int LEDposition, int red, int green, int blue){
  strip.setPixelColor(LEDposition, red, green, blue);
  strip.show();
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
    Serial.println(Peak[i].Status);

    // Difficulty
    Serial.println(doc["Resorts"][0]["MountainAreas"][peakNumber][featureType][i]["Difficulty"].as<String>());
    if (strcmp(doc["Resorts"][0]["MountainAreas"][peakNumber][featureType][i]["Difficulty"].as<const char*>(), "Beginner")){
      Peak[i].Difficulty = 0; // Beginner
    }
    else if (strcmp(doc["Resorts"][0]["MountainAreas"][peakNumber][featureType][i]["Difficulty"].as<const char*>(), "Intermediate")){
      Peak[i].Difficulty = 1; // Intermediate
    }
    else if (strcmp(doc["Resorts"][0]["MountainAreas"][peakNumber][featureType][i]["Difficulty"].as<const char*>(), "Difficult")){
      Peak[i].Difficulty = 2; // Advanced
    }
    else if (strcmp(doc["Resorts"][0]["MountainAreas"][peakNumber][featureType][i]["Difficulty"].as<const char*>(), "Expert")){
      Peak[i].Difficulty = 3; // Expert
    }
    Serial.println(Peak[i].Difficulty);

    // Grooming
    if (strcmp(doc["Resorts"][0]["MountainAreas"][peakNumber][featureType][i]["Grooming"].as<const char*>(), "No")){
      Peak[i].Grooming = 0; // If no grooming
    }
    else if (strcmp(doc["Resorts"][0]["MountainAreas"][peakNumber][featureType][i]["Grooming"].as<const char*>(), "Yes")){
      Peak[i].Grooming = 1; // If grooming
    }
    Serial.println(Peak[i].Grooming);

    // Snowmaking
    if (strcmp(doc["Resorts"][0]["MountainAreas"][peakNumber][featureType][i]["SnowMaking"].as<const char*>(), "No")){
      Peak[i].Snowmaking = 0; // If no snowmaking
    }
    else if (strcmp(doc["Resorts"][0]["MountainAreas"][peakNumber][featureType][i]["SnowMaking"].as<const char*>(), "Yes")){
      Peak[i].Snowmaking = 1; // If snowmaking
    }
    Serial.println(Peak[i].Snowmaking);
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
    Serial.println(Lift[peakCount + i].Status);
  }
}