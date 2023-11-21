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
const char *mqtt_server = "192.168.1.24";

WiFiClient espClient;
PubSubClient client(espClient);

//========== Handler =================//
xTaskHandle handler_Publish;
xTaskHandle handler_Sub;
xTaskHandle handler_readLora;
bool checkData = false;
#define sub1 "led1"
#define sub2 "led2"
void pubData(void *parameters)
{
  while (true)
  {
    if (checkData)
    {
      DynamicJsonDocument jsonDoc(128);
      jsonDoc["deviceId"] = chipId;
      jsonDoc["temperature"] = temp;
      jsonDoc["humidity"] = humi;
      jsonDoc["light"] = light;
      jsonDoc["soil"] = soil;
      jsonDoc["Rssi"] = Rssi;
      jsonDoc["Pin"] = pin;
      Serial.println();
      String jsonString;
      serializeJson(jsonDoc, jsonString);
      client.publish("sensor", jsonString.c_str());
      Serial.println(jsonString);
      checkData = false;
    }
    // vTaskDelete(NULL);
  }
}

void Task1(void *parameters)
{
  while (true)
  {
    customLoRa.read_Lora();
    if (checkData = false)
    {
      // xTaskCreatePinnedToCore(pubData, "pubData", 4096, NULL, 4, &handler_Publish, 1);
      checkData = true;
      // vTaskResume(handler_Publish);
    }
  }
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
    if ((char)payload[0] == '1')
    {
      Serial.println("on");
    }
    else
    {
      Serial.println("off");
    }
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
void reconnect()
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
      delay(2000);
    }
  }
}

void setup()
{
  Serial.begin(115200);
  DeviceService::config();
  wifiConfig.setupWifi();
  customLoRa.setup_Lora();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  xTaskCreatePinnedToCore(Task1, "task1", 4096, NULL, 1, &handler_readLora, 1);
  xTaskCreatePinnedToCore(pubData, "pubData", 4096, NULL, 1, &handler_Publish, 1);
  vTaskStartScheduler();
}
void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
  // vTaskDelete(NULL);
}
