#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <LoRa.h>
const char *ssid = "Asus";
const char *password = "12345678";
const char *mqtt_server = "192.168.137.1";
//========== Define Pin LoRa ==========//
#define PIN_LORA_MOSI 23
#define PIN_LORA_MISO 19
#define PIN_LORA_SCK 18
#define PIN_LORA_CS 5
#define PIN_LORA_RST 15
#define PIN_LORA_DIO0 4
#define LORA_FREQUENCY 433E6
//========== Handler =================//
xTaskHandle handler_Publish;
xTaskHandle handler_Sub;
xTaskHandle handler_readLora;
QueueHandle_t dataSoil1 = xQueueCreate(8, sizeof(float));
QueueHandle_t dataSoil2 = xQueueCreate(8, sizeof(float));
QueueHandle_t dataTemp = xQueueCreate(8, sizeof(float));
QueueHandle_t dataHumi = xQueueCreate(8, sizeof(float));

WiFiClient espClient;
PubSubClient client(espClient);

int chipId, statePump, stateFan;
float soil1, soil2, temp, humi, value;

void setup_Lora(){
  Serial.println("LoRa start done !");
  LoRa.setPins(PIN_LORA_CS, PIN_LORA_RST, PIN_LORA_DIO0);
  LoRa.setSPIFrequency(20000000);
  if (!LoRa.begin(LORA_FREQUENCY))
  {
    Serial.println("Starting LoRa faile!");
    while (1)
    {
      ;
    }
  }
    else
    {
      Serial.print("LoRa init with frequency ");
      Serial.println(LORA_FREQUENCY);
    }
}
void publishData(void *parameters)
{
  while (true)
  {
    if (xQueueReceive(dataSoil1, &soil1, 0) == pdTRUE)
		{
			Serial.print("Value Soil 1: ");
			Serial.println(soil1);
		}
		if (xQueueReceive(dataSoil2, &soil2, 0) == pdTRUE)
		{
			Serial.print("Value Soil 2: ");
			Serial.println(soil2);
		}
		if (xQueueReceive(dataTemp, &temp, 0) == pdTRUE)
		{
			Serial.print("Temp: ");
			Serial.println(temp);
		}
		if (xQueueReceive(dataHumi, &humi, 0) == pdTRUE)
		{
			Serial.print("Humi: ");
			Serial.println(humi);
		}
    DynamicJsonDocument jsonDoc(128);
    jsonDoc["Temp"] = temp;
    jsonDoc["Humi"] = humi;
    jsonDoc["Soil1"] = soil1;
    jsonDoc["Soil2"] = soil2;
    String JsonString;
    serializeJson(jsonDoc, JsonString);
    client.publish("sensor", JsonString.c_str());
  }
  vTaskDelete(NULL);
}

void read_String(String data)
{
  // Tìm vị trí của các giá trị ID và Soil Moisture trong chuỗi
  int idPos = data.indexOf("ID: ");
  int soilMoisturePos = data.indexOf("Soil Moisture: ");
  int tempPos = data.indexOf("Temp: ");
  int humiPos = data.indexOf("Humi: ");

  if (idPos != -1 && soilMoisturePos != -1)
  {
    // Tách và chuyển đổi giá trị ID từ chuỗi
    String idStr = data.substring(idPos + 4, soilMoisturePos - 2);
    chipId = idStr.toInt();
    // Tách và chuyển đổi giá trị Soil Moisture từ chuỗi
    String soilMoistureStr = data.substring(soilMoisturePos + 15);
    value = soilMoistureStr.toInt();
  }
  else if (idPos != -1 && tempPos != -1 && humiPos != -1)
  {
    // Tách và chuyển đổi giá trị id từ chuỗi
    String idStr = data.substring(idPos + 4, soilMoisturePos - 2);
    chipId = idStr.toInt();
    // Tách và chuyển đổi giá trị nhiệt độ từ chuỗi
    String tempStr = data.substring(tempPos + 6, humiPos - 2);
    temp = tempStr.toInt();
    // Tách và chuyển đổi giá trị độ ẩm từ chuỗi
    String humiStr = data.substring(humiPos + 6);
    humi = humiStr.toInt();
  }

  if (chipId == 1)
  {
    xQueueSend(dataSoil1, &value, 0);
  }
  else if (chipId == 2)
  {
    xQueueSend(dataSoil2, &value, 0);
  }
  else if (chipId == 0)
  {
    xQueueSend(dataTemp, &temp, 0);
    xQueueSend(dataHumi, &humi, 0);
  }
}

void read_Lora(void *parameters)
{
  while (true)
  {
    int packetSize = LoRa.parsePacket();
    if (packetSize)
    {
      String data = "";
      while (LoRa.available())
      {
        data += (char)LoRa.read();
      }
      Serial.print("Data Receive: ");
      Serial.println(data);
      Serial.print(" with: ");
      Serial.println(LoRa.packetRssi());
      read_String(data);
      xTaskCreatePinnedToCore(publishData, "publishData", 2048, NULL, 4, &handler_Publish, 1);
    }
  }
}

void setup_Wifi()
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

#define sub1 "den"
#define sub2 "pump"
#define sub3 "rem"
int ledState;
int pumpState;
int remState;
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
      ledState = 1;
      Serial.println("Led on");
    }
    else
    {
      ledState = 0;
      Serial.println("Led off");
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
      pumpState = 1;
      Serial.println("Pump on");
    }
    else
    {
      pumpState = 0;
      Serial.println("Pump off");
    }
  }
  else if (strstr(topic, sub3))
  {
    for (int i = 0; i < length; i++)
    {
      Serial.print((char)payload[i]);
    }
    Serial.println();
    if ((char)payload[0] == '1')
    {
      remState = 1;
      Serial.println("Rem on");
    }
    else
    {
      remState = 0;
      Serial.println("Rem off");
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
      Serial.println("Connected mqtt!");
      client.subscribe(sub1);
      client.subscribe(sub2);
      client.subscribe(sub3);
    }
    else
    {
      Serial.println(" try again in 2 seconds");
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(1000);
    }
  }
}

void smartConfig(){
  WiFi.mode(WIFI_AP_STA);
  WiFi.beginSmartConfig();
  Serial.println("Waiting for SmartConfig ");
  while (!WiFi.smartConfigDone())
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("SmartConfig Done");
  setup_Wifi();
}


void setup()
{
  Serial.begin(115200);
  setup_Wifi();
  setup_Lora();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  xTaskCreatePinnedToCore(publishData, "publishData", 2048, NULL, 2, &handler_Publish, 1);
  // xTaskCreatePinnedToCore(subData, "subData", 4096, NULL, 2, &handler_Sub, 1);
  xTaskCreatePinnedToCore(read_Lora, "read_Lora", 4096, NULL, 1, &handler_readLora, 1);
}
void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
}


//===============================//