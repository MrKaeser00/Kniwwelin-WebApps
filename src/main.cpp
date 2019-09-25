#include <Kniwwelino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>

// Set web server port number to 80.
ESP8266WebServer server(80);

//Buffer for file upload.
File fsUploadFile;

//Variable to store the LED state.
bool ledState;

//set LED pin
const uint8_t PIN = D0;

//A function to handle the file upload.
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

//A function to redirect the user from the / directory to index.html.
void handleFileList()
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

//A function to list the files on the esp8266 storage.
void handleIndexFile()
{
  File file = SPIFFS.open("/index.html", "r");
  server.streamFile(file, "text/html");
  file.close();
}

//A function to toggle the LED on and off.
void changeLedState()
{
  Serial.println("changeLedState called");
  if (ledState)
  {
    const char *xmlStr = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><led-state>Off</led-state>";
    server.send(200, "text/xml", xmlStr);
    //Kniwwelino.PINsetEffect(PIN, PIN_OFF);
    ledState = !ledState;
  }
  else
  {
    const char *xmlStr = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><led-state>On</led-state>";
    server.send(200, "text/xml", xmlStr);
    //Kniwwelino.PINsetEffect(PIN, PIN_ON);
    ledState = !ledState;
  }
}

//A function to check for the LED state (only used when first (re)loading the page).
void checkLedState()
{
  if (ledState)
  {
    const char *xmlStr = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><led-state>On</led-state>";
    server.send(200, "text/xml", xmlStr);
  }
  else
  {
    const char *xmlStr = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><led-state>Off</led-state>";
    server.send(200, "text/xml", xmlStr);
  }
}

bool loadFromSpiffs(String path){
  String dataType = "text/plain";
  if(path.endsWith("/")) path += "index.html";

  if(path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
  else if(path.endsWith(".html")) dataType = "text/html";
  //else if(path.endsWith(".htm")) dataType = "text/html";
  //else if(path.endsWith(".css")) dataType = "text/css";
  //else if(path.endsWith(".js")) dataType = "application/javascript";
  else if(path.endsWith(".png")) dataType = "image/png";
  //else if(path.endsWith(".gif")) dataType = "image/gif";
  //else if(path.endsWith(".jpg")) dataType = "image/jpeg";
  //else if(path.endsWith(".ico")) dataType = "image/x-icon";
  //else if(path.endsWith(".xml")) dataType = "text/xml";
  //else if(path.endsWith(".pdf")) dataType = "application/pdf";
  //else if(path.endsWith(".zip")) dataType = "application/zip";
  File dataFile = SPIFFS.open(path.c_str(), "r");
  if (server.hasArg("download")) dataType = "application/octet-stream";
  if (server.streamFile(dataFile, dataType) != dataFile.size()) {
  }

  dataFile.close();
  return true;
}

void handleWebRequests(){
  if(loadFromSpiffs(server.uri())) return;
  String message = "File Not Detected\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " NAME:"+server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  Serial.println(message);
}

void setup()
{
  Serial.begin(115200);
  SPIFFS.begin();
  ArduinoOTA.begin(); //Enables Over-The-Air updates
  Kniwwelino.begin("Name", true, true, false); // Wifi=true, Fastboot=true, MQTT Logging=false

  // Print local IP address.
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(Kniwwelino.getIP());

  //What the server does when the user is on the "/" directory.
  server.on("/", handleIndexFile);

  //List available files.
  server.on("/list", HTTP_GET, handleFileList);

  //Handle file upload.
  server.on("/upload", HTTP_POST, []() {
    server.send(200, "text/plain", "{\"success\":1}");
  },
            handleFileUpload);

  //Handle led status on site
  server.on("/ledstate", changeLedState);
  server.on("/cledstate", checkLedState);
  
  //Handle the logo.
  server.on("/logo.png", HTTP_POST, []() {
    File im = SPIFFS.open("/logo.png", "r");
    server.streamFile(im, "image/png");
    server.send(200, "text/plain", "{\"success\":1}");
    im.close();
  } );
  
  //Handles files not found
  server.onNotFound(handleWebRequests);

  //Starts the web-server.
  server.begin();

  //pinMode(PIN, OUTPUT);
}

void loop()
{
  server.handleClient();
  ArduinoOTA.handle();
  Kniwwelino.loop();
}