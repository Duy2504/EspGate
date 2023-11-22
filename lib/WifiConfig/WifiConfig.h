#ifndef _CONFIG_WIFI_H_
#define _CONFIG_WIFI_H_

#include <Arduino.h>
#include <EEPROM.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <CustomEEPROM.h>
#include <DeviceService.h>
class WifiConfig
{
private:
    bool smConfigMode = false;
    String ssid ;
    String pass ;
    static const char *mqtt_server;
    WiFiClient espClient;
    PubSubClient client;
    DeviceService device;
    
public:
    WifiConfig();
    ~WifiConfig();
    void smartConfig();
    void ConnectWifi(String ssid, String pass);
    void setupWifi();
    void configWifi();
    bool getSmConfigMode();
    void reconnect();

};


extern WifiConfig wifiConfig;

#endif /* _CONFIG_WIFI_H_ */