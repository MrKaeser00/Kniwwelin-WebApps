#include <Kniwwelino.h>
#include <Kniwwelino-WebApps.h>
#include <NewPing.h>

uint8_t pin = D6;

//HC-SR04 ultrasonic distance sensor.
String dist;
NewPing sonic(pin, pin, 200);

void setup()
{
  Serial.begin(115200);
  Kniwwelino.begin("WebApps-Main", true, true, false); // Wifi=true, Fastboot=true, MQTT Logging=false

  // Print local IP address.
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(Kniwwelino.getIP());

  //Handle ultrasonic sensor
  WebApps.on("/sonic", checkSonic);

  //Initializes WebApps library.
  WebApps.init();
}

void loop()
{
  WebApps.handle();
  Kniwwelino.loop();
}

//A function to check the distance in front of an HC_SR04P ultrasonic sensor.
void checkSonic()
{
  //Gets distance in from of sensor.
  dist = sonic.ping_cm();
  //Adds "cm" to the output.
  dist += "cm";
  //Sends data to the server do be displayed.
  WebApps.sendData("sonic", dist);
}