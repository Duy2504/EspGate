#include <CustomLoRa.h>

void CustomLoRa::setup_Lora()
{
  SPI.begin(PIN_LORA_SCK, PIN_LORA_MISO, PIN_LORA_MOSI);
  LoRa.setPins(PIN_LORA_CS, PIN_LORA_RST, PIN_LORA_DIO0);
  while (!LoRa.begin(LORA_FREQUENCY))
  {
    Serial.println("Starting LoRa failed!");
    delay(1000);
  }
  LoRa.setSpreadingFactor(LORA_SF);
  LoRa.setCodingRate4(LORA_CR);
  LoRa.setSignalBandwidth(LORA_BW);
  LoRa.setPreambleLength(LORA_PREAMBLE_LENGTH);
  LoRa.enableCrc();
  Serial.println("LoRa started OK!");
  //  LoRa.setSPIFrequency (20000000);
  //  LoRa.setTxPower (18);
  // put the radio into receive mode
  LoRa.receive();
}

void CustomLoRa::lora_rxMode()
{
  LoRa.enableInvertIQ();
  LoRa.receive();
}
void CustomLoRa::lora_txMode()
{
  LoRa.idle();
  LoRa.disableInvertIQ();
}

bool checkData;
int chipId;
float temp, humi, light, soil, pin, Rssi;
void CustomLoRa::tach_String(String data)
{
  // Tìm vị trí của các giá trị
  int idIndex = data.indexOf("ID: ");
  int tempIndex = data.indexOf("Temp: ");
  int humiIndex = data.indexOf("Humi: ");
  int lightIndex = data.indexOf("Light: ");
  int soilIndex = data.indexOf("Soil: ");
  int pinIndex = data.indexOf("Pin: ");

  // Tách chuỗi để lấy các giá trị
  String chipIdS = data.substring(idIndex + 4, tempIndex - 1);
  String tempS = data.substring(tempIndex + 6, humiIndex - 1);
  String humiS = data.substring(humiIndex + 6, lightIndex - 1);
  String lightS = data.substring(lightIndex + 7, soilIndex - 1);
  String soilS = data.substring(soilIndex + 6, pinIndex - 1);
  String pinS = data.substring(pinIndex + 5);

  chipId = chipIdS.toInt();
  temp = tempS.toFloat();
  humi = humiS.toFloat();
  light = lightS.toFloat();
  soil = soilS.toFloat();
  pin = pinS.toFloat();

  // Serial.print("Id: ");
  // Serial.println(chipId);
  // Serial.print("Nhiệt độ: ");
  // Serial.println(temp);
  // Serial.print("Độ ẩm: ");
  // Serial.println(humi);
  // Serial.print("Ánh sáng: ");
  // Serial.println(light);
  // Serial.print("Độ ẩm đất: ");
  // Serial.println(soil);
  // Serial.print("Pin: ");
  // Serial.println(30);
  // Serial.print("Rssi: ");
  // Serial.println(Rssi);
}
bool CustomLoRa::checkId(String data, String targetID)
{
  // Tìm vị trí của ID trong dữ liệu
  int idIndex = data.indexOf("ID: ");
  if (idIndex != -1)
  {
    // Lấy ID từ dữ liệu
    String receivedID = data.substring(idIndex + 4, idIndex + 11);
    // So sánh với giá trị mục tiêu
    return (receivedID == targetID);
  }
  return false; // Không tìm thấy ID trong dữ liệu
}

void CustomLoRa::read_Lora()
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
    Serial.print(data);
    Serial.print(" with: ");
    Serial.println(LoRa.packetRssi());
    Rssi = LoRa.packetRssi();
    // Tách dữ liệu chỉ khi ID là 4402492
    if (checkId(data, "4402492"))
    {
      tach_String(data);
      checkData = true;
    }
    else
    {
      checkData = false;
    }

    // tach_String(data);
    // xTaskCreatePinnedToCore(publishData, "publishData", 2048, NULL, 4, &handler_Publish, 1);
  }
}
int customIdMaster = 6;
void CustomLoRa::sendMessage(String message)
{
  // lora_txMode();
<<<<<<< HEAD
  delay(500);
=======
>>>>>>> 5f61097c6aca1231359f9bda46f379fcf07434b5
  Serial.print("Send control: ");
  LoRa.beginPacket();
  LoRa.print(message);
  Serial.println(message);
  LoRa.endPacket(true);
  // lora_rxMode();
}

CustomLoRa customLoRa;