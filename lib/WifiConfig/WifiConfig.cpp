#include <WifiConfig.h>

const char *mqtt_server = "192.168.1.16";


bool checkSmartConfig = false;
WifiConfig::WifiConfig()
{
}

WifiConfig::~WifiConfig()
{
}
bool checkDone = false;
void WifiConfig::smartConfig()
{
    device.ReadyLedToogle();
    WiFi.disconnect();
    WiFi.mode(WIFI_AP_STA);
    WiFi.beginSmartConfig();
    Serial.println("SmartConfig WiFi begin");
    while (!WiFi.smartConfigDone())
    {
        delay(500);
        Serial.print(".");
    }
    WiFi.stopSmartConfig();
    Serial.println("Smartconfig Done!");
    Serial.print("SSID: ");
    ssid = WiFi.SSID();
    pass = WiFi.psk();
    Serial.println(ssid);
    Serial.print("Pass: ");
    Serial.println(pass);
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    cEEPROM.write(ssid, 0, 19);
    cEEPROM.write(pass, 20, 39);
    device.ReadyLedOn();
    if (WiFi.smartConfigDone())
    {
        checkDone = false;
    }
    ConnectWifi(ssid, pass);
    // client.connect(mqtt_server);
}

void WifiConfig::ConnectWifi(String ssid, String pass)
{
    WiFi.stopSmartConfig();
    WiFi.disconnect();
    WiFi.reconnect();
    Serial.print("Connecting WiFi: ");
    Serial.println(ssid);
    Serial.print("Pass: ");
    Serial.println(pass);
    delay(5);
    WiFi.begin(ssid, pass);
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
    ConnectWifi(ssid, pass);
}

WifiConfig wifiConfig;
