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
    //========== LoRa signals ===========//
    static const int LORA_SF           = 7; // 7 Fastest or 12 fartest
    int LORA_CR                        = 5;
    int LORA_BW                        = 41.7E3; //31.25E3 // 41.7E3 //125E3
    int LORA_PREAMBLE_LENGTH           = 8; 

public:
    void setup_Lora();
    void lora_rxMode();
    void lora_txMode();
    void tach_String(String data);
    void read_Lora();
    void sendMessage(String message);
    bool checkId(String data, String targetID);
};
extern      int customIdMaster;
extern      bool checkData;
extern      int chipId;
extern      float temp, humi, light, soil, pin, Rssi;
extern CustomLoRa customLoRa;
#endif /* _CUSTOMLORA_H_*/