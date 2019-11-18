
#include <NeoPixelBus.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>


//Led strip stuff
const uint16_t PixelCount = 16;
const uint8_t PixelPin = 26;  

NeoPixelBus<NeoBrgFeature, Neo400KbpsMethod> strip(PixelCount, PixelPin);

int brightness = 255; //0-255

// Server stuff
const char* ssid = "***REMOVED***";
const char* passwd = "***REMOVED***";
AsyncWebServer server(80);

void setup()
{
  Serial.begin(115200);
  while (!Serial);

  if (!SPIFFS.begin(true)) {
    Serial.println("Failed mounting SPIFFS");
    return;
  }

  WiFi.begin(ssid, passwd);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("yolo");

  Serial.println(WiFi.localIP());
 
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });
 
  server.on("/jquery.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/jquery.js", "text/javascript");
  });

  server.on("/iro.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/iro.js", "text/javascript");
  });

  server.on("/get", HTTP_POST, [](AsyncWebServerRequest *request){
    String inputMessage;

    if (request->hasParam("res")) {
      inputMessage = request->getParam("res")->value();
      request->send(200);
      RgbColor color = parseHex(inputMessage);
      for (uint16_t pixel = 0; pixel < PixelCount; pixel++)
      {
          strip.SetPixelColor(pixel, color);
      }
      strip.Show();
    } else {
      request->send(400);
    }
    
  });

  server.begin();
  strip.Begin();
}

RgbColor parseHex(String hex) {
  if (hex.length() == 6) {
    uint8_t red = (uint8_t) strtol(hex.substring(0,2).c_str(), NULL, 16);
    uint8_t green = (uint8_t) strtol(hex.substring(2,4).c_str(), NULL, 16);
    uint8_t blue = (uint8_t) strtol(hex.substring(4,6).c_str(), NULL, 16);
    return RgbColor(red, green, blue);
  }

  return RgbColor(0,0,0);
}

void loop()
{

}


