/*
    Web-Apps.ha - Library to enable web-functionality on the Kniwwelino board.

    Released under LGPL 3.0.
*/

#include "Arduino.h"
#include "Kniwwelino-WebApps.h"
#include "FS.h"
#include "ArduinoJson.h"
#include "Kniwwelino.h"

//instantiates the WebServer object ionto the server pointer with WEB_PORT as an argument.
ESP8266WebServer server(WEB_PORT);

WebAppsLib::WebAppsLib() {}

//Initializes basic functionality (index.html) and starts the web-server. Needs to be called in setup().
void WebAppsLib::init()
{
    //
    SPIFFS.begin();

    //What the server does when the user is on the ROOT_DIR ("/") directory.
    server.on(ROOT_DIR, std::bind(&WebAppsLib::handleIndexFile, this));

    //Forward the logo to the server.
    server.on(LOGO_FILE, std::bind(&WebAppsLib::handleLogo, this));

    //List available files on the ESP8266's filestorage.
    server.on(LIST_DIR, HTTP_GET, std::bind(&WebAppsLib::handleFileList, this));

    //Get data from input field on site.
    server.on(GET_DIR, HTTP_GET, std::bind(&WebAppsLib::handleGet, this));

    //Starts the web-server.
    server.begin();
}

//A function that tells the server to handle clients. Needs to be called in loop().
void WebAppsLib::handle()
{
    server.handleClient();
}
//Lets the user define in their code what happens on a given uri. Just forwards path and function to server.on(path, function). Is Called during setup().
void WebAppsLib::on(const String path, THandlerFunction handler)
{
    server.on(path, handler);
}

//Lets the user define in their code what happens on a given uri. Just forwards path, method and function to server.on(path, method,  function). Is Called during setup().
void WebAppsLib::on(const String path,HTTPMethod method, THandlerFunction handler) 
{
    server.on(path, method, handler);
}

//Forwards server.arg(int arg).
String WebAppsLib::arg(int arg) {
    return server.arg(arg);
}

//Forwards server.send(int code, const String contentType, const String content).
void WebAppsLib::send(int code, const String contentType, const String content) {
    server.send(code, contentType, content);
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
    server.send(200, "application/json", strdata);
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
  JsonObject &dataJson = jsonBuffer.parse(server.arg(argNum));
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
    server.streamFile(file, "text/html");
    file.close();
}

//A function which is responsible to give the server the logo in the main page.
void WebAppsLib::handleLogo()
{
    File file = SPIFFS.open(LOGO_FILE, "r");
    server.streamFile(file, "image/png");
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
    server.send(200, "text/plain", output);
}

void WebAppsLib::handleGet() {
    String inputMessage;

    if (server.hasArg("input1"))
    {
        inputMessage = server.arg(0);
        Serial.println(inputMessage);
        Kniwwelino.MATRIXwriteAndWait(inputMessage);
    }
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");
}

WebAppsLib WebApps;