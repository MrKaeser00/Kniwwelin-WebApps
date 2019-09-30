#include <Kniwwelino.h>
#include <ArduinoOTA.h>
#include <Kniwwelino-WebApps.h>
//#include <NewPing.h>

//NewPing hcsr04_D0D5(D0, D5, 200);

//Variable to store the LED state.
bool ledState;

//A function to toggle the LED on and off.
void changeLedState()
{
  Serial.println("changeLedState called");
  if (ledState)
  {
    WebApps.sendData("Off");
    //Kniwwelino.PINsetEffect(D5, PIN_OFF);
    Kniwwelino.MATRIXwrite(WebApps.bool2string(!ledState));
    ledState = !ledState;
  }
  else
  {
    WebApps.sendData("On");
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
    WebApps.sendData("On");
  }
  else
  {
    WebApps.sendData("Off");
  }
}

void setup()
{
  Serial.begin(115200);
  SPIFFS.begin();
  ArduinoOTA.begin();                          //Enables Over-The-Air updates
  Kniwwelino.begin("Name", true, true, false); // Wifi=true, Fastboot=true, MQTT Logging=false
  // Print local IP address.
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(Kniwwelino.getIP());

  //Handle led status on site
  WebApps.server.on("/ledstate", changeLedState);
  WebApps.server.on("/cledstate", checkLedState);

  WebApps.init(80);
  //pinMode(D5, OUTPUT);
}

void loop()
{
  WebApps.handle();
  ArduinoOTA.handle();
  Kniwwelino.loop();
  //Serial.println(hcsr04_D0D5.ping_cm());

}