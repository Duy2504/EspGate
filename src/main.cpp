#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <LoRa.h>

// Thư viện Custom
#include <env.h>
#include <DeviceService.h>
#include <CustomEEPROM.h>
#include <WifiConfig.h>
#include <CustomLoRa.h>
const char *mqtt_server = "192.168.1.19";

WiFiClient espClient;
PubSubClient client(espClient);

//========== Handler =================//
xTaskHandle handler_Publish;
xTaskHandle handler_Sub;
xTaskHandle handler_readLora;
//========== TOPIC Control ==========//
#define sub1 "light_state"
#define sub2 "pump_state"
#define sub3 "rem_state"

void MQTT(void *parameters)
{
  while (true)
  {
    while (!client.connected())
    {
      Serial.println("Attempting MQTT connection...");
      String clientId = "ESP32Client-";
      clientId += String(random(0xffff), HEX);

      if (client.connect(clientId.c_str()))
      {
        Serial.println("MQTT connected!");
        client.subscribe(sub1);
        client.subscribe(sub2);
      }
      else
      {
        Serial.println(" try again in 2 seconds");
        Serial.println(mqtt_server);
        delay(2000);
      }
    }
    client.loop();
  }
}
void Pub()
{
  DynamicJsonDocument jsonDoc(128);
  jsonDoc["deviceId"] = chipId;
  jsonDoc["temperature"] = temp;
  jsonDoc["humidity"] = humi;
  jsonDoc["light"] = light;
  jsonDoc["soil"] = soil;
  jsonDoc["Rssi"] = Rssi;
  jsonDoc["Pin"] = pin;
  Serial.println("OK đã đóng gói");
  String jsonString;
  serializeJson(jsonDoc, jsonString);
  client.publish("sensor", jsonString.c_str());
  Serial.println(jsonString);
}
void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  if (strstr(topic, sub1))
  {
    for (int i = 0; i < length; i++)
    {
      Serial.print((char)payload[i]);
    }
    Serial.println();
    // // gửi cả payload cho relay
    // if ((char)payload[0] == '1')
    // {
    //   Serial.println("on");
    // }
    // else
    // {
    //   Serial.println("off");
    // }
  }
  else if (strstr(topic, sub2))
  {
    for (int i = 0; i < length; i++)
    {
      Serial.print((char)payload[i]);
    }
    Serial.println();
    if ((char)payload[0] == '1')
    {
      Serial.println("on");
    }
    else
    {
      Serial.println("off");
    }
  }
}
void Task1(void *parameters)
{
  while (true)
  {
    customLoRa.read_Lora();
    if (checkData)
    {
      Pub();
      checkData = false;
    }
  }
}
// void Sub(void *parameters){
// while (true)
// {
//   callback();
// }

// }



void setup()
{
  Serial.begin(115200);
  DeviceService::config();
  wifiConfig.setupWifi();
  customLoRa.setup_Lora();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  xTaskCreatePinnedToCore(MQTT, "MQTT", 20000, NULL, 2, NULL, 1);
  xTaskCreatePinnedToCore(Task1, "task1", 20000, NULL, 2, &handler_readLora, 1);
}
void loop()
{
}
