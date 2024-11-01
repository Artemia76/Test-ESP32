#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>

#define PIN 48
#define NUMPIXELS 1

// Replace with your network credentials
const char* ssid = "My SSID";
const char* password = "My Password";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYON 100
#define DELAYOFF 1000

//uint8_t R,G,B;
bool Red,Green,Blue;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  pixels.begin();
  Serial.begin(115200);
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  Red = false;
  Green = false;
  Blue = false;
}

void set_RGB (bool pRed, bool pGreen, bool pBlue)
{
  uint8_t R,G,B;
  R= pRed?255:0;
  G= pGreen?255:0;
  B= pBlue?255:0;
  pixels.setPixelColor(0, pixels.Color(R, G, B));
  pixels.show();
}

void loop() {
  WiFiClient client = server.available();   // Listen for incoming clients
  pixels.clear();
  /*for(int i=0; i<NUMPIXELS; i++) {
    R=static_cast<uint8_t>(random(10));
    G=static_cast<uint8_t>(random(10));
    B=static_cast<uint8_t>(random(10));
    pixels.setPixelColor(i, pixels.Color(R, G, B));
    pixels.show();
    delay(DELAYON);
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    pixels.show();
    delay(DELAYOFF);
  }*/

    if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /Red/on") >= 0) {
              Serial.println("Red on");
              Red = true;
              set_RGB(Red,Green,Blue);
            } else if (header.indexOf("GET /Red/off") >= 0) {
              Serial.println("Red off");
              Red = false;
              set_RGB(Red,Green,Blue);
            } else if (header.indexOf("GET /Green/on") >= 0) {
              Serial.println("Green on");
              Green = true;
              set_RGB(Red,Green,Blue);
            } else if (header.indexOf("GET /Green/off") >= 0) {
              Serial.println("Green off");
              Green = false;
              set_RGB(Red,Green,Blue);
            } else if (header.indexOf("GET /Blue/on") >= 0) {
              Serial.println("Blue on");
              Blue = true;
              set_RGB(Red,Green,Blue);
            } else if (header.indexOf("GET /Blue/off") >= 0) {
              Serial.println("Blue off");
              Blue = false;
              set_RGB(Red,Green,Blue);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #800000; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");
            
            // Display current state, and ON/OFF buttons for Red Color  
            client.println("<p>Red " + String(Red) + "</p>");
            // If the output26State is off, it displays the ON button       
            if (Red==false) {
              client.println("<p><a href=\"/Red/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/Red/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for Green Color  
            client.println("<p>Green " + String(Green) + "</p>");
            // If the output27State is off, it displays the ON button       
            if (Green==false) {
              client.println("<p><a href=\"/Green/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/Green/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

            // Display current state, and ON/OFF buttons for Green Color  
            client.println("<p>Blue " + String(Blue) + "</p>");
            // If the output27State is off, it displays the ON button       
            if (Blue==false) {
              client.println("<p><a href=\"/Blue/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/Blue/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }

}
