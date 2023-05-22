# DualTempLogger

## Abstract

This is the public part of a private project aiming to log temperatures of fridge and freezer. Based on a ESP8266 on a D1 board, equipped with a LiPo battery shield and two DS18B20 temperature sensor. The data is captured in regular time intervals and sent to a MQTT broker. A Telegraph settup is consuming the sensor messages and store them a InfluxDB database. The data is visualized with Grafana.

## Hardware

* [D1 mini with ESP8266](https://wiki.wemos.cc/products:d1:d1_mini)
* [Battery Shield for lithium batteries for D1 Mini](https://www.az-delivery.de/en/products/d1-mini-pro)
* [Prototyping Shield for D1 Mini NodeMCU ESP8266](https://www.az-delivery.de/en/products/prototyping-shield-fur-d1-mini)
* [DS18B20 1M cable digital stainless steel temperature sensor without heat shrink tube](https://www.az-delivery.de/en/products/1m-kabel-ds18b20-ohne-schrumpfschlauch)
* JST-XH 2.54mm 3-Pin Connector Plug and Socket (but any other 3 pin connector will do)
* [LiPo battery 3.7V 1200mAh with JST-PH plug](https://www.amazon.de/gp/product/B086LKZNP5)

Note: the above links are just examples. You can use any other hardware with the same specs.

## Software

The firmware directly running on the ESP-board is basically doing the following:

* Connect to the WiFi, using [WiFiManager](https://github.com/tzapu/WiFiManager) to configure the WiFi credentials.
* Discovering you network with mDNS/SD to locate MQTT broker.
* Connect to MQTT broker.
* Read temperature from sensors.
* Publish temperature to MQTT broker.
* Disconnecting MQTT and WiFi.
* Going to deep sleep for a configurable time.
* Repeat.

## WiFi

The firmware is using [WiFiManager](https://github.com/tzapu/WiFiManager), which will provide a WiFi access point, if no WiFi credentials are configured. The access point is named `Dual-Temp-Logger/<nnnnnn>` where `<nnnnnn>` is the hexadecimal chip ID of the MCU. Connect to the WiFi access point and configure the WiFi credentials. The WiFi credentials are stored in the ESP8266 flash memory and will be used on the next boot.

## MQTT

You need to run an MQTT broker dicoverabe by mDNS/SD. I'm using [Mosquitto](https://mosquitto.org/) on a Raspberry Pi. The MQTT broker is configured to be accessible from my local WiFi only, thus no authentication is required.

## Service Discovery

The ESP8266 firmware is using mDNS/SD to discover the MQTT broker. The MQTT broker is running on a Raspberry Pi, which is also running a Avahi daemon. The Avahi daemon is configured to announce the MQTT broker via mDNS/SD by adding a service description file. The service description file is located in `/etc/avahi/services/mqtt.service` and looks like this:

```xml:/etc/avahi/services/mqtt.service
<!DOCTYPE service-group SYSTEM "avahi-service.dtd">
<service-group>
 <name replace-wildcards="yes">MQTT on %h</name>
  <service>
   <type>_mqtt._tcp</type>
   <port>1883</port>
  </service>
</service-group>
```

The avahi daemon doesn't need to be restarted after adding the service description file. The service is announced immediately. In order to check the proper accouncement of the service, you can use the `avahi-browse` command:

```bash
% avahi-browse -d local _mqtt._tcp --resolve -t
```

The output should contain the network IP (not just the localhost 127.0.0.1) address and the port of the MQTT broker. Connection to the MQTT broker should be working with the network IP address and the port returned by the the above query. To check, you can use the `mosquitto_sub` command with inserting the IP address and port, e.g.:

```bash
% mosquitto_sub -h <IP address> -p <port> -t '$SYS/broker/version'
```
