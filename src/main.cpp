#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <atomic>

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

// Set web server port number to 80
AsyncWebServer server(80);

Adafruit_NeoPixel pixels(1, led, NEO_GRB + NEO_KHZ800);

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
    Lum = 1.0;
    request->send(200);
    Serial.println("Received ON.");
  });

  server.on("/off",HTTP_GET, [](AsyncWebServerRequest *request)
  {
    Lum = 0.0;
    request->send(200);
    Serial.println("Received OFF.");
  });

  server.begin();
  Serial.println("Server Online.");
}

void set_RGB (double pRed, double pGreen, double pBlue, double pLum)
{
  uint8_t R,G,B;
  R= static_cast<uint8_t>(pRed * pLum * 255.0);
  G= static_cast<uint8_t>(pGreen * pLum * 255.0);
  B= static_cast<uint8_t>(pBlue * pLum * 255.0);
  pixels.setPixelColor(0, pixels.Color(R, G, B));
  pixels.show();
}

void loop()
{
double R,G,B;
pixels.clear();
  R=static_cast<double>(random(255))/255.0;
  G=static_cast<double>(random(255))/255.0;
  B=static_cast<double>(random(255))/255.0;
  set_RGB(R,G,B,Lum);
  delay(DelayOn);
  set_RGB(R,G,B,0.0);
  delay(DelayOff);
}
