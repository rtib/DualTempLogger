#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#include "ServiceDiscovery.h"

ServiceDiscovery::ServiceDiscovery(const char* clientId = "esp8266") {
  Serial.println("\nSeting up mDNS for " + String(clientId) + " ...");
  MDNS.begin(clientId);
  found = selected = 0;
}

void ServiceDiscovery::searchMQTT() {
  Serial.println("\nQuerying mDNS for MQTT...");
  found = MDNS.queryService("mqtt", "tcp");
  Serial.println("Found " + String(found) + " MQTT service(s).");
  if (found == 0) {
    Serial.println("No mDNS service found");
  } else {
    Serial.println("mDNS results: ");
    for (int i = 0; i < found; ++i) {
      Serial.print("  "); Serial.print(i); Serial.print(": ");
      Serial.print(MDNS.hostname(i)); Serial.print(" "); 
      Serial.print(MDNS.IP(i)); Serial.print(":"); Serial.println(MDNS.port(i));
    }
  }
}

int ServiceDiscovery::getSelected()
{
  return selected;
}

void ServiceDiscovery::setSelected(int selected)
{
  if (selected > found) {
    Serial.println("Invalid selection. Resetting to 0.");
    this->selected = 0;
  } else {
    this->selected = selected;
  }
}

IPAddress ServiceDiscovery::getIP() {
  return MDNS.IP(selected);
}

String ServiceDiscovery::getHostname() {
  return MDNS.hostname(selected);
}

uint16_t ServiceDiscovery::getPort() {
  return MDNS.port(selected);
}
