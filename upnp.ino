#include <WiFiUdp.h>

#define MCAST_PORT_CLIENT 50000
// #define MCAST_PORT_SERVER 1900

WiFiUDP udp;

IPAddress MCAST_ADDR(239, 255, 255, 250);

unsigned long previousMillis = 0;
const unsigned long interval = 10UL*1000UL; // 10sec
const unsigned long timeout = 3UL*60UL*1000UL; // 3min

void upnpHandle() {
  unsigned long currentMillis = millis();
  if (currentMillis < timeout && currentMillis - previousMillis >= interval) {
    previousMillis += interval;
    upnpSend(MCAST_ADDR, MCAST_PORT_CLIENT); // could send a multicast packet every 10 sec
  }
}

// should we call it SSDP?
void upnpSend(IPAddress ip, int port) {
      Serial.println("Send udp multicast packet.");
      String message = "HTTP/1.1 200 OK\r\n";;
      message += "CACHE-CONTROL: max-age=86400\r\n";
      message += "DATE: Fri, 15 Apr 2016 04:56:29 GMT\r\n";
      message += "EXT:\r\n";
      message += "LOCATION: http://" + WiFi.localIP().toString() + ":80/wemo/setup.xml\r\n";
      message += "OPT: \"http://schemas.upnp.org/upnp/1/0/\"; ns=01\r\n";
      message += "01-NLS: b9200ebb-736d-4b93-bf03-835149d13983\r\n";
      message += "SERVER: Unspecified, UPnP/1.0, Unspecified\r\n";
      message += "ST: urn:Belkin:device:**\r\n";
      message += "USN: uuid:Socket-1_0-38323636-4558-4dda-9188-cda0e6cc3dc0::urn:Belkin:device:**\r\n";
      message += "X-User-Agent: redsonic\r\n\r\n";
      // send back a reply, to the IP address and port we got the packet from
      udp.beginPacket(ip, port);
      udp.write(message.c_str());
      udp.endPacket();
}
