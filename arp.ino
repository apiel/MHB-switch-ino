#include <lwip/netif.h>
#include <lwip/etharp.h>

unsigned long lastArp = 0;
const unsigned long intervalArp = 10UL*1000UL; // 10sec

void sendGratuitousARP() {
  netif *n = netif_list;
  while (n) {
    etharp_gratuitous(n);
    n = n->next;
  }
}

void arpHandle() {
  if (millis() > lastArp + intervalArp) {
    sendGratuitousARP();
    lastArp = millis();
  }
}

