/*
    Web-Apps.ha - Library to enable web-functionality on the Kniwwelino board.

    Released under LGPL 3.0.
*/

#include "Arduino.h"
#include "Kniwwelino-WebApps.h"
#include "FS.h"
#include "ArduinoJson.h"

WebAppsLib::WebAppsLib() {}

void WebAppsLib::init(int port)
{
    ESP8266WebServer server(port);
    //What the server does when the user is on the "/" directory.
    //server.on("/", WebApps.handleIndexFile);

    //List available files.
    server.on("/list", HTTP_GET, std::bind(&WebAppsLib::handleIndexFile, this));

    //Handles files not found
    server.onNotFound(std::bind(&WebAppsLib::handleWebRequests, this));

    //Starts the web-server.
    server.begin();
}

void WebAppsLib::handle()
{
    server.handleClient();
}
/*
void WebAppsLib::on(const String path, THandlerFunction handler)
{
    server.on(path, handler);
}
*/
//converts a boolean to a string by returning 1 if boolean is true and 0 if boolean is false
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

//Puts data to send into json format and sends it to web-server.
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

//A function to redirect the user from the / directory to index.html.
void WebAppsLib::handleIndexFile()
{
    File file = SPIFFS.open("/index.html", "r");
    server.streamFile(file, "text/html");
    file.close();
}

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

WebAppsLib WebApps = WebAppsLib();