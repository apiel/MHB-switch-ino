#include <ESP8266HTTPClient.h>

void callUrl(String route, String params) {
    HTTPClient http;  //Declare an object of class HTTPClient
  
    String url = route + "?device=" + String(DEVICE_ID);
    url += "&mac=" +  WiFi.macAddress() + "&ip=" + WiFi.localIP().toString() + params;
    url.replace(" ", "+");
    Serial.println(url);
    http.begin(url);
    int httpCode = http.GET();
    Serial.println("http result: " + String(httpCode));
    http.end();
}

void callMiddleware(String action, String params) {
  callUrl("http://192.168.0.122:3000/esp/" + action, params);
}

