#include <Kniwwelino.h>
#include <Kniwwelino-WebApps.h>
#include <OneWire.h>
#include <DS18B20.h>

//DS18B20 temperature sensor.
OneWire OneWireD5(D5);
DS18B20 tempSensor(&OneWireD5);

void setup()
{
  Serial.begin(115200);
  Kniwwelino.begin("WebApps-Temperature", true, true, false); // Wifi=true, Fastboot=true, MQTT Logging=false
  tempSensor.begin();

  // Print local IP address.
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(Kniwwelino.getIP());

  //Handle temperature.
  WebApps.on("/temp", checkTemp);

  //Initializes WebApps library.
  WebApps.init(true);
}

void loop()
{
  //Handle clients.
  WebApps.handleClient();
  Kniwwelino.loop();
}

//Wraps the temperature sensor function.
float tempSensorWrapper() {
  tempSensor.requestTemperatures();
  return tempSensor.getTempC();
}

//A function to check the temperature from a DS18B20 sensor.
void checkTemp()
{
  //Sends temperature as a string to the server to be displayed.
  WebApps.sendData("temp", (String)tempSensorWrapper());
}
