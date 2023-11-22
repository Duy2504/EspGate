#include <CustomEEPROM.h>

CustomEEPROM::CustomEEPROM(){
    //EEPROM_Init();
}
CustomEEPROM::~CustomEEPROM(){
    //EEPROM_End();
}
void CustomEEPROM::EEPROM_Init(){
    EEPROM.begin(EEPROM_SIZE);
    delay(5);
}
void CustomEEPROM::EEPROM_End(){
    delay(5);
    EEPROM.end();
}

//WRITE
void CustomEEPROM::write(String data, int beginE, int endE){
    int var = data.length() + beginE;
    if (var - 1 > endE)
    {
        Serial.println("Thiếu dung lượng EEPROM !");
    }
    clear(beginE, endE);
    
    for (int i = beginE; i < endE; i++)
    {
        EEPROM.write(i, data[i - beginE]);
        delay(TIME_DELAY);
    }
    if (EEPROM.commit())
    {
        Serial.println("\n Ghi EEPROM thành công!");
    }
    else
    {
        Serial.println("Ghi EEPROM thất bại!");
    }
}
String CustomEEPROM::read(int beginE, int endE){
    String data;
    char c;
    for(int i = beginE; i <= endE; i++){
        c = (char)EEPROM.read(i);
        if (c != 0)
        {
            data += c;
        }
        delay(TIME_DELAY);
    }
    return data;
}
void CustomEEPROM::clear(int beginE, int endE){
    for (int i = beginE; i <= endE; i++){
        EEPROM.write(i,0);
        delay(TIME_DELAY);
    }
}

bool CustomEEPROM::importConnection(String WiFissid, String WiFipassword)
{
    write(WiFissid, 0, 19);
    write(WiFipassword, 20, 39);
    Serial.print("SSID: ");
    Serial.println(WiFissid);
    Serial.print("Password: ");
    Serial.println(WiFipassword);
    return true;
}
String CustomEEPROM::getWiFissid(){
    String data = read(0, 19);
    data.trim();
    return data;
}
String CustomEEPROM::getWiFipassword(){
    String data = read(20, 39);
    data.trim();
    return data;
}

CustomEEPROM cEEPROM;