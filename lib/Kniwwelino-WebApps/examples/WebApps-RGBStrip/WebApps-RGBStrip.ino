#include <Kniwwelino.h>
#include <Kniwwelino-WebApps.h>
#include <WS2812FX.h>

//Set your pin here.
uint8_t pin = D7;

//NeoPixel RGB strip.
WS2812FX RGB = WS2812FX(5, pin,  NEO_GRB + NEO_KHZ800);

void setup()
{
  Serial.begin(115200);
  Kniwwelino.begin("WebApps-RGBStrip", true, true, false); // Wifi=true, Fastboot=true, MQTT Logging=false

  // Print local IP address.
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(Kniwwelino.getIP());

  WebApps.on("/colorcode", HTTP_POST, setRGBLed);

  RGB.setColor(0, 0, 0);

  //Initializes WebApps library.
  WebApps.init();

  RGB.init();
  RGB.start();
}

void loop()
{
  WebApps.handle();
  Kniwwelino.loop();
  RGB.service();
}

void setRGBLed()
{
  String colorString = WebApps.getColorData("colorcode");
  RGB.setColor(Kniwwelino.RGBhex2int(colorString));
}