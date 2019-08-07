#include <WiFiUdp.h>
//#include "lwip/igmp.h"

#define MCAST_PORT 1900

WiFiUDP udp;

IPAddress MCAST_ADDR(239, 255, 255, 250);

void upnpInit() {
  Serial.println("Upnp init.");
  //udp.beginPacketMulticast(MCAST_ADDR, MCAST_PORT, WiFi.localIP());
  udp.beginMulticast(WiFi.localIP(), MCAST_ADDR, MCAST_PORT);

/*
  ip_addr_t ifaddr;
  ifaddr.addr = WiFi.localIP();
  ip_addr_t multicast_addr;
  multicast_addr.addr = (uint32_t) MCAST_ADDR;
  igmp_joingroup(&ifaddr, &multicast_addr);
  */
}

void upnpHandle() {
  char incomingPacket[255];
  int packetSize = udp.parsePacket();
  if (packetSize) {
    // receive incoming UDP packets
    // Serial.printf("Received %d bytes from %s, port %d\n", packetSize, udp.remoteIP().toString().c_str(), udp.remotePort());
    int len = udp.read(incomingPacket, 255);
    if (len > 0) {
      incomingPacket[len] = 0;
    }
    // Serial.printf("UDP packet contents: %s\n", incomingPacket);
    if (String(incomingPacket).indexOf("ssdp:discover") != -1) {
      Serial.printf("Received ssdp:discover request from %s on port %d\n", udp.remoteIP().toString().c_str(), udp.remotePort());

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
      udp.beginPacket(udp.remoteIP(), udp.remotePort());
      udp.write(message.c_str());
      udp.endPacket();
    }
  }
}

/*
Received 107 bytes from 192.168.0.130, port 50000
UDP packet contents: M-SEARCH * HTTP/1.1
HOST: 239.255.255.250:1900
MAN: "ssdp:discover"
MX: 15
ST: urn:Belkin:device:**
*/


