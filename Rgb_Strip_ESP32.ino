
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
uint8_t hue = 0;
double lightness = 0.7;
double value = 1;
double saturation = 1;

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

RgbColor HslToRGB(uint8_t hue, double s, double l) {
  uint8_t r, g, b = 0;
  
  double v1, v2;
  double h = hue/360.0;

  v2 = (s + l) - (l * s);
  v1 = 2 * l - v2;

  r = (uint8_t)(255 * HueToRgb(v1, v2, h + (1.0 / 3)));
  g = (uint8_t)(255 * HueToRgb(v1, v2, h));
  b = (uint8_t)(255 * HueToRgb(v1, v2, h - (1.0 / 3)));

  Serial.print(r);
  Serial.print(", ");
  Serial.print(g);
  Serial.print(", ");
  Serial.println(b);

  return RgbColor(r, g, b);
}

double HueToRgb(double v1, double v2, double h) {
  if (h < 0) {
    h += 1;
  }
  if (h > 1) {
    h -= 1;
  }
  if ((6 * h) < 1) {
    return (v1 + (v2 - v1) * 6 * h);
  }
  if ((2 * h) < 1) {
    return (v2);
  }
  if ((3 * h) < 2) {
    return (v1 + (v2 - v1) * ((2.0 / 3) - h) * 6);
  }

  return v1;
}

RgbColor HsvToRgb(uint8_t hue, double sat, double val) {
  double r, g, b = 0;
  double h = hue % 360;
  double hh = h;
  hh /= 60.0;

  long i = (long)hh;
  double ff = hh - i;
  double p = val * (1.0 - sat);
  double q = val * (1.0 - (sat * ff));
  double t = val * (1.0 - (sat * (1.0 - ff)));

  switch (i) {
    case 0:
      r = val;
      g = t;
      b = p;
      break;
    case 1:
      r = q;
      g = val;
      b = p;
      break;
    case 2:
      r = p;
      g = val;
      b = t;
      break;
    case 3:
      r = p;
      g = q;
      b = val;
      break;
    case 4:
      r = t;
      g = p;
      b = val;
      break;
    case 5:
    default:
      r = val;
      g = p;
      b = q;
      break;
  }

  r = r * 255;
  g = g * 255;
  b = q * 255;

  Serial.print(r);
  Serial.print(", ");
  Serial.print(g);
  Serial.print(", ");
  Serial.println(b);

  return RgbColor(r, g, b);
}

void loop()
{
  if (mode == 1) {
    hue = hue + 1;

    RgbColor color = HsvToRgb(hue, saturation, value);

    for (uint16_t pixel = 0; pixel < PixelCount; pixel++)
      {
          strip.SetPixelColor(pixel, color);
      }
    strip.Show();
    delay(10);
  }
}
