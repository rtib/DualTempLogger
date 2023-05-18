#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "messenger.h"

Messenger::Messenger() {
  msgClient = PubSubClient(wifiClient);
  init = false;
}

Messenger& Messenger::setAppName(const char *appName)
{
  this->appName = appName;
  Serial.println("Messenger.appName set: " + String(appName));
  return *this;
}

Messenger& Messenger::setVersion(const char *version)
{
  this->version = version;
  Serial.println("Messenger.version set: " + String(version));
  return *this;
}

Messenger& Messenger::setClientId(const char *clientId)
{
  this->clientId = clientId;
  Serial.println("Messenger.clientId set: " + String(clientId));
  return *this;
}

Messenger& Messenger::setBroker(IPAddress ip, uint16_t port) {
  this->msgClient.setServer(ip, port);
  Serial.println("Messenger.broker set: " + ip.toString() + ":" + String(port));
  return *this;
}

bool Messenger::initialize() {
  init = true;
  mqReconnect();
  return init;
}

void Messenger::send(const char* topic, const char* payload) {
  if (init) {
    Serial.print("Sending message to "); Serial.print(topic); Serial.print(": "); Serial.println(payload);
    msgClient.publish(topic, payload);
  }
}

void Messenger::heartbeat() {
  Serial.print("Sending heartbeat... ");
  if (init) {
    String topic = clientId + "/heartbeat";
    msgClient.publish(topic.c_str(), "ping");
    msgClient.publish("Dual-Temp-Logger/b6cacc/heartbeat", "ping");
    Serial.println("sent.");
  }
}

void Messenger::loop() {
  msgClient.loop();
}

void Messenger::mqReconnect() {
  while (!msgClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (msgClient.connect("test")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(msgClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
