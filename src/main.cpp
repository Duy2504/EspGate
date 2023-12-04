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
// const char *mqtt_server = "192.168.137.1";

WiFiClient espClient;
PubSubClient client(espClient);

//========== Handler =================//
xTaskHandle handler_MQTT;
xTaskHandle handler_readSensor;
xTaskHandle handler_smartConfig;
QueueHandle_t messageControl = xQueueCreate(27, sizeof(String));
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
        client.subscribe(sub3);
      }
      else
      {
        Serial.println(" try again in 2 seconds");
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
  // Serial.println("OK đã đóng gói");
  String jsonString;
  serializeJson(jsonDoc, jsonString);
  client.publish("sensor", jsonString.c_str());
  // Serial.println(jsonString);
}

void callback(char *topic, byte *payload, unsigned int length)
{
  String subData = "";
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  if (strstr(topic, sub1))
  {
    for (int i = 0; i < length; i++)
    {
      subData += (char)payload[i];
    }
    Serial.println(subData);
  }
  else if (strstr(topic, sub2))
  {
    for (int i = 0; i < length; i++)
    {
      subData += (char)payload[i];
    }
    Serial.println(subData);
  }
  else if (strstr(topic, sub3))
  {
    for (int i = 0; i < length; i++)
    {
      subData += (char)payload[i];
    }
    Serial.println(subData);
  }
  String m = "ID: " + String(customIdMaster) + ", Topic: " + String(topic) + ", Message: " + String(subData);
  xQueueSend(messageControl, &m, portMAX_DELAY);
}
void sendRelay(void *parmaeters)
{
  TickType_t xLastWakeTime;
  const TickType_t xFrequency = pdMS_TO_TICKS(1000);
  xLastWakeTime = xTaskGetTickCount();
  String Relay;
  while (true)
  {
    if (xQueueReceive(messageControl, &Relay, portMAX_DELAY) == pdTRUE)
    {
      customLoRa.sendMessage(Relay);
      // delay(1000);
    }
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}

void readSensor(void *parameters)
{
  TickType_t xLastWakeTime;
  const TickType_t xFrequency = pdMS_TO_TICKS(500);
  xLastWakeTime = xTaskGetTickCount();
  while (true)
  {
    customLoRa.read_Lora();
    if (checkData)
    {
      Pub();
      checkData = false;
    }
    // vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}
void smartWifi(void *parameters)
{
  if (checkSmartConfig)
  {
    wifiConfig.smartConfig();
  }
  if (checkDone)
  {
    vTaskDelete(handler_smartConfig);
  }
}
void checkButton(void *parameters)
{
  while (true)
  {
    if (deviceService.longPress())
    {
      xTaskCreatePinnedToCore(smartWifi, "smartWifi", 20000, NULL, 4, &handler_smartConfig, ARDUINO_RUNNING_CORE);
    }
  }
}
void setup()
{
  Serial.begin(115200);
  deviceService.setup();
  wifiConfig.setupWifi();
  customLoRa.setup_Lora();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  xTaskCreatePinnedToCore(MQTT, "MQTT", 20000, NULL, 2, &handler_MQTT, ARDUINO_RUNNING_CORE);
  // xTaskCreatePinnedToCore(checkButton, "checkButton", 20000, NULL, 2, NULL, ARDUINO_RUNNING_CORE);
  xTaskCreatePinnedToCore(sendRelay, "sendRelay", 20000, NULL, 2, NULL, ARDUINO_RUNNING_CORE);
  xTaskCreatePinnedToCore(readSensor, "readSensor", 20000, NULL, 2, &handler_readSensor, ARDUINO_RUNNING_CORE);

  vTaskStartScheduler();
}
void loop()
{
}
