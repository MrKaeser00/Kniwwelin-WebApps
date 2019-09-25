#include <Kniwwelino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>

// Set web server port number to 80
ESP8266WebServer server(80);

File fsUploadFile;

bool ledState;

//set LED pin
const uint8_t PIN = D0;

void handleFileUpload()
{
  HTTPUpload &upload = server.upload();
  if (upload.status == UPLOAD_FILE_START)
  {
    String filename = upload.filename;
    if (!filename.startsWith("/"))
      filename = "/" + filename;
    Serial.print("handleFileUpload Name: ");
    Serial.println(filename);
    fsUploadFile = SPIFFS.open(filename, "w");
  }
  else if (upload.status == UPLOAD_FILE_WRITE)
  {
    if (fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize);
  }
  else if (upload.status == UPLOAD_FILE_END)
  {
    if (fsUploadFile)
      fsUploadFile.close();
    Serial.print("handleFileUpload Size: ");
    Serial.println(upload.totalSize);
  }
}

void handleFileList()
{
  String path = "/";
  // Assuming there are no subdirectories
  Dir dir = SPIFFS.openDir(path);
  String output = "[";
  while (dir.next())
  {
    File entry = dir.openFile("r");
    // Separate by comma if there are multiple files
    if (output != "[")
      output += ",";
    output += String(entry.name()).substring(1);
    entry.close();
  }
  output += "]";
  server.send(200, "text/plain", output);
}

void handleIndexFile()
{
  File file = SPIFFS.open("/index.html", "r");
  server.streamFile(file, "text/html");
  file.close();
}

void changeLedState()
{
  Serial.println("changeLedState called"); 
  if (ledState)
  {
    char *xmlStr = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><led-state>Off</led-state>";
    server.send(200, "text/xml", xmlStr);
    //Kniwwelino.PINsetEffect(PIN, PIN_OFF);
    ledState = !ledState;
  }
  else
  {
    char *xmlStr = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><led-state>On</led-state>";
    server.send(200, "text/xml", xmlStr);
    //Kniwwelino.PINsetEffect(PIN, PIN_ON);
    ledState = !ledState;
  }
}

void setup()
{
  Serial.begin(115200);
  SPIFFS.begin();
  ArduinoOTA.begin();
  Kniwwelino.begin("Name", true, true, false); // Wifi=true, Fastboot=true, MQTT Logging=false

  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(Kniwwelino.getIP());

  server.on("/", handleIndexFile);

  // list available files
  server.on("/list", HTTP_GET, handleFileList);

  // handle file upload
  server.on("/upload", HTTP_POST, []() {
    server.send(200, "text/plain", "{\"success\":1}");
  },
            handleFileUpload);

  //handle led status on site
  server.on("/ledstate", changeLedState);

  server.begin();

  //pinMode(PIN, OUTPUT);
}

void loop()
{
  server.handleClient();
  ArduinoOTA.handle();
  Kniwwelino.loop();
}