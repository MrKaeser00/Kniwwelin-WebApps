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

  //Set pin to output
  pinMode(pin, OUTPUT);

}

void loop()
{
  //Handles clients.
  WebApps.handle();
  Kniwwelino.loop();
}

//A function to toggle the LED on and off.
void changeLedState()
{
  //Checks what state the LED is currently in.
  if (ledState)
  {
	//Sends a string to the server with the name "state" and value "Off".  
    WebApps.sendData("state", "Off");
    Kniwwelino.PINsetEffect(pin, PIN_OFF);
	//Writes a "0" or "1" on the Matrix depending on the LED state.
    Kniwwelino.MATRIXwrite(WebApps.bool2string(!ledState));
	//Inverts LED state.
    ledState = !ledState;
  }
  else
  {
	//Sends a string to the server with the name "state" and value "On".  
    WebApps.sendData("state", "On");
	//Writes a "0" or "1" on the Matrix depending on the LED state.
    Kniwwelino.MATRIXwrite(WebApps.bool2string(!ledState));
    Kniwwelino.PINsetEffect(pin, PIN_ON);
	//Inverts LED state.
    ledState = !ledState;
  }
}

//A function to check for the LED state (only used when first (re)loading the page).
void checkLedState()
{
  Kniwwelino.MATRIXwrite(WebApps.bool2string(ledState));
  //Checks what state the LED is currently in.
  if (ledState)
  {
    WebApps.sendData("state", "On");
  }
  else
  {
    WebApps.sendData("state", "Off");
  }
}