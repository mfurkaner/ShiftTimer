#include "../include/config.h"

#define GSCOM_WEBAPP_URL "https://script.google.com/macros/s/AKfycbxo2-Qc7fuoEyIuE0Y8nQ-tBbnC6plPaXXvd-0TE5qwkem26V2AC9KAHxC2UbnVtYg7/exec"

SDHandler::SDHandler(){}

bool SDHandler::ReadConfiguration(const String& config_dosya_adi, SPIClass& sd_spi, int sd_cs){
    bool set = false;

    SD.begin(sd_cs, sd_spi);

    File config_dosya = SD.open(config_dosya_adi, FILE_READ);

    if ( config_dosya ){
        sd_open = true;
        String dosya_verisi ;
        
        while (config_dosya.available()) {
            dosya_verisi += config_dosya.readString();
        }
        Serial.println("Read : ");
        Serial.println(dosya_verisi);

        StaticJsonDocument<2048> doc;
        DeserializationError error = deserializeJson(doc, dosya_verisi);
        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return false;
        }
     
        JsonArray wifi = doc["wifi"].as<JsonArray>(); 
        wifi_sayisi = wifi.size();
        Serial.printf("Wifi settings fount to be %d long\n", wifi_sayisi);

        wifi_conf = new WifiConfig[wifi_sayisi];

        for (int i = 0;i < wifi_sayisi ; i++){
            strlcpy(wifi_conf[i].wifi_id, wifi[i]["wifi_id"], sizeof(wifi_conf[i].wifi_id)); 
            strlcpy(wifi_conf[i].wifi_pass, wifi[i]["wifi_pass"], sizeof(wifi_conf[i].wifi_pass)); 
        }

        mola_beep_sayisi = doc["mola_beep_sayisi"].as<int>();
        mola_beep_ms_uzunlugu = doc["mola_beep_ms_uzunlugu"].as<int>();

        if(doc.containsKey("web_app_url")){
            size_t max = sizeof(webapp_url) > sizeof(doc["web_app_url"]) ? sizeof(doc["web_app_url"]) : sizeof(webapp_url);
            
            if ( max == sizeof(webapp_url) )
                Serial.printf("web_app_url size is more than allowed %d! : %d\n", sizeof(webapp_url), sizeof(doc["web_app_url"]));
            strlcpy(webapp_url, doc["webapp_url"], max);
        }
        else{
            strlcpy(webapp_url, GSCOM_WEBAPP_URL, sizeof(GSCOM_WEBAPP_URL));
        }


        JsonArray elemanlar = doc["personeller"];
        personel_sayisi = elemanlar.size();
        Serial.printf("Personel settings found to be %d long\n", personel_sayisi);

        personeller = new Personel[personel_sayisi];
        personeller_girdi = new bool[personel_sayisi];

        for( int i = 0 ; i < personel_sayisi ; i++ ){
            strlcpy(personeller[i].isim, elemanlar[i]["isim"], sizeof(personeller[i].isim));
            strlcpy(personeller[i].rf_id_hex, elemanlar[i]["rf_id_hex"], sizeof(personeller[i].rf_id_hex));
            personeller_girdi[i] = false;
        }

        config_dosya.close();
        for(int i = 0 ; i < wifi_sayisi ; i++){
            Serial.print(" -");
            Serial.printf("%s : %s\n", wifi_conf[i].wifi_id, wifi_conf[i].wifi_pass);
        }

        Serial.flush();
        Serial.printf("Mola beep sayisi : %d\n", mola_beep_sayisi);
        Serial.printf("Mola beep uzunlugu : %d\n", mola_beep_ms_uzunlugu);
        Serial.printf("Personel sayisi : %d\n", personel_sayisi);

        
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


bool SDHandler::LogGSData(const String& log_dosya_adi, const GSData& data){
    if (sd_open){
        String dirname = "/" + data.tarih;
        dirname = dirname.substring(dirname.indexOf('_') + 1);
        if( SD.exists(dirname) == false)
            SD.mkdir(dirname);
        
        dirname += "/" + log_dosya_adi;
        File log;
        if(SD.exists(dirname) == false)
            log = SD.open(dirname, FILE_WRITE);
        else
            log = SD.open(dirname, FILE_APPEND);
        if(log){
            String newline = data.tarih;
            newline.replace('-', '/');
            newline += ", ";

            newline += data.saat;
            newline.replace('-', ':');
            newline += ", ";

            newline += data.isim;
            newline += ", ";

            newline += data.tag;

            log.println(newline);
            log.close();
            Serial.println("Data written to : " + dirname);
            return true;
        }
        log.close();
    }
    return false;
}