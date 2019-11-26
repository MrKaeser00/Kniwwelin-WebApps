/*
    Web-Apps.h - Library to enable web-functionality on the Kniwwelino board.

    Released under LGPL 3.0.
*/
#ifndef WebApps_h
#define WebApps_h

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <FS.h>

#include <Kniwwelino.h>

#include <map>

#define INDEX_FILE  "/index.html"
#define LOGO_FILE   "/logo.png"

#define ROOT_DIR    "/"
#define LIST_DIR    "/list"
#define GET_DIR     "/get"

#define JSONBUFFER  100
#define WEB_PORT    80

class WebAppsLib: public ESP8266WebServer
{
public:
    WebAppsLib();
    void init(boolean enableFailover);
    void setCredentials(const char *ssid, const char *password);
    void sendData(String topic, String data);
    String getData(String topic);
    String getData(int argNum, String topic);
    String getColorData(String topic);
    String bool2string(boolean boo);
    void handleGet();

    void setRGBLed();

    void contentBuilder(String sTemplate);
    void contentBuilder(char *arrayTemplates[],const int numberOfElements); 

    void pageBuilder(String style, String content, String script);

    String ledSiteBuilder(String buf, String ledId);
    
private:
    void handleIndexFile();
    void handleLogo();
    void handleFileList();

    void getConfig();
    void remConfig();
    void remEverything();

    const char *_ssid;
    const char *_password;
    String _page;

    void pageServer();

    std::map<String, bool> _led;


    //boolean _led[] = {};

    void changeLedState();
    void checkLedState();
    void servePagetxt();

    String extractJSON(JsonObject json, String topic);
};

extern WebAppsLib WebApps;
#endif