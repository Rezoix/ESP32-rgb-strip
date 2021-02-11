
#include <NeoPixelBus.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>


//Led strip stuff
const uint16_t PixelCount = 16;
const uint8_t PixelPin = 26;  

//Modes
//Static: 0
//Color cycle: 1
uint8_t mode = 1; 

NeoPixelBus<NeoBrgFeature, Neo400KbpsMethod> strip(PixelCount, PixelPin);

uint8_t brightness = 255; //0-255
double hue = 0.0;
double lightness = 0.7;
double value = 1.0;
double saturation = 1.0;
double rate = 1.0;

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

    if (request->hasParam("res") && mode == 0) {
      inputMessage = request->getParam("res")->value();
      request->send(200);
      RgbColor color = parseHex(inputMessage);
      for (uint16_t pixel = 0; pixel < PixelCount; pixel++)
      {
          strip.SetPixelColor(pixel, color);
      }
      strip.Show();
    } else if (request->hasParam("mode")) {
      if (request->hasParam("mode")) {
        inputMessage = request->getParam("mode")->value();
        request->send(200);
        mode = inputMessage.toInt();
      }
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

//Hue 0-360, Saturation 0.0-1.0, Value 0.0-1.0
RgbColor HsvToRgb(double hue, double sat, double val) {
  double r, g, b = 0.0;
  double max = val * 255;
  double min = max * (1.0 - sat);

  uint16_t i = ((uint16_t)hue) / 60;
  double mod = fmod(hue, 60.0);

  double adj = (max - min) * mod / 60.0;

  switch (i) {
    case 0:
      r = max;
      g = min + adj;
      b = min;
      break;
    case 1:
      r = max - adj;
      g = max;
      b = min;
      break;
    case 2:
      r = min;
      g = max;
      b = min + adj;
      break;
    case 3:
      r = min;
      g = max - adj;
      b = max;
      break;
    case 4:
      r = min + adj;
      g = min;
      b = max;
      break;
    case 5:
    default:
      r = max;
      g = min;
      b = max - adj;
  }


  return RgbColor(r, g, b);
}

void loop()
{
  if (mode == 1) {
    hue = fmod(hue+rate, 360.0);

    RgbColor color = HsvToRgb(hue, saturation, value);

    for (uint16_t pixel = 0; pixel < PixelCount; pixel++)
      {
          strip.SetPixelColor(pixel, color);
      }
    strip.Show();
    delay(10);
  }
}
