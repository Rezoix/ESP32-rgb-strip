
#include <NeoPixelBus.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <Arduino_JSON.h>

#include "secrets.h"

//Led strip stuff
const uint16_t PixelCount = 16;
const uint8_t PixelPin = 26;

//Modes:
//-off
//-static
//-cycle
String mode = "off";

NeoPixelBus<NeoBrgFeature, Neo400KbpsMethod> strip(PixelCount, PixelPin);

double brightness = 1.0; //0-1.0
String colorHex = "ffffff";
double hue = 0.0;
double value = 1.0;
double saturation = 1.0;
double rate = 1.0;

// Server stuff
const char *ssid = ssidSecret;     //From secrets file
const char *passwd = passwdSecret; //From secrets file
AsyncWebServer server(80);

void setup()
{
  Serial.begin(115200);
  while (!Serial);

  if (!SPIFFS.begin(true))
  {
    Serial.println("Failed mounting SPIFFS");
    return;
  }

  WiFi.begin(ssid, passwd);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println(WiFi.localIP());

  serverSetup();

  strip.Begin();
}


void serverSetup() 
{
  //Static site
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  server.serveStatic("/static/css/", SPIFFS, "/css/");
  server.serveStatic("/static/js/", SPIFFS, "/js/");


  //GET '/get' COMMANDS
  //
  //CurrentValues
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request) {

    Serial.println("Get received");

    if (request->hasParam("currentvalues")) {
      Serial.println("Current Values");
      String message = "{";
      message += "\"mode\": \"" + mode + "\",";
      message += "\"color\": \"#" + colorHex + "\",";
      message += "\"brightness\": " + String(brightness*255) + ",";
      message += "\"rate\": " + String(rate);
      message += "}";


      request->send(200, "application/json", message);
    }
  });

  //POST '/set' COMMANDS
  //
  //Mode, Color, Brightness, Rate
  server.on("/set", HTTP_POST, [](AsyncWebServerRequest *request) {
    String inputMessage;


    if (request->hasParam("mode"))
    {
      inputMessage = request->getParam("mode")->value();
      request->send(200);
      mode = inputMessage;

      if (mode == "off")
      {
        strip.ClearTo(RgbColor(0,0,0));
        strip.Show();
      }
      else if (mode == "static")
      {
        setColor(colorHex);
      }
    }
    else if (request->hasParam("color"))
    {
      inputMessage = request->getParam("color")->value();
      request->send(200);
      if (mode == "static")
      {
        colorHex = inputMessage;
        setColor(inputMessage);
      }
    }
    else if (request->hasParam("brightness"))
    {
      inputMessage = request->getParam("brightness")->value();
      request->send(200);
      brightness = strtod(inputMessage.c_str(), NULL) / 255.0;
      if (mode == "static") 
      {
        updateColor();
      }
    }
    else if (request->hasParam("rate"))
    {
      inputMessage = request->getParam("rate")->value();
      request->send(200);
      rate = strtod(inputMessage.c_str(), NULL);
    }
    else
    {
      request->send(400);
    }
  });

  server.begin();
}

RgbColor parseHex(String hex)
{
  if (hex.length() == 6)
  {
    uint8_t red = (uint8_t)strtol(hex.substring(0, 2).c_str(), NULL, 16);
    uint8_t green = (uint8_t)strtol(hex.substring(2, 4).c_str(), NULL, 16);
    uint8_t blue = (uint8_t)strtol(hex.substring(4, 6).c_str(), NULL, 16);
    return RgbColor(red, green, blue);
  }

  return RgbColor(0, 0, 0);
}

void setColor(RgbColor color)
{
  for (uint16_t pixel = 0; pixel < PixelCount; pixel++)
  {
    strip.SetPixelColor(pixel, color);
  }
  strip.Show();
}

void setColor(HsbColor color)
{
  for (uint16_t pixel = 0; pixel < PixelCount; pixel++)
  {
    strip.SetPixelColor(pixel, color);
  }
  strip.Show();
}

void setColor(String hex)
{
  RgbColor color = parseHex(hex);
  setColor(color);
}

void updateColor()
{
  RgbColor color = parseHex(colorHex);
  setColor(color);
}

void loop()
{
  if (mode == "cycle")
  {
    hue = fmod(hue + rate, 360.0);
    HsbColor color = HsbColor(hue/360, saturation, brightness);
    setColor(color);

    delay(10);
  }
}
