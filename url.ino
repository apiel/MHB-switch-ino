void callUrl(String route, String params) {
    HTTPClient http;  //Declare an object of class HTTPClient
  
    String url = route + "?name=" + eepromRead();
    url += "&mac=" + wifiGetMac() + "&ip=" + WiFi.localIP().toString() + params;
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

