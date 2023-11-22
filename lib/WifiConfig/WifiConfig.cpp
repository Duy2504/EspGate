#include <WifiConfig.h>

const char *WifiConfig::mqtt_server = "192.168.137.1";

WifiConfig::WifiConfig()
{
}

WifiConfig::~WifiConfig()
{
}

void WifiConfig::smartConfig()
{
    WiFi.disconnect();
    WiFi.mode(WIFI_AP_STA);
    WiFi.beginSmartConfig();
    Serial.println("Waiting for smartConfig!");
    while (!WiFi.smartConfigDone())
    {
        delay(500);
        Serial.print(".");
    }
    WiFi.stopSmartConfig();
    Serial.println("Smartconfig Done!");
    Serial.print("SSID: ");
    ssid  = WiFi.SSID();
    pass  = WiFi.psk();
    Serial.println(ssid);
    Serial.print("Pass: ");
    Serial.println(pass);
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    cEEPROM.write(ssid, 0, 19);
    cEEPROM.write(pass, 20, 39);
    device.ReadyLedOn();
    smConfigMode = true;
}

void WifiConfig::ConnectWifi(String ssid, String pass){
    WiFi.stopSmartConfig();
    WiFi.disconnect();
    WiFi.reconnect();
    Serial.print("Connecting WiFi: ");
    Serial.println(ssid);
    Serial.print("Pass: ");
    Serial.println(pass);
    delay(5);
    WiFi.begin(ssid,pass);
    Serial.print("Connecting ...");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    delay(500);
    Serial.println("");
    Serial.println("WiFi Connection success!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP()); 
}

void WifiConfig::setupWifi()
{
    String ssid = cEEPROM.getWiFissid();
    String pass = cEEPROM.getWiFipassword();
    ConnectWifi(ssid,pass);
}
void WifiConfig::configWifi()
{
    if (device.pressSetupButton2())
    {
        smartConfig();
        Serial.println("Vao che do ConfigWifi !");
        device.ReadyLedToogle();
    }    
}
bool WifiConfig::getSmConfigMode()
{
    return smConfigMode;
}

WifiConfig wifiConfig;
