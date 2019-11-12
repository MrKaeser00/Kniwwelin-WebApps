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
    //WebApps.on(ROOT_DIR, std::bind(&WebAppsLib::handleIndexFile, this));

    //Forward the logo to the server.
    //server.on(LOGO_FILE, std::bind(&WebAppsLib::handleLogo, this));

    //List available files on the ESP8266's filestorage.
    WebApps.on(LIST_DIR, HTTP_GET, std::bind(&WebAppsLib::handleFileList, this));

    //Get data from input field on site.
    WebApps.on(GET_DIR, HTTP_GET, std::bind(&WebAppsLib::handleGet, this));

    WebApps.on("/colorcode", HTTP_POST, std::bind(&WebAppsLib::setRGBLed, this));

    WebApps.on(ROOT_DIR, std::bind(&WebAppsLib::pageServer, this));
    //WebApps.on("/wifi", std::bind(&WebAppsLib::getConfig, this));
    //WebApps.on("/delconfig", std::bind(&WebAppsLib::remConfig, this));
    //WebApps.on("/deleverything", std::bind(&WebAppsLib::remEverything, this));

    WebApps.on("/changeLedState", HTTP_POST, std::bind(&WebAppsLib::changeLedState, this));
    WebApps.on("/checkLedState", HTTP_POST, std::bind(&WebAppsLib::checkLedState, this));

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
    _ssid = ssid;
    _password = password;
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
    Serial.println("sendData: "+strdata);
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
    Serial.println("getData: "+WebApps.arg(argNum));
    String dataString = dataJson.get<String>(topic);
    Serial.println("Str getData: "+dataString);
    return dataString;
}

//Returns color value as string in form '00ff00'.
String WebAppsLib::getColorData(String topic)
{
    String colorString = WebApps.getData(0, topic);
    colorString.remove(0, 1);
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

void WebAppsLib::handleGet()
{
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

void WebAppsLib::setRGBLed()
{
    String colorString = WebApps.getColorData("colorcode");
    const char *str = colorString.c_str();
    int r, g, b;
    sscanf(str, "%02x%02x%02x", &r, &g, &b);
    Kniwwelino.RGBsetColorEffect(r, g, b, RGB_ON, RGB_FOREVER);
}

void WebAppsLib::contentBuilder(String sTemplate)
{
    File file = SPIFFS.open("/t/" + sTemplate + ".temp", "r");
    String buf = file.readString();

    String style = buf.substring(buf.indexOf("<--StyleMarker") + 14, buf.indexOf("StyleMarker-->"));
    String content = buf.substring(buf.indexOf("<--ContentMarker") + 16, buf.indexOf("ContentMarker-->"));
    String script = buf.substring(buf.indexOf("<--ScriptMarker") + 15, buf.indexOf("ScriptMarker-->"));

    //Serial.println(style);
    //Serial.println(content);
    //Serial.println(script);

    WebApps.pageBuilder(style, content, script);
    file.close();
}

void WebAppsLib::contentBuilder(String arrayTemplates[], const int numberOfElements)
{
    String style;
    String content;
    String script;

    for (int i = 0; i <= numberOfElements; i++)
    {
        String buf;
        File file;
        int ledincluded = 0;

        Serial.println(i);

        if (arrayTemplates[i].startsWith("led_"))
        {
            
            //Serial.println(arrayTemplates[i]);
            file = SPIFFS.open("/t/led.temp", "r");
            buf = file.readString();
            
            buf.replace("<--ledId-->", arrayTemplates[i]);
            //Serial.println(buf);
            //buf.replace("<--checkLedId", "check_" + arrayTemplates[i]);
            //int num = (int)arrayTemplates[i].substring(5,6).c_str();
            //Serial.println(arrayTemplates[i].substring(5,6));
            _led[arrayTemplates[i]] = false;
            Serial.println(_led[arrayTemplates[i]]);

            //Serial.println(_led[arrayTemplates[i]]);
            ledincluded += 1;

            if (ledincluded == 1) 
            {
                script += buf.substring(buf.indexOf("<--ScriptMarker") + 15, buf.indexOf("ScriptMarker-->"));
            }
        }
        else
        {
            //Serial.println("rgbled ++ ");
            file = SPIFFS.open("/t/" + arrayTemplates[i] + ".temp", "r");
            buf = file.readString();
            script += buf.substring(buf.indexOf("<--ScriptMarker") + 15, buf.indexOf("ScriptMarker-->"));
        }

        style += buf.substring(buf.indexOf("<--StyleMarker") + 14, buf.indexOf("StyleMarker-->"));
        content += buf.substring(buf.indexOf("<--ContentMarker") + 16, buf.indexOf("ContentMarker-->"));
        //script += buf.substring(buf.indexOf("<--OneScriptMarker") + 18, buf.indexOf("OneScriptMarker-->"));
        file.close();
    }

    //Serial.println(style);
    //Serial.println(content);
    //Serial.println(script);

    WebApps.pageBuilder(style, content, script);
}

void WebAppsLib::pageBuilder(String style, String content, String script)
{
    String page = "<!DOCTYPE html>\n";
    page += "<html lang=\"en\">\n";
    page += "<head>\n";
    page += "<title>Kniwwelino-WebApps</title>\n";
    page += "<style>\n";
    //page += "body {color: BLACK; font: Arial; width: 100%;}";
    page += style;
    page += "</style>\n</head>\n<body>\n";
    page += content;
    page += "</body>\n<script>\n";
    page += script;
    page += "</script>\n</html>\n";

    _page = page;
}

void WebAppsLib::pageServer()
{
    WebApps.send(200, "text/html", _page);
}

void WebAppsLib::changeLedState()
{
    String ledId = WebApps.getData("ledId");
    //int id = (int)ledId.substring(4,5).c_str();
    Serial.println(ledId);
    bool ledState = _led[ledId];
    Serial.println(ledState);
    uint8_t pin = atoi(ledId.substring(4).c_str());
    if (ledState)
    {
        WebApps.sendData(ledId, "Off");
        //Kniwwelino.PINsetEffect(pin, PIN_OFF);
        Kniwwelino.MATRIXwrite(WebApps.bool2string(!ledState));
        _led[ledId] = !ledState;
    }
    else
    {
        WebApps.sendData(ledId, "On");
        Kniwwelino.MATRIXwrite(WebApps.bool2string(!ledState));
        //Kniwwelino.PINsetEffect(pin, PIN_ON);
        _led[ledId] = !ledState;
    }
}

//A function to check for the LED state (only used when first (re)loading the page).
void WebAppsLib::checkLedState()
{
    String ledId = WebApps.getData("ledId");
    //int id = (int)ledId.substring(4,5).c_str();
    //Serial.println(WebApps.getData("ledId"));
    //String ledId = WebApps.getData("ledId");
    Serial.println("ledId: "+ledId);
    boolean ledState = _led[ledId];
    Kniwwelino.MATRIXwrite(WebApps.bool2string(ledState));
    if (ledState)
    {
        WebApps.sendData(ledId, "On");
    }
    else
    {
        WebApps.sendData(ledId, "Off");
    }
}

WebAppsLib WebApps;