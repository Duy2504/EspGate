#include <DeviceService.h>

Ticker ticker;
void DeviceService::setup()
{
  pinMode(readyLed, OUTPUT);
  digitalWrite(readyLed, HIGH);
  pinMode(buttonWifi, INPUT_PULLUP);
  Serial.begin(115200);
  EEPROM.begin(2048);
  checkInitEEPROM();
  delay(5);
}

void DeviceService::ReadyLedOn()
{
  digitalWrite(readyLed, HIGH);
  ticker.detach();
}
void DeviceService::ReadyLedOff()
{
  digitalWrite(readyLed, LOW);
  ticker.detach();
}
void DeviceService::tick()
{
  int state = digitalRead(readyLed);
  digitalWrite(readyLed , !state);
}
void DeviceService::ReadyLedToogle()
{
  ticker.attach(1, tick);
}
void DeviceService::checkInitEEPROM()
{
}

bool DeviceService::longPress()
{
  static int lastPress = 0;
  if(millis() - lastPress >= 3000 && digitalRead(buttonWifi) == 0)
  {
    return true;
  }
  else if(digitalRead(buttonWifi) == 1){
    lastPress == millis();
  }
  return false;
}

DeviceService deviceService;