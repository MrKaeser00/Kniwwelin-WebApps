#include <Kniwwelino.h>
#include <Kniwwelino-WebApps.h>

//Variable to store the LED state.
bool ledState;

//Set your pin here.
uint8_t pin = D5;

void setup()
{
  Serial.begin(115200);
  Kniwwelino.begin("WebApps-LED", true, true, false); // Wifi=true, Fastboot=true, MQTT Logging=false

  // Print local IP address.
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(Kniwwelino.getIP());

  //Handle led state on site
  WebApps.on("/ledstate", changeLedState);
  WebApps.on("/cledstate", checkLedState);

  //Initializes WebApps library.
  WebApps.init();

  //Set pin to ouput
  pinMode(pin, OUTPUT);

}

void loop()
{
  WebApps.handle();
  Kniwwelino.loop();
}

//A function to toggle the LED on and off.
void changeLedState()
{
  Serial.println("changeLedState called");
  if (ledState)
  {
    WebApps.sendData("state", "Off");
    Kniwwelino.PINsetEffect(pin, PIN_OFF);
    Kniwwelino.MATRIXwrite(WebApps.bool2string(!ledState));
    ledState = !ledState;
  }
  else
  {
    WebApps.sendData("state", "On");
    Kniwwelino.MATRIXwrite(WebApps.bool2string(!ledState));
    Kniwwelino.PINsetEffect(pin, PIN_ON);
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