#include <Kniwwelino.h>
#include <ArduinoOTA.h>
#include <Kniwwelino-WebApps.h>
/*#include <OneWire.h>
#include <DS18B20.h>
#include <NewPing.h>
#include <WS2812FX.h>

//HC-SR04 ultrasonic distance sensor.
String dist;
NewPing sonic(D6, D6, 200);

//Variable to store the temperature.
String temp;

//NeoPixel RGB strip.
WS2812FX RGB = WS2812FX(5, D7,  NEO_GRB + NEO_KHZ800);

//DS18B20 temperature sensor.
OneWire OneWireD5(D5);
DS18B20 tempSensor(&OneWireD5);

float tempSensorWrapper() {
  tempSensor.requestTemperatures();
  return tempSensor.getTempC();
}
*/
const int numberOfElements = 1;
char *siteArray[numberOfElements] = {"led_D6"};

//maybe define name by name_pin or something similar
/*
//A function to check the temperature from a DS18B20 sensor.
void checkTemp()
{
  Serial.println("temp called");
  WebApps.sendData("temp", (String)tempSensorWrapper());
}

//A function to check the distance in fornt of an HC_SR04P ultrasonic sensor.
void checkSonic()
{
  Serial.println("sonic called");
  dist = sonic.ping_cm();
  dist += "cm";
  WebApps.sendData("sonic", dist);
  //Serial.println(dist);
}

*/
void setup()
{
  Serial.begin(115200);
  ArduinoOTA.begin();                          //Enables Over-The-Air updates
  Kniwwelino.begin("WebApps-Main", true, true, false); // Wifi=true, Fastboot=true, MQTT Logging=false
  //tempSensor.begin();

  // Print local IP address.
  //Serial.println("");
  //Serial.println("WiFi connected.");
  //Serial.println("IP address: ");
  Serial.println(Kniwwelino.getIP());

  //Handle temperature.
  //WebApps.on("/temp", checkTemp);

  //Handle ultrasonic sensor
  //WebApps.on("/sonic", checkSonic);

  //Initializes WebApps library.
  //WebApps.setCredentials("Kniwwelino_9", "EDD1C");
  WebApps.init(true);
  //pinMode(D5, OUTPUT);

  //RGB.init();
  //RGB.start();

  WebApps.contentBuilder(siteArray, numberOfElements);
}

void loop()
{
  WebApps.handleClient();
  ArduinoOTA.handle();
  Kniwwelino.loop();
  //RGB.service();
}