#ifndef ESP_CONFIG
#define ESP_CONFIG

#include <Arduino.h>
#include <SD.h>
#include <ArduinoJson.h>
#include "personel.h"

struct WifiConfig{
    char wifi_id[33];
    char wifi_pass[33];
};

struct Configuration{
    WifiConfig* wifi_conf;
    int wifi_sayisi;
    
    int mola_beep_sayisi;
    int mola_beep_ms_uzunlugu;

    int personel_sayisi;
    Personel* personeller;
    bool* personeller_girdi;

    Configuration();

    bool ReadConfiguration(const String& config_dosya_adi, SPIClass& sd_spi, int sd_cs);
};

#endif