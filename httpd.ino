#include <ESP8266WebServer.h>

ESP8266WebServer server(80);

void handleRoot() {
  Serial.println("> Handle root.");
  String message = "Hello from esp8266 MHB-switch.\n\n";
  message += "device name:  " + String(DEVICE_NAME) + "\n";
  message += "device id:    " + String(DEVICE_ID) + "\n";
  message += "mac address:  " + wifiGetMac() + "\n";
  message += "relay status: " + relayGetStatus() + "\n";
  server.send(200, "text/plain", message);
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

void handleNotFound(){
  Serial.println("> Handle not found.");
  server.send(404, "text/plain", "Not found");
}

void httpdInit(){
  server.on("/", handleRoot);
  server.on("/reboot", handleReboot);
  server.on("/on", handleRelayOn);
  server.on("/off", handleRelayOff);
  server.on("/toggle", handleRelayToggle);
  server.on("/wemo/setup.xml", handleUpnpSetup);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void httpdHandle(){
  server.handleClient();
}

