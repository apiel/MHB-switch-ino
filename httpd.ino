#include <ESP8266WebServer.h>
#include <ESP8266httpUpdate.h>

ESP8266WebServer server(80);

#ifdef WEMOS
    #define PIN_LED 2
#else
    #define PIN_LED 13
#endif

void handleRoot() {
  Serial.println("> Handle root.");
  String message = "Hello from esp8266 MHB-switch.\n\n";
  message += "version:      " + String(FIRMWARE_VERSION) + "\n";
  message += "device name:  " + String(DEVICE_NAME) + "\n";
  message += "device id:    " + String(DEVICE_ID) + "\n";
  message += "mac address:  " + wifiGetMac() + "\n";
  message += "relay status: " + relayGetStatus() + "\n";
  server.send(200, "text/plain", message);
}

void handleOta() {
  Serial.println("> Handle Ota.");
  if (server.hasArg("url")) { 
    String url = server.arg("url");

    WiFiClient client;
    ESPhttpUpdate.setLedPin(PIN_LED, LOW);
    t_httpUpdate_return ret = ESPhttpUpdate.update(client, url);
   
    if (ret == HTTP_UPDATE_FAILED) {
      String message = "HTTP_UPDATE_FAILD Error (" + String(ESPhttpUpdate.getLastError()) + "): " + ESPhttpUpdate.getLastErrorString();
      server.send ( 200, "text/plain", message);
      Serial.println(message);
    } else if (ret == HTTP_UPDATE_NO_UPDATES) {
      server.send ( 200, "text/plain", "HTTP_UPDATE_NO_UPDATES.");
      Serial.println("HTTP_UPDATE_NO_UPDATES");
    } else if (ret == HTTP_UPDATE_OK) {
      server.send ( 200, "text/plain", "HTTP_UPDATE_OK.");
      Serial.println("HTTP_UPDATE_OK");
    } else {
      server.send ( 200, "text/plain", "Try to update firmware with unknown response."); 
      Serial.println("UNKNOWN_RETURN");
    }
  }
  else {
    server.send ( 400, "text/plain", "Update firmware parameter missing. Provide host and path e.g: http://sonoff.local/ota?url=http://192.168.0.120/firmware.bin");
  }  
}

void handleReboot() {
  Serial.println("> Handle reboot.");
  server.send(200, "text/plain", "reboot");
  delay(500);
  ESP.reset();
}

void handleRelayOn() {
  Serial.println("> Handle relay on.");
  relayOn();
  server.send(200, "text/plain", "Set relay on."); 
}

void handleRelayOff() {
  Serial.println("> Handle relay off.");
  relayOff();
  server.send(200, "text/plain", "Set relay off."); 
}

void handleRelayToggle() {
  Serial.println("> Handle relay toggle.");
  relayToggle();
  String message = "Toggle relay to " + relayGetStatus() + ".";
  server.send(200, "text/plain", message); 
}

void handleRelayTimer() {
  Serial.println("> Handle relay timer.");
  relaySetTimer();
  server.send(200, "text/plain", "Set relay timer."); 
}

void handleUpnpSetup() {
  Serial.println("> Handle Upnp setup.");
  String uid = String(DEVICE_ID) + "-" + wifiGetMac();
  String message = "<?xml version=\"1.0\"?>";
  message += "<root>";
  message += "<device>";
  message += "<deviceType>urn:Belkin:device:controllee:1</deviceType>";
  message += "<friendlyName>";
  message += String(DEVICE_NAME);
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
  server.send(200, "application/xml", message);
}

// curl -A '' -X POST -H 'Content-type: text/xml; charset="utf-8"' -H 'SOAPACTION: "urn:Belkin:service:basicevent:1#GetBinaryState"' -s http://192.168.0.25/upnp/control/basicevent1 --data '<?xml version="1.0" encoding="utf-8"?><s:Envelope xmlns:s="http://schemas.xmlsoap.org/soap/envelope/" s:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"><s:Body><u:SetBinaryState xmlns:u="urn:Belkin:service:basicevent:1"><BinaryState>1</BinaryState></u:SetBinaryState></s:Body></s:Envelope>'
// https://gist.github.com/aussieade/7647e2c892bf511510566a7d95f40d95

void handleUpnpController(){
  Serial.println("> Handle Upnp controller.");
  if (server.args()) {
    String body = server.arg(0);
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
  server.send(200, "application/xml", message);
}

void handleNotFound(){
  Serial.println("> Handle not found.");
  server.send(404, "text/plain", "Not found");
}

void httpdInit(){
  server.on("/", handleRoot);
  server.on("/reboot", handleReboot);
  server.on("/ota", handleOta);
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

void httpdHandle(){
  server.handleClient();
}

