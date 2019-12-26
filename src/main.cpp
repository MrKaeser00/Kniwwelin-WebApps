#include <Kniwwelino.h>
#include <ArduinoOTA.h>
#include <Kniwwelino-WebApps.h>

const int numberOfElements = 3;
char *siteArray[numberOfElements] = {"led_D6", "led_D0", "rgbled"};

void setup()
{
  Serial.begin(115200);
  ArduinoOTA.begin();                                  //Enables Over-The-Air updates
  Kniwwelino.begin("WebApps-Main", true, true, false); // Wifi=true, Fastboot=true, MQTT Logging=false

  Serial.println(Kniwwelino.getIP());

  //Initializes WebApps library.
  //WebApps.setCredentials("Kniwwelino_9", "EDD1C123");
  WebApps.init(true);   //Failover=true

  pinMode(D0, OUTPUT);
  pinMode(D6, OUTPUT);

  WebApps.contentBuilder(siteArray, numberOfElements);
}

void loop()
{
  WebApps.handleClient();
  ArduinoOTA.handle();
  Kniwwelino.loop();
}