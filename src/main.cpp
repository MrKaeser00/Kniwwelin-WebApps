#include <Kniwwelino.h>
#include <ArduinoOTA.h>
#include "server.h"

// Set web server port number to 80
WiFiServer server(80);

// Assign output variables to GPIO pins
//const int output5 = 5;
//const int output4 = 4;

void setup()
{
  Serial.begin(115200);
  ArduinoOTA.begin();
  Kniwwelino.begin("Name", true, true, false); // Wifi=true, Fastboot=true, MQTT Logging=false
  // Initialize the output variables as outputs
  //pinMode(output5, OUTPUT);
  //pinMode(output4, OUTPUT);
  // Set outputs to LOW
  //digitalWrite(output5, LOW);
  //digitalWrite(output4, LOW);

  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(Kniwwelino.getIP());
  server.begin();
}

void loop()
{
  WiFiClient client = server.available(); // Listen for incoming clients
  checkClient(client);
  ArduinoOTA.handle();
}