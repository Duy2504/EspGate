#include <Arduino.h>
#include <EEPROM.h>

#ifndef _DEVICESERVICE_H_
#define _DEVICESERVICE_H_
class DeviceService
{
private:
    static const int readyLed = 2;
    static const int buttonConfigWifi = 13;
    bool buttonConfigWifiStatus = true;
    int countPressAction = 0;
    
public:
    static void config();
    static void ReadyLedOn();
    static void ReadyLedOff();
    void ReadyLedToogle();
    static bool pressSetupButton();
    bool pressSetupButton2();
    static void checkInitEEPROM();
};

#endif /* _DEVICESERVICE_H_*/