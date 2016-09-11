/*
 * ESPloit
 * WiFi controlled HID Keyboard Emulator
 * By www.Exploit.Agency / www.LegacySecurityGroup.com
*/

//We need this stuff
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <SoftwareSerial.h>

//Setup RX and TX pins to be used for the software serial connection
const int RXpin=5;
const int TXpin=4;
SoftwareSerial SOFTserial(RXpin,TXpin);

//Length of time in ms to wait between sending lines from payload
int DelayLength=2000;

// ***Begin WiFi Configuration***
const int accesspointmode = 1; // set to 0 to connect to an existing network or leave it set to 1 to use the esp8266 as an access point

// SSID and PASSWORD of network go below
const char ssid[] = "Exploit";
const char password[] = "DotAgency";
// channel and hidden are for when using the esp8266 as an access point
const int channel = 6;
const int hidden = 1; // set int hidden to 0 to broadcast SSID of access point or leave as 1 to hide SSID

// Configure Networking
IPAddress local_IP(192,168,1,1); //IP of the esp8266 server
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

// Port for web server
ESP8266WebServer server(80);
// ***End of WiFi Configuration block***

File fsUploadFile;
String webString;

void handleFileUpload()
{
  if(server.uri() != "/upload") return;
  HTTPUpload& upload = server.upload();
  if(upload.status == UPLOAD_FILE_START){
    String filename = upload.filename;
    filename = "/payloads/"+filename;
    Serial.print("Uploading file "); 
    Serial.print(filename+" ... ");
    fsUploadFile = SPIFFS.open(filename, "w");
    filename = String();
  }
  else if(upload.status == UPLOAD_FILE_WRITE){
    if(fsUploadFile)
    fsUploadFile.write(upload.buf, upload.currentSize);
  }
  else if(upload.status == UPLOAD_FILE_END){
    if(fsUploadFile)
    fsUploadFile.close();
    Serial.println("Success");
  }
}

void ListPayloads(){
  String FileList = "<a href=\"/\"><- BACK TO INDEX</a><br><br><b>Choose Payload:</b><br><br>";
  Dir dir = SPIFFS.openDir("/payloads");
  while (dir.next()) {
    String FileName = dir.fileName();
    File f = dir.openFile("r");
    FileList += " ";
    FileList += "<a href=\"/showpayload?payload="+FileName+"\">"+FileName+"</a>"+"<br>";
  }
  server.send(200, "text/html", FileList);
}

void setup(void)
{
  SOFTserial.begin(4800);
  Serial.begin(115200);
  Serial.println("");
  Serial.println("ESPloit - Wifi Controlled HID Keyboard Emulator");
  Serial.println("");
  SPIFFS.begin();
  
// Determine if set to Access point mode
  if (accesspointmode == 1) {
    Serial.print("Setting up Network Configuration ... ");
    Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Success" : "Failed!");

    Serial.print("Starting Access Point ... ");
    Serial.println(WiFi.softAP(ssid, password, channel, hidden) ? "Success" : "Failed!");

    Serial.print("IP address = ");
    Serial.println(WiFi.softAPIP());
  }
// or Join existing network
  else if (accesspointmode != 1) {
    Serial.print("Setting up Network Configuration ... ");
    Serial.println(WiFi.config(local_IP, gateway, subnet) ? "Success" : "Failed!");

    Serial.print("Connecting to network ... ");
    Serial.println(WiFi.begin(ssid, password) ? "Success" : "Failed!");

    Serial.print("IP address = ");
    Serial.println(WiFi.localIP());
  }

//Set up Web Pages
  server.on("/", [](){
    server.send(200, "text/html", "<html><body>-----<br><b>ESPloit</b> - WiFi controlled HID Keyboard Emulator<br><i>by www.Exploit.Agency / www.LegacySecurityGroup.com</i><br>-----<br><a href=\"/uploadpayload\">Upload Payload</a></html><br>-<br><a href=\"/listpayloads\">Choose Payload</a><br>-<br><a href=\"/format\">Format File System</a></html>");
  });

  server.on("/format", [](){
    server.send(200, "text/html", "<html><body>This will reformat the SPIFFS File System.<br><br>Are you sure?<br><br><a href=\"/format/yes\">YES</a> - <a href=\"/\">NO</a></html></html>");
  });

  server.on("/format/yes", [](){
    server.send(200, "text/html", "<a href=\"/\"><- BACK TO INDEX</a><br><br>Formatting file system: This may take up to 90 seconds");
    Serial.print("Formatting file system...");
    SPIFFS.format();
    Serial.println(" Success");
  });
    
  server.on("/uploadpayload", []() {
    server.send(200, "text/html", "<a href=\"/\"><- BACK TO INDEX</a><br><br><b>Upload Payload:</b><br><br><form method='POST' action='/upload' enctype='multipart/form-data'><input type='file' name='upload'><input type='submit' value='Upload'></form>");
  });
    
  server.on("/listpayloads", ListPayloads);
    
  server.onFileUpload(handleFileUpload);
    
  server.on("/upload", HTTP_POST, []() {
    server.send(200, "text/html", "<a href=\"/\"><- BACK TO INDEX</a><br><br>Upload Successful!<br><br><a href=\"/listpayloads\">List Payloads</a>");
  });

  server.on("/showpayload", [](){
    webString="";
    String payload;
    payload += server.arg(0);
    File f = SPIFFS.open(payload, "r");
    String webString = f.readString();
    f.close();
    server.send(200, "text/html", "<a href=\"/\"><- BACK TO INDEX</a><br><br><a href=\"/dopayload?payload="+payload+"\"><button>Run Payload</button></a><pre>"+payload+"\n-----\n"+webString+"</pre>");
    webString="";
  });

  server.on("/dopayload", [](){
    String dopayload;
    dopayload += server.arg(0);
    server.send(200, "text/html", "<a href=\"/\"><- BACK TO INDEX</a><br><br><pre>Running payload: "+dopayload+"</pre><br>This may take a while to complete...");
    Serial.println("Running payaload: "+dopayload);
    File f = SPIFFS.open(dopayload, "r");
    while(f.available()) {
      String line = f.readStringUntil('\n');
      SOFTserial.println(line);
      Serial.println(line);
      delay(DelayLength); //delay between lines in payload, I found running it slower works best
    }
    f.close();
  });
    
  server.begin();
  Serial.println("Web Server Started");
}

void loop() {
  server.handleClient();
}
