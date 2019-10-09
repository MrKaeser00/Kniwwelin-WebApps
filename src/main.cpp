#include <Kniwwelino.h>
#include <ArduinoOTA.h>
#include <Kniwwelino-WebApps.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <NewPing.h>

//Variable to store the LED state.
bool ledState;

//Variables for the ultrasonic sensor.
String dist;
NewPing sonic(D6, D6, 200);

//Variable to store the temperature.
String temp;

//Vairables for the temperature sensor to work.
OneWire OneWireD5(D5);
DallasTemperature tempSensor(&OneWireD5);

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
  //temp = tempSensor.getTempC(0);
  //Serial.println(system_get_rtc_time());
  WebApps.sendData("temp", "1°C");
}

//A function to check the distance in fornt of an HC_SR04P ultrasonic sensor.
void checkSonic()
{
  Serial.println("sonic called");
  dist = sonic.ping_cm();
  dist += "cm";
  WebApps.sendData("sonic", dist);
  Serial.println(dist);
}

void setRGBLed()
{
  Serial.println("setRGBLed called");
  StaticJsonBuffer<JSONBUFFER> jsonBuffer;
  JsonObject &colorJson = jsonBuffer.parse(WebApps.arg(0));
  String colorString;
  colorString = colorJson.get<String>("colorcode");
  Serial.println(colorString);
  Kniwwelino.RGBsetColor(colorString);
  //WebApps.send(200, "text/plain", "");
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

  //Handle temperature.
  WebApps.on("/temp", checkTemp);

  //Handle ultrasonic sensor
  WebApps.on("/sonic", checkSonic);

  WebApps.on("/colorcode", HTTP_POST, setRGBLed);

  Kniwwelino.RGBsetColor("#0000ff");

  //Initializes WebApps library.
  WebApps.init();
  //pinMode(D5, OUTPUT);
}

void loop()
{
  WebApps.handle();
  ArduinoOTA.handle();
  Kniwwelino.loop();
}