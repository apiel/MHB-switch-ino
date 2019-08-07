#include <ESP8266WebServer.h>

ESP8266WebServer server(80);

void handleRoot() {
  Serial.println("> Handle root.");
  server.send(200, "text/plain", "hello from esp8266!");
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
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void httpdHandle(){
  server.handleClient();
}

