#include <WiFiUdp.h>

#include <lwip/udp.h>
#include <lwip/igmp.h>

#define MCAST_PORT 1900

WiFiUDP udp;

IPAddress MCAST_ADDR(239, 255, 255, 250);

UdpContext * _server;

void upnpInit() {
  Serial.println("Upnp init.");
  // udp.beginMulticast(WiFi.localIP(), MCAST_ADDR, MCAST_PORT); // somehow do something
  // udp.begin(MCAST_PORT);

  // upnpSend(MCAST_ADDR, 50000); // could send a multicast packet every 10 sec (for first 3min)

/*
  ip_addr_t ifaddr;
  ifaddr.addr = WiFi.localIP();
  ip_addr_t multicast_addr;
  multicast_addr.addr = (uint32_t) MCAST_ADDR;
  igmp_joingroup(&ifaddr, &multicast_addr);
  */


  _server = new UdpContext;
  _server->ref();

  ip_addr_t ifaddr;
  ifaddr.addr = WiFi.localIP();
  ip_addr_t multicast_addr;
  multicast_addr.addr = (uint32_t) MCAST_ADDR;
  if (igmp_joingroup(&ifaddr, &multicast_addr) != ERR_OK ) {
    Serial.println("SSDP failed to join igmp group");
  } else if (_server->listen(*IP_ADDR_ANY, MCAST_PORT)) {
    _server->setMulticastInterface(ifaddr);
    _server->setMulticastTTL(1);
    _server->onRx(yoyo);
    if (!_server->connect(multicast_addr, MCAST_PORT)) {
      Serial.println("SSDP failed to connect");
    }
  } else {
    Serial.println("SSDP failed to listen");
  }
}


void yoyo() {
  if (_server->next()) {
    Serial.println("yoyo");
    Serial.println(_server->getRemoteAddress());
    Serial.println(_server->getRemotePort());

    int ssdplen = _server->getSize();
    // Allocate a byte extra before and after the message, this makes the rest of the code
    // work with fewer tests or code duplication.
    char *buffer = (char *)malloc(ssdplen+2);
    _server->read(buffer+1, ssdplen);  // FIXME return value ?
    buffer[0] = '\r';
    buffer[ssdplen+1] = 0;

    free(buffer);

    Serial.println(buffer); 
  }
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
      upnpSend(udp.remoteIP(), udp.remotePort());
    }
  }
}

void upnpSend(IPAddress ip, int port) {
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

/*
Received 107 bytes from 192.168.0.130, port 50000
UDP packet contents: M-SEARCH * HTTP/1.1
HOST: 239.255.255.250:1900
MAN: "ssdp:discover"
MX: 15
ST: urn:Belkin:device:**
*/


