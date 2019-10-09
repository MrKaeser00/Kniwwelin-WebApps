/*
    Web-Apps.ha - Library to enable web-functionality on the Kniwwelino board.

    Released under LGPL 3.0.
*/
#ifndef WebApps_h
#define WebApps_h

#include "Arduino.h"
#include "ESP8266WebServer.h"

#define INDEX_FILE  "/index.html"
#define LOGO_FILE   "/logo.png"

#define ROOT_DIR    "/"
#define LIST_DIR    "/list"
#define GET_DIR     "/get"

#define JSONBUFFER  200
#define WEB_PORT    80

class WebAppsLib
{
public:
    ESP8266WebServer server;
    WebAppsLib();
    void init();
    void handle();
    typedef std::function<void(void)> THandlerFunction;
    void on(const String path, THandlerFunction handler);
    void on(const String path, HTTPMethod method, THandlerFunction handler);
    void sendData(String topic, String data);
    String arg(int arg);
    void send(int code, const String contentType, const String content);
    String bool2string(boolean boo);
    void handleGet();
    
    
private:
    void handleIndexFile();
    void handleLogo();
    void handleFileList();
    //void handleGet();
};

extern WebAppsLib WebApps;
#endif