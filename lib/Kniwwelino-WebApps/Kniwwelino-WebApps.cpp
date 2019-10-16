/*
    Web-Apps.cpp - Library to enable web-functionality on the Kniwwelino board.

    Released under LGPL 3.0.
*/

#include "Kniwwelino-WebApps.h"

WebAppsLib::WebAppsLib() : ESP8266WebServer(WEB_PORT) {}

//Initializes basic functionality (index.html) and starts the web-server. Needs to be called in setup(). When failover is enabled, then you have to call setCredentials(ssid, password) before init(enableFailover)
void WebAppsLib::init(boolean enableFailover) //true=hotspot failover enabled
{
    //Starts filesystem access.
    SPIFFS.begin();

    //What the server does when the user is on the ROOT_DIR ("/") directory.
    WebApps.on(ROOT_DIR, std::bind(&WebAppsLib::handleIndexFile, this));

    //Forward the logo to the server.
    //server.on(LOGO_FILE, std::bind(&WebAppsLib::handleLogo, this));

    //List available files on the ESP8266's filestorage.
    WebApps.on(LIST_DIR, HTTP_GET, std::bind(&WebAppsLib::handleFileList, this));

    //Get data from input field on site.
    WebApps.on(GET_DIR, HTTP_GET, std::bind(&WebAppsLib::handleGet, this));

    //WebApps.on("/wifi", std::bind(&WebAppsLib::getConfig, this));
    //WebApps.on("/delconfig", std::bind(&WebAppsLib::remConfig, this));
    //WebApps.on("/deleverything", std::bind(&WebAppsLib::remEverything, this));

    MDNS.begin("Kniwwelino");

    if (enableFailover && !Kniwwelino.isConnected())
    {
        //WiFi.softAP(_ssid, _password);
        WiFi.mode(WIFI_AP);
        //MDNS.begin(Kniwwelino.getName().c_str());
        //MDNS.addService("esp", "tcp", 5353);
    }

    //Starts the web-server.
    WebApps.begin();
}

//Note: when using failsafe mode, the MQTT functions cannot be used in the code.
void WebAppsLib::setCredentials(const char *ssid, const char *password) 
{
    _ssid=ssid;
    _password=password;
}

//Puts data to send into json format and sends it to web-server. Can be called from any funcion.
void WebAppsLib::sendData(String topic, String data)
{
    StaticJsonBuffer<JSONBUFFER> jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    root[topic] = data;
    String strdata;
    root.printTo(strdata);
    strdata = "[" + strdata + "]";
    WebApps.send(200, "application/json", strdata);
}

//Returns data as string from json XMLHttpRequest and assumes the argument from the request is 0, which it usually is.
String WebAppsLib::getData(String topic)
{
    WebApps.getData(0, topic);
}

//Returns data as string from json XMLHttpRequest.
String WebAppsLib::getData(int argNum, String topic)
{
  StaticJsonBuffer<JSONBUFFER> jsonBuffer;
  JsonObject &dataJson = jsonBuffer.parse(WebApps.arg(argNum));
  String dataString = dataJson.get<String>(topic);
  return dataString;
}

//Returns color value as string in form '00ff00'.
String WebAppsLib::getColorData(String topic)
{
  String colorString = WebApps.getData(0, topic);
  colorString.remove(0,1);
  return colorString;
}

//converts a boolean to a string by returning 1 if boolean is true and 0 if boolean is false. Can be called from any funcion.
String WebAppsLib::bool2string(boolean boo)
{
    if (boo)
    {
        return "1";
    }
    else
    {
        return "0";
    }
}

//A function to redirect the user from the / directory to index.html.
void WebAppsLib::handleIndexFile()
{
    File file = SPIFFS.open(INDEX_FILE, "r");
    WebApps.streamFile(file, "text/html");
    file.close();
}

//A function which is responsible to give the server the logo in the main page.
void WebAppsLib::handleLogo()
{
    File file = SPIFFS.open(LOGO_FILE, "r");
    WebApps.streamFile(file, "image/png");
    file.close();
}

//A function to list the files on the esp8266 storage.
void WebAppsLib::handleFileList()
{
    String path = "/";
    // Assuming there are no subdirectories.
    Dir dir = SPIFFS.openDir(path);
    String output = "[";
    while (dir.next())
    {
        File entry = dir.openFile("r");
        // Separate by comma if there are multiple files.
        if (output != "[")
            output += ",";
        output += String(entry.name()).substring(1);
        entry.close();
    }
    output += "]";
    WebApps.send(200, "text/plain", output);
}

void WebAppsLib::handleGet() {
    String inputMessage;

    if (WebApps.hasArg("input1"))
    {
        inputMessage = WebApps.arg(0);
        Serial.println(inputMessage);
        Kniwwelino.MATRIXwriteAndWait(inputMessage);
    }
    WebApps.sendHeader("Location", "/", true);
    WebApps.send(302, "text/plain", "");
}

void WebAppsLib::getConfig() 
{
    File file = SPIFFS.open("/wifi.conf", "r");
    WebApps.streamFile(file, "text/plain");
    file.close();
}

void WebAppsLib::remConfig()
{
    SPIFFS.remove("/wifi.conf");
}

void WebAppsLib::remEverything()
{
    SPIFFS.remove("/wifi.conf");
    SPIFFS.remove("/index.html");
    SPIFFS.remove("/logo.png");
}
WebAppsLib WebApps;