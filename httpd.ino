#include <ESP8266httpUpdate.h>
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

#ifdef WEMOS
    #define PIN_LED 2
#else
    #define PIN_LED 13
#endif

void handleRoot(AsyncWebServerRequest *request) {
  Serial.println("> Handle root.");
  String message = "Hello from esp8266 MHB-switch.\n\n";
  message += "version:      " + String(FIRMWARE_VERSION) + "\n";
  #ifdef USE_EEPROM
  message += "eeprom name:  " + eepromRead() + "\n";
  #endif
  message += "device id:    " + String(DEVICE_ID) + "\n";
  message += "mac address:  " + WiFi.macAddress() + "\n";
  message += "relay status: " + relayGetStatus() + "\n";

  request->send(200, "text/plain", message);
}

void handlePing(AsyncWebServerRequest *request) {
  Serial.println("> Handle ping.");
  request->send(200, "text/plain", "pong");
}

void handleReboot(AsyncWebServerRequest *request) {
  Serial.println("> Handle reboot.");
  request->send(200, "text/plain", "reboot");
  delay(500);
  ESP.reset();
}

#ifdef USE_EEPROM
void handleName(AsyncWebServerRequest *request) {
  Serial.println("> Handle Name.");
  if (request->hasParam("value")) { 
    String value = request->getParam("value")->value();
    eepromWrite(value);
    request->send ( 200, "text/plain", "OK.");
  } else {
    request->send ( 400, "text/plain", "Update device name, parameter missing. Provide value e.g: http://sonoff.local/name?value=my+device");
  } 
}
#endif

const char* otaHtml = "<form method='POST' action='/ota' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";
void handleOta(AsyncWebServerRequest *request) {
  AsyncWebServerResponse *response = request->beginResponse(200, "text/html", otaHtml);
  response->addHeader("Connection", "close");
  response->addHeader("Access-Control-Allow-Origin", "*");
  request->send(response);
}
void handleOtaUpdate(AsyncWebServerRequest *request) {
    // the request handler is triggered after the upload has finished... 
    // create the response, add header, and send response
    AsyncWebServerResponse *response = request->beginResponse(200, "text/html", (Update.hasError())?"FAIL":"<html><body><div>SUCCESS, need to <a href='/reboot'>reboot</a></div></body></html>");
    response->addHeader("Connection", "close");
    response->addHeader("Access-Control-Allow-Origin", "*");
    // restartRequired = true;  // Tell the main loop to restart the ESP
    request->send(response);
}
void handleOtaUpdateChunk(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
    if(!index){ // if index == 0 then this is the first frame of data
      Serial.printf("UploadStart: %s\n", filename.c_str());
      Serial.setDebugOutput(true);
      
      // calculate sketch space required for the update
      uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
      if(!Update.begin(maxSketchSpace)){//start with max available size
        Update.printError(Serial);
      }
      Update.runAsync(true); // tell the updaterClass to run in async mode
    }

    //Write chunked data to the free sketch space
    if(Update.write(data, len) != len){
        Update.printError(Serial);
    }
    
    if(final){ // if the final flag is set then this is the last frame of data
      if(Update.end(true)){ //true to set the size to the current progress
          Serial.printf("Update Success: %u B\nNeed to reboot...\n", index+len);
          // ESP.restart();
        } else {
          Update.printError(Serial);
        }
        Serial.setDebugOutput(false);
    }
}

void handleRelayOn(AsyncWebServerRequest *request) {
  Serial.println("> Handle relay on.");
  relayOn();
  request->send(200, "text/plain", "Set relay on."); 
}

void handleRelayOff(AsyncWebServerRequest *request) {
  Serial.println("> Handle relay off.");
  relayOff();
  request->send(200, "text/plain", "Set relay off."); 
}

void handleRelayToggle(AsyncWebServerRequest *request) {
  Serial.println("> Handle relay toggle.");
  relayToggle();
  String message = "Toggle relay to " + relayGetStatus() + ".";
  request->send(200, "text/plain", message); 
}

void handleRelayTimer(AsyncWebServerRequest *request) {
  Serial.println("> Handle relay timer.");
  relaySetTimer();
  request->send(200, "text/plain", "Set relay timer."); 
}

