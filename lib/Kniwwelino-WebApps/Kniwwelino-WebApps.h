/*
    Web-Apps.h - Library to enable web-functionality on the Kniwwelino board.
    Author: Christophe Kayser
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

#define INDEX_FILE "/index.html"
#define LOGO_FILE "/logo.png"

#define ROOT_DIR "/"
#define LIST_DIR "/list"
#define GET_DIR "/get"
#define LED_TOGGLE_DIR "/changeLedState"
#define LED_CHECK_DIR "/checkLedState"

#define JSONBUFFER 100
#define WEB_PORT 80

class WebAppsLib : public ESP8266WebServer
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

    void contentBuilder(String sTemplate);
    void contentBuilder(char *arrayTemplates[], const int numberOfElements);

private:
    void handleFileList();
    void pageServer();

    void getConfig();
    void remConfig();

    void pageBuilder(String style, String content, String script);

    void setRGBLed();

    void changeLedState();
    void checkLedState();
    void servePagetxt();
	
    void handleGet();
	
    const char *_ssid = "Kniwwelino-AP";
    const char *_password = "kniwwelino";
    String _page;

    std::map<String, bool> _led;
};

extern WebAppsLib WebApps;
#endif