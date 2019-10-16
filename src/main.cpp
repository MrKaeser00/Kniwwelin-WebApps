#include <Kniwwelino.h>
#include <ArduinoOTA.h>
#include <Kniwwelino-WebApps.h>
#include <OneWire.h>
#include <DS18B20.h>
#include <NewPing.h>
#include <WS2812FX.h>

//Variable to store the LED state.
bool ledState;

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

void setRGBLed()
{
  String colorString = WebApps.getColorData("colorcode");
  RGB.setColor(Kniwwelino.RGBhex2int(colorString));
  Kniwwelino.RGBsetColor(colorString);
}

void setup()
{
  Serial.begin(115200);
  ArduinoOTA.begin();                          //Enables Over-The-Air updates
  Kniwwelino.begin("WebApps-Main", true, true, false); // Wifi=true, Fastboot=true, MQTT Logging=false
  tempSensor.begin();

  // Print local IP address.
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(Kniwwelino.getIP());

  //Handle led state on site
  WebApps.on("/ledstate", changeLedState);
  WebApps.on("/cledstate", checkLedState);

  //Handle temperature.
  WebApps.on("/temp", checkTemp);

  //Handle ultrasonic sensor
  WebApps.on("/sonic", checkSonic);

  WebApps.on("/colorcode", HTTP_POST, setRGBLed);

  //Kniwwelino.RGBsetColor("000000");

  //Initializes WebApps library.
  //WebApps.setCredentials("Kniwwelino_9", "EDD1C");
  WebApps.init(true);
  //pinMode(D5, OUTPUT);

  RGB.init();
  RGB.start();
}

void loop()
{
  WebApps.handleClient();
  ArduinoOTA.handle();
  Kniwwelino.loop();
  RGB.service();
}