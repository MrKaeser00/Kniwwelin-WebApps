/*
    Web-Apps.ha - Library to enable web-functionality on the Kniwwelino board.

    Released under LGPL 3.0.
*/
#ifndef WebApps_h
#define WebApps_h

#include "Arduino.h"
#include "ESP8266WebServer.h"

class WebAppsLib
{
public:
    WebAppsLib();
    void init(int port);
    String bool2string(boolean boo);
    void sendData(String data);
    void handle();
    //typedef std::function<void(void)> THandlerFunction;
    //void on(const String path, THandlerFunction handler);
    ESP8266WebServer server;

private:
    void handleFileList();
    void hanleFileIndex();
    void handleIndexFile();
    bool loadFromSpiffs(String path);
    void handleWebRequests();
};

extern WebAppsLib WebApps;
#endif