#include "../include/config.h"

Configuration::Configuration(){}

bool Configuration::ReadConfiguration(const String& config_dosya_adi, SPIClass& sd_spi, int sd_cs){
    bool set = false;

    //SD.begin(sd_cs, sd_spi);

    //File config_dosya = SD.open(config_dosya_adi, FILE_READ);

    if ( /*config_dosya*/ true){
        
        String dosya_verisi = "{\"wifi_id\" : \"FiberHGW_TPDDFE\",\"wifi_pass\" : \"dUwterdVqxE9\",\"mola_beep_sayisi\" : 5,\"mola_beep_ms_uzunlugu\" : 1000,\"personel_sayisi\":2,\"personeller\" : [{\"isim\" : \"Furkan\",\"rf_id_hex\" : \"83 2D 32 FD\"},{\"isim\" : \"Zeynep\",\"rf_id_hex\" : \"73 41 42 F7\"}]}";
        /*
        while (config_dosya.available()) {
            dosya_verisi += config_dosya.read();
        }*/

        StaticJsonDocument<1024> doc;
        DeserializationError error = deserializeJson(doc, dosya_verisi);
        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return false;
        }
        else{
            Serial.println("Read the config file as :");
            Serial.println(dosya_verisi);
        }

        strlcpy(wifi_id, doc["wifi_id"], sizeof(wifi_id)); 
        strlcpy(wifi_pass, doc["wifi_pass"], sizeof(wifi_pass)); 

        mola_beep_sayisi = doc["mola_beep_sayisi"].as<int>();
        mola_beep_ms_uzunlugu = doc["mola_beep_ms_uzunlugu"].as<int>();
        personel_sayisi = doc["personel_sayisi"].as<int>();

        JsonArray elemanlar = doc["personeller"];

        personeller = new Personel[personel_sayisi];

        for( int i = 0 ; i < personel_sayisi ; i++ ){
            strlcpy(personeller[i].isim, elemanlar[i]["isim"], sizeof(personeller[i].isim));
            strlcpy(personeller[i].rf_id_hex, elemanlar[i]["rf_id_hex"], sizeof(personeller[i].rf_id_hex));
        }

        //config_dosya.close();

        Serial.println(wifi_id);
        Serial.println(wifi_pass);
        Serial.println(mola_beep_sayisi);
        Serial.println(mola_beep_ms_uzunlugu);
        Serial.println(personel_sayisi);
        
        for(int i = 0 ; i < personel_sayisi ; i++){
            Serial.print(" -");
            Serial.printf("%s : %s\n", personeller[i].isim, personeller[i].rf_id_hex);
        }
        set = true;
    }
    else{
        Serial.print(config_dosya_adi);
        Serial.println(" bulunamadi!");
    }

    return set;
}