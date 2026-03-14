#include <Arduino.h>
#include <stdio.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include "HTTPClientSpecial.h"
#include "WifiCred.h"
#include <FastLED.h>

#define Brightness 15 // Max brightness for NeoPixel is 255
#define LED_TYPE WS2812B // Neopixels are WS2812B
#define COLOR_ORDER GRB
#define FetchInterval 3600000 // Performs HTTP Get once every hour to prevent spamming servers

// Pin Definitions
#define buttonUI 15 // CHANGE THIS LATER
#define Lincoln_Pin 9
#define Gadd_Pin 10
#define Castlerock_Pin 11
#define NorthLynx_Pin 12
#define Ellen_Pin 13
#define Inverness_Pin 14
#define LED_Count 8

// Define functions to toggle NeoPixels
void connectWiFi(void);
void getHTTPS(void);
void offLED(Adafruit_NeoPixel, int);
void onLED(Adafruit_NeoPixel, int, int, int, int);
void handleLEDStatusUpdate(struct Trail*, Adafruit_NeoPixel, int, int, int);
void parseTrailData(DynamicJsonDocument, struct Trail*, int, int, const char, const char*);
void parseLiftData(DynamicJsonDocument, struct Trail*, int, int, const char, int);

struct Trail{
    int Status;
    int Difficulty;
    int Grooming;
    int Snowmaking;
};


int DisplayStatus = 1; // Default display status is 0 for trail/lift status
int DisplayQuery = 1;

/*************************************************************************************************
/ This block outlines the trails on the Sugarbush property, broken out by peak. In their backend, they are split by peak, so this make it easier to parse the JSON. 
/ They seldom change trail names or ratings so there shouldn't be too many issues with this.
*************************************************************************************************/
// Peak order: Lincoln Peak, Gadd Peak, Castlerock Peak, North Lynx Peak, Mt Ellen, Inverness Peak
unsigned int peakNumber[] = {2, 3, 4, 5, 6, 7};
const char* PeakNameOrder[] = {"Lincoln Peak", "Gadd Peak", "Castlerock Peak", "North Lynx Peak", "Mt Ellen", "Inverness Peak"};

// Lincoln Peak
const int LincolnTrailCount = 9;
const char* LincolnTrailNames[LincolnTrailCount] = {"Jester", "Allyn\'s Traverse", "Organgrinder", "Spillsville", "Sigi\'s Ripcord", "Paradise", "Lower Paradise", "Paradise Extension", "Lower Ripcord"};
const int LincolnLiftCount = 1;
const char* LincolnLiftNames[LincolnLiftCount] = {"Heaven\'s Gate Quad"};
int LincolnLiftStatus[LincolnLiftCount];

// Gadd Peak
const int GaddTrailCount= 27;
const char* GaddTrailNames[GaddTrailCount] = {"Domino Chute", "Downspout", "Domino", "Lower Downspout", "Lower Domino", "Heaven\'s Gate Traverse", "Lower Jester", "Lower Birdland", "Lower Organgrinder", "Murphy\'s Glades", "Birdland", "Valley House Traverse", "Stein\'s Run", "The Mall", "Snowball", "Lixi\'s Twist", "Moonshine", "Lower Twist", "Spring Fling", "Lower Snowball", "Spring Road", "Racer\'s Edge", "Cat\'s Meow", "Lower Moonshine", "Coffee Run", "Gondolier", "Header"};
const int GaddLiftCount = 2;
const char* GaddLiftNames[GaddLiftCount] = {"Super Bravo Express Quad", "Valley House Quad"};
int GaddLiftStatus[GaddLiftCount];

// Castlerock Peak
const int CastlerockTrailCount = 11;
const char* CastlerockTrailNames[CastlerockTrailCount] = {"Castlerock Run", "Hi & Lo Road", "Cotillion", "Bailout", "Lower Castlerock Run", "Lower Lift Line", "Runout", "Troll Road", "Rumble", "Middle Earth", "Lift Line"};
const int CastlerockLiftCount = 1;
const char* CastlerockLiftNames[CastlerockLiftCount] = {"Castlerock Double"};
int CastlerockLiftStatus[CastlerockLiftCount];

// North Lynx Peak
const int NorthLynxTrailCount = 24;
const char* NorthLynxTrailNames[NorthLynxTrailCount] = {"Castlerock Connection", "Birch Run", "Morning Star", "Sunrise", "Hot Shot", "Sleeper Chutes", "Sleeper", "Waterfall", "Sleeper Road", "Lower Hot Shot", "First Time", "Sugarbear Forest", "Sugarbear Road", "Easy Rider", "Out Road", "In Road", "Out To Lunch", "Lower Pushover", "Slowpoke", "Overshot", "Pushover", "Second Thoughts", "Pushover Chute", "Village Run"};
const int NorthLynxLiftCount = 6;
const char* NorthLynxLiftNames[NorthLynxLiftCount] = {"North Lynx Triple", "Gate House Express Quad", "Schoolhouse Lift", "Slide Brook Express Quad", "Village Quad", "Welcome Mat"};

// Mt Ellen
const int EllenTrailCount = 36;
const char* EllenTrailNames[EllenTrailCount] = {"Black Diamond", "Panorama", "Rim Run", "F.I.S.", "Upper Lookin\' Good", "Elbow", "Lookin\' Good", "Lower Rim Run", "Spin Out", "South Bound", "Lower Elbow", "Bravo", "Exterminator", "Lower Exterminator", "Way Back", "Lower F.I.S.", "Snowflake", "Graduation", "Riemergasse", "Sugar Run", "Lower Crackerjack", "Straight Shot", "Crackerjack", "Tumbler", "The Cliffs", "Hammerhead", "Encore", "Which Way", "Cruiser", "North Star", "Northway", "Northridge Expressway", "Lower Northstar", "Mainstream", "Easy Does It", "Rob Roy"};
const int EllenLiftCount = 5;
const char* EllenLiftNames[EllenLiftCount] = {"Green Mountain Express Quad", "North Ridge Express Quad", "Summit Quad", "Sunshine Quad", "Easy Up"};

// Inverness Peak
const int InvernessTrailCount = 4;
const char* InvernessTrailNames[InvernessTrailCount] = {"Brambles", "Inverness", "Semi-Tough", "Walt\'s Trail"};
const int InvernessLiftCount = 1;
const char* InvernessLiftNames[InvernessLiftCount] = {"Inverness Quad"};

/*Instantiation of LED "strips" on board. Each peak is defined as its own strip & data is parsed based on these strips & their 
 corresponding trails/lifts. */
Adafruit_NeoPixel stripLincoln(LincolnTrailCount + LincolnLiftCount, Lincoln_Pin, NEO_GRB + NEO_KHZ800); 
//Adafruit_NeoPixel stripGadd(GaddTrailCount + GaddLiftCount, Gadd_Pin, NEO_GRB + NEO_KHZ800); 
//Adafruit_NeoPixel stripCastlerock(CastlerockTrailCount + CastlerockLiftCount, Castlerock_Pin, NEO_GRB + NEO_KHZ800); 
//Adafruit_NeoPixel stripNorthLynx(NorthLynxTrailCount + NorthLynxLiftCount, NorthLynx_Pin, NEO_GRB + NEO_KHZ800); 
//Adafruit_NeoPixel stripEllen(EllenTrailCount + EllenLiftCount, Ellen_Pin, NEO_GRB + NEO_KHZ800); 
//Adafruit_NeoPixel stripInverness(InvernessTrailCount + InvernessLiftCount, Inverness_Pin, NEO_GRB + NEO_KHZ800); 
