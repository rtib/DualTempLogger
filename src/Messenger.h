#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <PubSubClient.h>

#define CONNECT_RETRIES 5

class Messenger {
  public:
    Messenger();
    ~Messenger();

    Messenger& setAppName(const char* appName);
    Messenger& setVersion(const char* version);
    Messenger& setClientId(const char* clientId);
    Messenger& setBroker(IPAddress ip, uint16_t port);
    bool initialize();
    void send(const char* topic, const char* payload);
    void heartbeat();
    void loop();
    void disconnect();

  private:
    String appName;
    String version;
    String clientId;
    bool init;
    WiFiClient wifiClient;
    PubSubClient msgClient;

    bool mqReconnect();
};
