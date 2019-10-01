/*
    Web-Apps.ha - Library to enable web-functionality on the Kniwwelino board.

    Released under LGPL 3.0.
*/

#include "Arduino.h"
#include "Kniwwelino-WebApps.h"
#include "FS.h"
#include "ArduinoJson.h"

//instantiates the WebServer object ionto the server pointer with WEB_PORT as an argument.
ESP8266WebServer server(WEB_PORT);

WebAppsLib::WebAppsLib() {}

//Initializes basic functionality (index.html) and starts the web-server. Needs to be called in setup().
void WebAppsLib::init()
{
    //What the server does when the user is on the ROOT_DIR ("/") directory.
    server.on(ROOT_DIR, std::bind(&WebAppsLib::handleIndexFile, this));

    //Forward the logo to the server.
    server.on(LOGO_FILE, std::bind(&WebAppsLib::handleLogo, this));

    //List available files on the ESP8266's filestorage.
    server.on(LIST_DIR, HTTP_GET, std::bind(&WebAppsLib::handleFileList, this));

    //Handles files not found.
    //server.onNotFound(std::bind(&WebAppsLib::handleWebRequests, this));

    //Starts the web-server.
    server.begin();
}

//A function that tells the server to handle clients. Needs to be called in loop().
void WebAppsLib::handle()
{
    server.handleClient();
}

//Lets the user define in their code what happens on a given uri. Just forwards path an function to server.on(path, function). Is Called during setup().
void WebAppsLib::on(const String path, THandlerFunction handler)
{
    server.on(path, handler);
}

//Puts data to send into json format and sends it to web-server. Can be called from any funcion.
void WebAppsLib::sendData(String data)
{
    StaticJsonBuffer<30> jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    root["state"] = data;
    String strdata;
    root.printTo(strdata);
    strdata = "[" + strdata + "]";
    server.send(200, "application/json", strdata);
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

/*
bool WebAppsLib::loadFromSpiffs(String path)
{
    String dataType = "text/plain";
    if (path.endsWith(".src"))
        path = path.substring(0, path.lastIndexOf("."));
    else if (path.endsWith(".html"))
        dataType = "text/html";
    //else if(path.endsWith(".htm")) dataType = "text/html";
    //else if(path.endsWith(".css")) dataType = "text/css";
    //else if(path.endsWith(".js")) dataType = "application/javascript";
    else if (path.endsWith(".png"))
        dataType = "image/png";
    //else if(path.endsWith(".gif")) dataType = "image/gif";
    //else if(path.endsWith(".jpg")) dataType = "image/jpeg";
    //else if(path.endsWith(".ico")) dataType = "image/x-icon";
    //else if(path.endsWith(".xml")) dataType = "text/xml";
    //else if(path.endsWith(".pdf")) dataType = "application/pdf";
    //else if(path.endsWith(".zip")) dataType = "application/zip";
    File dataFile = SPIFFS.open(path.c_str(), "r");
    if (server.hasArg("download"))
        dataType = "application/octet-stream";
    if (server.streamFile(dataFile, dataType) != dataFile.size())
    {
    }

    dataFile.close();
    return true;
}

void WebAppsLib::handleWebRequests()
{
    if (WebApps.loadFromSpiffs(server.uri()))
        return;
    String message = "File Not Detected\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++)
    {
        message += " NAME:" + server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
    Serial.println(message);
}
*/

WebAppsLib WebApps;