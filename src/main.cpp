#include <Kniwwelino.h>
#include <ArduinoOTA.h>
#include <Kniwwelino-WebApps.h>
#include <OneWire.h>
#include <DallasTemperature.h>

//Variable to store the LED state.
bool ledState;

//Variable to store the temperature.
String temp;

OneWire OneWireD5(D5);
DallasTemperature tempSensor(&OneWireD5);
/*
float tempSensor_wrapper() {
  tempSensor.requestTemperatures();
  return tempSensor.getTemp();
}
*/
//A function to toggle the LED on and off.
void changeLedState()
{
  Serial.println("changeLedState called");
  if (ledState)
  {
    WebApps.sendData("state", "Off");
    //Kniwwelino.PINsetEffect(D5, PIN_OFF);
    Kniwwelino.MATRIXwrite(WebApps.bool2string(!ledState));
    ledState = !ledState;
  }
  else
  {
    WebApps.sendData("state", "On");
    Kniwwelino.MATRIXwrite(WebApps.bool2string(!ledState));
    //Kniwwelino.PINsetEffect(D5, PIN_ON);
    ledState = !ledState;
  }
}

//A function to check for the LED state (only used when first (re)loading the page).
void checkLedState()
{
  Kniwwelino.MATRIXwrite(WebApps.bool2string(ledState));
  if (ledState)
  {
    WebApps.sendData("state", "On");
  }
  else
  {
    WebApps.sendData("state", "Off");
  }
}

void checkTemp()
{

  Serial.println(system_get_rtc_time());
  //temp = (String)tempSensor_wrapper() + "°C";
  //WebApps.sendData("temp", temp);
  WebApps.sendData("temp", "1°C");
}

void setup()
{
  Serial.begin(115200);
  SPIFFS.begin();
  ArduinoOTA.begin();                          //Enables Over-The-Air updates
  Kniwwelino.begin("Name", true, true, false); // Wifi=true, Fastboot=true, MQTT Logging=false
  tempSensor.begin();

  // Print local IP address.
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(Kniwwelino.getIP());

  //Handle led state on site
  WebApps.on("/ledstate", changeLedState);
  WebApps.on("/cledstate", checkLedState);
  WebApps.on("/checktemp", checkTemp);

  WebApps.init();
  //pinMode(D5, OUTPUT);
}

void loop()
{
  WebApps.handle();
  ArduinoOTA.handle();
  Kniwwelino.loop();
}