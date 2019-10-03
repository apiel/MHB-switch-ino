#include <WiFiUdp.h>

// https://github.com/kakopappa/arduino-esp8266-alexa-multiple-wemo-switch/

IPAddress ipMulti(239, 255, 255, 250);
const unsigned int portMulti = 1900;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];

WiFiUDP UDP;

const unsigned long SSDPtimeout = 3UL*60UL*1000UL + millis(); // 3min

void SSDPhandle(){
  if (millis() < SSDPtimeout) {
    SSDPhandleMulticast();
  }
}

void SSDPhandleMulticast(){
  int packetSize = UDP.parsePacket();
  if (packetSize <= 0)
    return;
  
  IPAddress senderIP = UDP.remoteIP();
  unsigned int senderPort = UDP.remotePort();
  
  // read the packet into the buffer
  UDP.read(packetBuffer, packetSize);
  
  // check if this is a M-SEARCH for WeMo device
  String request = String((char *)packetBuffer);

  if(request.indexOf("M-SEARCH") >= 0) {
      // Issue https://github.com/kakopappa/arduino-esp8266-alexa-multiple-wemo-switch/issues/22 fix
      if((request.indexOf("urn:Belkin:device:**") > 0) || (request.indexOf("ssdp:all") > 0) || (request.indexOf("upnp:rootdevice") > 0)) {
        Serial.println("Got UDP Belkin Request..");
        SSDPsend(senderIP, senderPort);
      }
  }
}

bool SSDPbegin(){
  boolean state = false;
  
  Serial.println("Begin multicast ..");
  
  if(UDP.beginMulticast(WiFi.localIP(), ipMulti, portMulti)) {
    Serial.print("Udp multicast server started at ");
    Serial.print(ipMulti);
    Serial.print(":");
    Serial.println(portMulti);

    state = true;
  }
  else{
    Serial.println("Connection failed");
  }
  
  return state;
}

void SSDPsend(IPAddress ip, int port) {
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
      UDP.beginPacket(ip, port);
      UDP.write(message.c_str());
      UDP.endPacket();
}

