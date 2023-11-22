#include <DeviceService.h>

void DeviceService::config(){
    pinMode(readyLed, OUTPUT);
    digitalWrite(readyLed, LOW);
    pinMode(buttonConfigWifi, INPUT);
    Serial.begin(115200);
    EEPROM.begin(2048);
    checkInitEEPROM();
    delay(5);
}

void DeviceService::ReadyLedOn(){
    digitalWrite(readyLed,HIGH);
}
void DeviceService::ReadyLedOff(){
    digitalWrite(readyLed,LOW);
}
void DeviceService::ReadyLedToogle(){
    digitalWrite(readyLed,HIGH);
    delay(500);
    digitalWrite(readyLed,LOW);
    delay(500);
}
void DeviceService::checkInitEEPROM(){

}
bool DeviceService::pressSetupButton(){
    if (digitalRead(buttonConfigWifi) == 0)
    {
        return true;
        delay(1000);
    }
    else
    {
        return false;
    }
}

bool DeviceService::pressSetupButton2()
{
  if (digitalRead(buttonConfigWifi) != buttonConfigWifiStatus)
  {
    countPressAction++;
    buttonConfigWifiStatus = !buttonConfigWifiStatus;
  }
  else
  {
    return false;
  }

  if (countPressAction == 2)
  {
    countPressAction = 0;
    return true;
  }
  else
  {
    return false;
  }
}