void handleUpnpSetup(AsyncWebServerRequest *request) {
  Serial.println("> Handle Upnp setup.");
  String uid = String(DEVICE_ID) + "-" + WiFi.macAddress();
  String message = "<?xml version=\"1.0\"?>";
  message += "<root>";
  message += "<device>";
  message += "<deviceType>urn:Belkin:device:controllee:1</deviceType>";
  message += "<friendlyName>";
  #ifdef USE_EEPROM
  message += eepromRead();
  #else
  message += uid;
  #endif
  message += "</friendlyName>";
  message += "<manufacturer>Belkin International Inc.</manufacturer>";
  message += "<modelName>Emulated Socket</modelName>";
  message += "<modelNumber>3.1415</modelNumber>";
  message += "<UDN>uuid:Socket-1_0-" + uid + "</UDN>";
  message += "<serialNumber>" + uid + "</serialNumber>";
  message += "<binaryState>0</binaryState>";
  message += "<serviceList>";
  message += "<service>";
  message += "<serviceType>urn:Belkin:service:basicevent:1</serviceType>";
  message += "<serviceId>urn:Belkin:serviceId:basicevent1</serviceId>";
  message += "<controlURL>/upnp/control/basicevent1</controlURL>";
  message += "<eventSubURL>/upnp/event/basicevent1</eventSubURL>";
  message += "<SCPDURL>/eventservice.xml</SCPDURL>";
  message += "</service>";
  message += "</serviceList>";
  message += "</device>";
  message += "</root>";
  request->send(200, "application/xml", message);
}

// curl -A '' -X POST -H 'Content-type: text/xml; charset="utf-8"' -H 'SOAPACTION: "urn:Belkin:service:basicevent:1#GetBinaryState"' -s http://192.168.0.25/upnp/control/basicevent1 --data '<?xml version="1.0" encoding="utf-8"?><s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/" s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><s:Body><u:SetBinaryState xmlns:u="urn:Belkin:service:basicevent:1"><BinaryState>1</BinaryState></u:SetBinaryState></s:Body></s:Envelope>'
// https://gist.github.com/aussieade/7647e2c892bf511510566a7d95f40d95

void handleUpnpController(AsyncWebServerRequest *request){
  Serial.println("> Handle Upnp controller.");

  if (request->args()) {
    int pos = 0;
    String body = request->arg(pos).c_str();
    if (body.indexOf("SetBinaryState") != -1) {
      Serial.println("Upnp controller set state.");
      if (body.indexOf("<BinaryState>1</BinaryState>") != -1) {
        relayOn();
      } else {
        relayOff();
      }
    }
  }
  String message = "<?xml version=\"1.0\"?>";
  message += "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">";
  message += "<s:Body>";
  message += "<u:GetBinaryStateResponse xmlns:u=\"urn:Belkin:service:basicevent:1\">";
  message += "<BinaryState>" + String(relayGetState()) + "</BinaryState>";
  message += "</u:GetBinaryStateResponse>";
  message += "</s:Body>";
  message += "</s:Envelope>";
  request->send(200, "application/xml", message);
}


void handleNotFound(AsyncWebServerRequest *request){
  Serial.println("> Handle not found.");
  request->send(404, "text/plain", "Not found");
}

void httpdInit(){
  server.on("/", handleRoot);
  server.on("/ping", handlePing);
  server.on("/reboot", handleReboot);
  #ifdef USE_EEPROM
  server.on("/name", handleName);
  #endif
  server.on("/ota", HTTP_GET, handleOta);
  server.on("/ota", HTTP_POST, handleOtaUpdate, handleOtaUpdateChunk);
  
  server.on("/on", handleRelayOn);
  server.on("/off", handleRelayOff);
  server.on("/toggle", handleRelayToggle);
  server.on("/timer", handleRelayTimer);
  server.on("/wemo/setup.xml", handleUpnpSetup);
  server.on("/upnp/control/basicevent1", handleUpnpController);
  server.onNotFound(handleNotFound);
  
  server.begin();

  Serial.println("HTTP server started");
}

