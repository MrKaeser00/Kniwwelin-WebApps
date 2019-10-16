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

  //What the server does when this URL is called.
  WebApps.on("/colorcode", HTTP_POST, setRGBStrip);

  //Turns the strip off at the beginning of the code.
  RGB.setColor(0, 0, 0);

  //Initializes WebApps library.
  WebApps.init();

  //Start the WS2812FX service.
  RGB.init();
  RGB.start();
}

void loop()
{
  //Handle clients.
  WebApps.handleClient();
  Kniwwelino.loop();
  //Handle the WS2812FX service.
  RGB.service();
}

//Sets the RGB LED strip according to the data received from server.
void setRGBStrip()
{
  //Gets color data from server and stores it in a String variable.
  String colorString = WebApps.getColorData("colorcode");
  //Sets the RGB strip accordingly.
  RGB.setColor(Kniwwelino.RGBhex2int(colorString));
}