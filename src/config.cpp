#include "../include/config.h"

Configuration::Configuration(){}

bool Configuration::ReadConfiguration(const String& config_dosya_adi, SPIClass& sd_spi, int sd_cs){
    bool set = false;

    //SD.begin(sd_cs, sd_spi);

    //File config_dosya = SD.open(config_dosya_adi, FILE_READ);

    if ( /*config_dosya*/ true){
        
        //String dosya_verisi = "{\"wifi_id\" : \"TurkTelekom_ZTX6Z7_2.4GHz\",\"wifi_pass\" : \"Asparkardan\",\"mola_beep_sayisi\" : 5,\"mola_beep_ms_uzunlugu\" : 1000,\"personel_sayisi\":2,\"personeller\" : [{\"isim\" : \"Cihan\",\"rf_id_hex\" : \"83 2D 32 FD\"},{\"isim\" : \"Furkan\",\"rf_id_hex\" : \"73 41 42 F7\"}]}";
        String dosya_verisi = "{\"wifi\" : [{\"wifi_id\" : \"TurkTelekom_ZTX6Z7_2.4GHz\",\"wifi_pass\" : \"Asparkardan\"},{\"wifi_id\" : \"M Furkan iPhone'u\", \"wifi_pass\" : \"12345678\"}],\"mola_beep_sayisi\" : 5,\"mola_beep_ms_uzunlugu\" : 1000,\"personeller\" : [{\"isim\" : \"Cihan\",\"rf_id_hex\" : \"83 2D 32 FD\"},{\"isim\" : \"Furkan\",\"rf_id_hex\" : \"73 41 42 F7\"}]}";
        
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

        //personel_sayisi = doc["wifi_sayisi"].as<int>();
        JsonArray wifi = doc["wifi"]; 
        wifi_sayisi = wifi.size();

        wifi_conf = new WifiConfig[wifi_sayisi];

        for (int i = 0;i < wifi_sayisi ; i++){
            strlcpy(wifi_conf[i].wifi_id, wifi[i]["wifi_id"], sizeof(wifi_conf[i].wifi_id)); 
            strlcpy(wifi_conf[i].wifi_pass, wifi[i]["wifi_pass"], sizeof(wifi_conf[i].wifi_pass)); 
        }

        //strlcpy(wifi_id, doc["wifi_id"], sizeof(wifi_id)); 
        //strlcpy(wifi_pass, doc["wifi_pass"], sizeof(wifi_pass)); 

        mola_beep_sayisi = doc["mola_beep_sayisi"].as<int>();
        mola_beep_ms_uzunlugu = doc["mola_beep_ms_uzunlugu"].as<int>();
        //personel_sayisi = doc["personel_sayisi"].as<int>();

        JsonArray elemanlar = doc["personeller"];
        personel_sayisi = elemanlar.size();

        personeller = new Personel[personel_sayisi];
        personeller_girdi = new bool[personel_sayisi];

        for( int i = 0 ; i < personel_sayisi ; i++ ){
            strlcpy(personeller[i].isim, elemanlar[i]["isim"], sizeof(personeller[i].isim));
            strlcpy(personeller[i].rf_id_hex, elemanlar[i]["rf_id_hex"], sizeof(personeller[i].rf_id_hex));
            personeller_girdi[i] = false;
        }

        //config_dosya.close();
        //SD.end();
        for(int i = 0 ; i < wifi_sayisi ; i++){
            Serial.print(" -");
            Serial.printf("%s : %s\n", wifi_conf[i].wifi_id, wifi_conf[i].wifi_pass);
        }
        //Serial.println(wifi_id);
        //Serial.println(wifi_pass);
        Serial.println("Mola beep sayisi : " + mola_beep_sayisi);
        Serial.println("Mola beep uzunlugu : " + mola_beep_ms_uzunlugu);
        Serial.println("Personel sayisi : " + personel_sayisi);
        
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