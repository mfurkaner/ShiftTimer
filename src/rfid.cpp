#include "../include/rfid.h"


RFID::RFID(int rfid_cs, int rfid_rst) : rc522(rfid_cs, rfid_rst){

}

void RFID::setPersonels(Personel* personel, int personel_say){
    bilinen_personeller = personel;
    this->personel_sayisi = personel_say;
}

void RFID::init(Personel* personel, int personel_sayisi){
    
    setPersonels(personel, personel_sayisi);
    
    rc522.PCD_Init();
}


void BuzzerBeep2Kez(void* para){

    digitalWrite(27, HIGH);
    delay(100);
    digitalWrite(27, LOW);
    delay(50); 
    digitalWrite(27, HIGH);
    delay(100);
    digitalWrite(27, LOW);

    vTaskDelete( NULL );
}

void BuzzerBeepUzun(void* para){
    digitalWrite(27, HIGH);
    delay(1000);
    digitalWrite(27, LOW);

    vTaskDelete( NULL );
}

bool RFID::checkRFIDPresence(RFIDinfo& info){
        // Look for new cards
    if ( ! rc522.PICC_IsNewCardPresent()) 
    {
        return false;
    }
    Serial.println("A card is present!");
    // Select one of the cards
    if ( ! rc522.PICC_ReadCardSerial()) 
    {
        return false;
    }
    //Show UID on serial monitor
    Serial.print("RFID found! Tag :");
    String content= "";
    byte letter;
    for (byte i = 0; i < rc522.uid.size; i++) 
    {
        //Serial.print(rc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        //Serial.print(rc522.uid.uidByte[i], HEX);
        content.concat(String(i == 0 ? "" : " ") + String(rc522.uid.uidByte[i] < 0x10 ? "0" : ""));
        content.concat(String(rc522.uid.uidByte[i], HEX));
    }
    content.toUpperCase();
    Serial.println(content);
    info.tag = content;

    for(int i = 0 ; i < personel_sayisi ; i++){
        if(content.equals(String(bilinen_personeller[i].rf_id_hex))){
            Serial.printf("Found the personel! %s : %s\n", bilinen_personeller[i].isim, bilinen_personeller[i].rf_id_hex);
            info.name = bilinen_personeller[i].isim;
            info.known = true;
            break;
        }
    }
    xTaskCreate((info.known ? BuzzerBeep2Kez : BuzzerBeepUzun), "RFIDBeepTask", 5000, NULL, 1, NULL);
    return true;
}
