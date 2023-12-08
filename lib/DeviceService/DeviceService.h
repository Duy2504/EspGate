#include <Arduino.h>
#include <EEPROM.h>
#include "Ticker.h"
#ifndef _DEVICESERVICE_H_
#define _DEVICESERVICE_H_
class DeviceService
{
private:
    static const int readyLed = 2;
    static const int buttonWifi = 13;
public:
    static void setup();
    static void ReadyLedOn();
    static void ReadyLedOff();
    static void tick();
    void ReadyLedToogle();
    static bool longPress();
    static void checkInitEEPROM();
};
extern DeviceService deviceService;
#endif /* _DEVICESERVICE_H_*/