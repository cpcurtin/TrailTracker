
#include "TrailTracker_Sugarbush.h"

void offLED(Adafruit_NeoPixel, int);
void onLED(Adafruit_NeoPixel, int, int, int, int);

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
          DynamicJsonDocument doc(100000);
          DeserializationError error = deserializeJson(doc, payload);
          if (error){ // Check for error in deserialization
            Serial.print("Error! Deserialize failed!\n");
            Serial.println(error.f_str());
          }
          else{
            Serial.print("No Error!\n");
            Serial.print("Lincoln Peak\n");
            for (int i = 0; i < LincolnTrailCount; i++){ // Lincoln Peak
              Serial.println(doc["Resorts"][0]["MountainAreas"][2]["Trails"][i]["Name"].as<String>());
              LincolnTrailStatus[i] = doc["Resorts"][0]["MountainAreas"][2]["Trails"][i]["Status"].as<const char*>();
              Serial.println(LincolnTrailStatus[i]);
              LincolnTrailRating[i] = doc["Resorts"][0]["MountainAreas"][2]["Trails"][i]["Difficulty"].as<const char*>();
              Serial.println(LincolnTrailRating[i]);
              if ((doc["Resorts"][0]["MountainAreas"][2]["Trails"][i]["Status"].as<String>() == "Open") && i < LED_Count){
                stripLincoln.setPixelColor(i, 0, Brightness, 0);
              }
              else if ((doc["Resorts"][0]["MountainAreas"][2]["Trails"][i]["Status"].as<String>() == "Closed") && i < LED_Count){
                stripLincoln.setPixelColor(i, Brightness, 0, 0);
              }
            }
            stripLincoln.show();
            Serial.print("Lincoln Lifts\n");
            for (int i = 0; i < LincolnLiftCount; i++){
              Serial.println(doc["Resorts"][0]["MountainAreas"][2]["Lifts"][i]["Name"].as<String>());
              LincolnLiftStatus[i] = doc["Resorts"][0]["MountainAreas"][2]["Lifts"][i]["Status"].as<const char*>();
              Serial.println(LincolnLiftStatus[i]);
            }
            Serial.print("Gadd Peak\n");
            for (int i = 0; i < GaddTrailCount; i++){
              Serial.println(doc["Resorts"][0]["MountainAreas"][3]["Trails"][i]["Name"].as<String>());
              Serial.println(doc["Resorts"][0]["MountainAreas"][3]["Trails"][i]["Status"].as<String>());
              GaddTrailStatus[i] = doc["Resorts"][0]["MountainAreas"][3]["Trails"][i]["Status"].as<const char*>();
              Serial.println(GaddTrailStatus[i]);
              GaddTrailRating[i] = doc["Resorts"][0]["MountainAreas"][3]["Trails"][i]["Difficulty"].as<const char*>();
              Serial.println(GaddTrailRating[i]);
            }
            Serial.print("Gadd Lifts\n");
            for (int i = 0; i < GaddLiftCount; i++){
              Serial.println(doc["Resorts"][0]["MountainAreas"][3]["Lifts"][i]["Name"].as<String>());
              GaddLiftStatus[i] = doc["Resorts"][0]["MountainAreas"][3]["Lifts"][i]["Status"].as<const char*>();
              Serial.println(GaddLiftStatus[i]);
            }
            Serial.print("Castlerock Peak\n");
            for (int i = 0; i < CastlerockTrailCount; i++){
              Serial.println(doc["Resorts"][0]["MountainAreas"][4]["Trails"][i]["Name"].as<String>());
              Serial.println(doc["Resorts"][0]["MountainAreas"][4]["Trails"][i]["Status"].as<String>());
              CastlerockTrailStatus[i] = doc["Resorts"][0]["MountainAreas"][4]["Trails"][i]["Status"].as<const char*>();
              Serial.println(CastlerockTrailStatus[i]);
              CastlerockTrailRating[i] = doc["Resorts"][0]["MountainAreas"][4]["Trails"][i]["Difficulty"].as<const char*>();
              Serial.println(CastlerockTrailRating[i]);
            }
            Serial.print("Castlerock Lifts\n");
            for (int i = 0; i < CastlerockLiftCount; i++){
              Serial.println(doc["Resorts"][0]["MountainAreas"][4]["Lifts"][i]["Name"].as<String>());
              CastlerockLiftStatus[i] = doc["Resorts"][0]["MountainAreas"][4]["Lifts"][i]["Status"].as<const char*>();
              Serial.println(CastlerockLiftStatus[i]);
            }
            Serial.print("North Lynx Peak\n");
            for (int i = 0; i < NorthLynxTrailCount; i++){
              Serial.println(doc["Resorts"][0]["MountainAreas"][5]["Trails"][i]["Name"].as<String>());
              Serial.println(doc["Resorts"][0]["MountainAreas"][5]["Trails"][i]["Status"].as<String>());
              NorthLynxTrailStatus[i] = doc["Resorts"][0]["MountainAreas"][5]["Trails"][i]["Status"].as<const char*>();
              Serial.println(NorthLynxTrailStatus[i]);
              NorthLynxTrailRating[i] = doc["Resorts"][0]["MountainAreas"][5]["Trails"][i]["Difficulty"].as<const char*>();
              Serial.println(NorthLynxTrailRating[i]);
            }
            Serial.print("North Lynx Lifts\n");
            for (int i = 0; i < NorthLynxLiftCount; i++){
              Serial.println(doc["Resorts"][0]["MountainAreas"][5]["Lifts"][i]["Name"].as<String>());
              NorthLynxLiftStatus[i] = doc["Resorts"][0]["MountainAreas"][5]["Lifts"][i]["Status"].as<const char*>();
              Serial.println(NorthLynxLiftStatus[i]);
            }
            Serial.print("Mt. Ellen\n");
            for(int i = 0; i < EllenTrailCount; i++){
              Serial.println(doc["Resorts"][0]["MountainAreas"][6]["Trails"][i]["Name"].as<String>());
              Serial.println(doc["Resorts"][0]["MountainAreas"][6]["Trails"][i]["Status"].as<String>());
              EllenTrailStatus[i] = doc["Resorts"][0]["MountainAreas"][6]["Trails"][i]["Status"].as<const char*>();
              Serial.println(EllenTrailStatus[i]);
              EllenTrailRating[i] = doc["Resorts"][0]["MountainAreas"][6]["Trails"][i]["Difficulty"].as<const char*>();
              Serial.println(EllenTrailRating[i]);
            }
            Serial.print("Ellen Lifts\n");
            for (int i = 0; i < EllenLiftCount; i++){
              Serial.println(doc["Resorts"][0]["MountainAreas"][6]["Lifts"][i]["Name"].as<String>());
              EllenLiftStatus[i] = doc["Resorts"][0]["MountainAreas"][6]["Lifts"][i]["Status"].as<const char*>();
              Serial.println(EllenLiftStatus[i]);
            }
            Serial.print("Inverness Peak\n");
            for (int i = 0; i < InvernessTrailCount; i++){
              Serial.println(doc["Resorts"][0]["MountainAreas"][7]["Trails"][i]["Name"].as<String>());
              Serial.println(doc["Resorts"][0]["MountainAreas"][7]["Trails"][i]["Status"].as<String>());
              InvernessTrailStatus[i] = doc["Resorts"][0]["MountainAreas"][7]["Trails"][i]["Status"].as<const char*>();
              Serial.println(InvernessTrailStatus[i]);
              InvernessTrailRating[i] = doc["Resorts"][0]["MountainAreas"][7]["Trails"][i]["Difficulty"].as<const char*>();
              Serial.println(InvernessTrailRating[i]);
            }
            Serial.print("Inverness Lifts\n");
            for (int i = 0; i < InvernessLiftCount; i++){
              Serial.println(doc["Resorts"][0]["MountainAreas"][7]["Lifts"][i]["Name"].as<String>());
              InvernessLiftStatus[i] = doc["Resorts"][0]["MountainAreas"][7]["Lifts"][i]["Status"].as<const char*>();
              Serial.println(InvernessLiftStatus[i]);
            }
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