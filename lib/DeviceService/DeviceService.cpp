#include <DeviceService.h>

Ticker ticker;
void DeviceService::setup()
{
  pinMode(readyLed, OUTPUT);
  digitalWrite(readyLed, HIGH);
  pinMode(buttonWifi, INPUT);
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
  digitalWrite(readyLed, !state);
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
  // Thời gian nhấn giữ được đặt là 5000 miliseconds (5 giây)
  const int timePress = 3000;
  static int lastState = LOW;
  int currentState = digitalRead(buttonWifi);
  static unsigned long pressedTime = 0;
  static unsigned long releasedTime = 0;

  currentState = digitalRead(buttonWifi);

  if (lastState == HIGH && currentState == LOW) // button is pressed
    pressedTime = millis();
  else if (lastState == LOW && currentState == HIGH)
  { // button is released
    releasedTime = millis();

    long pressDuration = releasedTime - pressedTime;

    if (pressDuration >= timePress)
    {
      lastState = currentState;
      return true;
    }
    // Serial.println("A long press is detected");
  }
  // save the the last state
  lastState = currentState;
  return false;
}

DeviceService deviceService;