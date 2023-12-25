#include <version.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ESP8266mDNS.h>

#include "ServiceDiscovery.h"
#include "messenger.h"

#define APP_NAME                "Dual-Temp-Logger"
#define ONE_WIRE_BUS            4
#define SENSOR_POWER_PIN        5
#define SLEEP_DURATION          30e6

ADC_MODE(ADC_VCC);

OneWire oneWire;
DallasTemperature sensor;
Messenger messenger;
WiFiManager wifiManager;
int numberOfDevices;
String sensorsPrefix;

String hexAddress(DeviceAddress deviceAddress) {
  String hex = "";
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) hex += "0";
    hex += String(deviceAddress[i], HEX);
  }
  return hex;
}

void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

void shutdown() {
  Serial.println("\nDisconnecting MQTT...");
  messenger.disconnect();

  Serial.println("\nShutting down WiFi...");
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);

  Serial.println("\nPowering down sensors.");
  digitalWrite(SENSOR_POWER_PIN, LOW);

  Serial.println("\nGoing to sleep...");
  ESP.deepSleep(SLEEP_DURATION);
}

void setup()
{
  Serial.begin(9600);
  Serial.print("\n\n");
  Serial.print(F(APP_NAME)); Serial.print(F(" ")); Serial.println(F(VERSION));

  String clientId = String(APP_NAME) + "/" + String(ESP.getChipId(), HEX);
  Serial.print("Client ID: "); Serial.println(clientId);

  sensorsPrefix = String("sensors/") + String(ESP.getChipId(), HEX) + "/";
  Serial.print("Sensors prefix: "); Serial.println(sensorsPrefix);

  Serial.println("\nPowering up sensors.");
  pinMode(SENSOR_POWER_PIN, OUTPUT);
  digitalWrite(SENSOR_POWER_PIN, HIGH);
  delay(1000);

  Serial.println("\nStarting up WiFi Manager...");
  wifiManager.autoConnect(clientId.c_str());
  if(WiFi.isConnected()) {
    Serial.print("WiFi connected with ");
    Serial.print("SSID: "); Serial.print(WiFi.SSID());
    Serial.print(", IP address: "); Serial.println(WiFi.localIP()); 
  } else {
    Serial.println("\nWiFi not connected.");
    shutdown();
  }

  ServiceDiscovery sd = ServiceDiscovery(clientId.c_str());
  sd.searchMQTT();

  messenger.setVersion(VERSION)
    .setClientId(clientId.c_str())
    .setAppName(APP_NAME)
    .setBroker(sd.getIP(), sd.getPort());
  if (!messenger.initialize()) {
    shutdown();
  }

  messenger.heartbeat();

  Serial.print("\nVcc: "); Serial.println(ESP.getVcc());
  messenger.send((sensorsPrefix + "Vcc").c_str(), String(ESP.getVcc()).c_str());

  Serial.println("\nPreparing measurement...");
  oneWire = OneWire(ONE_WIRE_BUS);
  sensor = DallasTemperature(&oneWire);

  sensor.begin();
  numberOfDevices = sensor.getDeviceCount();
  Serial.print("Found "); Serial.print(numberOfDevices); Serial.println(" devices.");
  sensor.setResolution(12);

  sensor.requestTemperatures();
  while (!sensor.isConversionComplete());  // wait until sensor is ready

  Serial.println("\nMeasuring...");
  for (int i = 0; i < numberOfDevices; i++)
  {
    DeviceAddress address;
    float tempC = sensor.getTempCByIndex(i);
    Serial.print("Device ");
    Serial.print(i);
    Serial.print(" - ");
    if (sensor.getAddress(address, i)) {
      printAddress(address);
      Serial.print(" - ");
    }
    Serial.print(tempC);
    Serial.println(" C");
    String topic = sensorsPrefix + "temperature/" + hexAddress(address);
    messenger.send(topic.c_str(), String(tempC).c_str());
  }

  Serial.println("Shutting down...");
  shutdown();
}

void loop()
{
  shutdown();
}
