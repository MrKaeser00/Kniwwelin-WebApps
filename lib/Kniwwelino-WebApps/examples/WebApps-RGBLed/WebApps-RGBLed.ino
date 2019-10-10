#include <Kniwwelino.h>
#include <Kniwwelino-WebApps.h>

void setup()
{
  Serial.begin(115200);
  Kniwwelino.begin("WebApps-RGBLed", true, true, false); // Wifi=true, Fastboot=true, MQTT Logging=false

  // Print local IP address.
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(Kniwwelino.getIP());

  WebApps.on("/colorcode", HTTP_POST, setRGBLed);

  Kniwwelino.RGBsetColor("000000");

  //Initializes WebApps library.
  WebApps.init();

}

void loop()
{
  WebApps.handle();
  Kniwwelino.loop();
}

void setRGBLed()
{
  String colorString = WebApps.getColorData("colorcode");
  Kniwwelino.RGBsetColor(colorString);
}