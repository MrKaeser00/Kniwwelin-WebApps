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

  //What the server does when this URL is called.
  WebApps.on("/colorcode", HTTP_POST, setRGBLed);

  //Turns the LED off a the beginning of the code.
  Kniwwelino.RGBsetColor("000000");

  //Initializes WebApps library.
  WebApps.init();

}

void loop()
{
  //Handles clients.
  WebApps.handle();
  Kniwwelino.loop();
}

//A function which sets the RGB LED according to the data received from site.
void setRGBLed()
{
  //Gets data from server and stores it in a String variable.
  String colorString = WebApps.getColorData("colorcode");
  //Set the LED accordingly.
  Kniwwelino.RGBsetColor(colorString);
}