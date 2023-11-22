#include <Arduino.h>
#include <DeviceService.h>
#include <LoRa.h>
#include <ArduinoJson.h>
#ifndef _CUSTOMLORA_H_
#define _CUSTOMLORA_H_

class CustomLoRa
{
private:
    //========== Define Pin LoRa ==========//
    static const int PIN_LORA_MOSI     = 23;
    static const int PIN_LORA_MISO     = 19;
    static const int PIN_LORA_SCK      = 18;
    static const int PIN_LORA_CS       = 5;
    static const int PIN_LORA_RST      = 15;
    static const int PIN_LORA_DIO0     = 4;
    static const int LORA_FREQUENCY    = 433E6;

public:
    void setup_Lora();
    void tach_String(String data);
    void read_Lora();
    bool checkId(String data, String targetID);
};
extern      bool checkData;
extern      int chipId;
extern      float temp, humi, light, soil, pin, Rssi;
extern CustomLoRa customLoRa;
#endif /* _CUSTOMLORA_H_*/