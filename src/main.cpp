#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <atomic>
#include <regex>
#include "env.h"

const int led = 48;
const int capteurLuminosite = 10;

// Replace with your network credentials
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
const int DelayOn = 100;
const int DelayOff = 1000;

// Use atomic to share async mutlithreaded 
std::atomic<double> Lum (1.0);
std::atomic<bool> Switch (true);
std::atomic<double> R (0.0);
std::atomic<double> G (0.0);
std::atomic<double> B (0.0);

// Set web server port number to 80
AsyncWebServer server(80);

Adafruit_NeoPixel pixels(1, led, NEO_GRB + NEO_KHZ800);

void set_RGB (double pRed, double pGreen, double pBlue, double pLum)
{
  uint8_t R,G,B,W;
  R= Switch?static_cast<uint8_t>(pRed * 255.0):0.0;
  G= Switch?static_cast<uint8_t>(pGreen * 255.0):0.0;
  B= Switch?static_cast<uint8_t>(pBlue * 255.0):0.0;
  W = Switch?static_cast<uint8_t>(pLum * 255.0):0.0;
  pixels.setPixelColor(0, pixels.Color(R, G, B, W));
  pixels.show();
}

void get_RGB (const String& pHexRGB)
{
  std::string HexValue = pHexRGB.c_str();
  std::regex pattern("#([0-9a-fA-F]{6})");
  std::smatch match;
  if (std::regex_match(HexValue, match, pattern))
  {
      int r, g, b;
      sscanf(match.str(1).c_str(), "%2x%2x%2x", &r, &g, &b);
      R=static_cast<double>(r/255.0);
      G=static_cast<double>(g/255.0);
      B=static_cast<double>(b/255.0);
      Lum = (R + G + B)/3.0;
      Serial.println(
        "R=" + String(R) +
        " G=" + String(G) +
        " B=" + String(B));
  }
}

void setup()
{
  //--------------------------------------------serial
  Serial.begin(115200);
  while(!Serial){} // Wait until serial monitor is online
  Serial.println("Starting Program");

  //--------------------------------------------GPIO
  pixels.begin();
  pinMode(capteurLuminosite, INPUT);

  //--------------------------------------------SPIFFS
  if(!SPIFFS.begin())
  {
    Serial.println("Erreur SPIFFS...");
    return;
  }

  File root =  SPIFFS.open("/");
  File file  = root.openNextFile();

  while(file)
  {
    Serial.print("file: ");
    Serial.println(file.name());
    file.close();
    file = root.openNextFile();
  }

  //--------------------------------------------WIFI
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.println(ssid);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }

  // Print local IP address and start web server
  Serial.println("\n");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  //--------------------------------------------SERVER
  server.on("/",HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/w3.css",HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/w3.css", "text/css");
  });

  server.on("/script.js",HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/script.js", "text/javascript");
  });

  server.on("/lireLuminosite",HTTP_GET, [](AsyncWebServerRequest *request)
  {
    int val = analogRead(capteurLuminosite);
    String luminosite = String(val);
    request->send(200,"text/plain", luminosite);
    Serial.println("Received lireLuminosite = " +luminosite);
  });

  server.on("/on",HTTP_GET, [](AsyncWebServerRequest *request)
  {
    Switch = true;
    request->send(200);
    Serial.println("Received ON.");
  });

  server.on("/off",HTTP_GET, [](AsyncWebServerRequest *request)
  {
    Switch = false;
    request->send(200);
    Serial.println("Received OFF.");
  });

  server.on("/set",HTTP_POST, [](AsyncWebServerRequest *request)
  {
    int paramsNr = request->params();
    AsyncWebParameter * j = request->getParam(0); // 1st parameter
    Serial.print("rgb: ");
    Serial.print(j->value());                     // value ^
    Serial.println();
    get_RGB(j->value());
    request->send(200);
  });
  server.begin();
  Serial.println("Server Online.");
}

void loop()
{
  pixels.clear();
  set_RGB(R,G,B,Lum);
  delay(DelayOn);
  //set_RGB(R,G,B,0.0);
  //delay(DelayOff);
}
