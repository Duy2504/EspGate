#include <Arduino.h>
#include <EEPROM.h>

#ifndef _CUSTOMEEPROM_H_
#define _CUSTOMEEPROM_H_
class CustomEEPROM
{
private:
    byte TIME_DELAY = 5;
    byte EEPROM_SIZE = 250;
public:
    CustomEEPROM();
    ~CustomEEPROM();
    void EEPROM_Init();
    void EEPROM_End();
    void write(String data, int beginE, int endE);
    String read(int beginE, int endE);
    void clear(int beginE, int endE);
    String getWiFissid();
    String getWiFipassword();
    bool importConnection(String WiFissid, String WiFipassword);
};

extern CustomEEPROM cEEPROM;
#endif