#ifndef ESP_DATA
#define ESP_DATA

#include <Arduino.h>
#include "rfid.h"

struct GSData{
    String tarih;
    String saat;
    String isim;
    String tag;

    GSData(String tarih, String saat, String isim, String tag) : tarih(tarih), saat(saat), isim(isim), tag(tag){
        tag.replace(' ', '_');
        isim.replace(' ', '_');
    }
    GSData(const RFIDinfo& rfid_info, struct tm& timeinfo){
        if (rfid_info.known){
            isim = rfid_info.name;
            isim.replace(' ', '_');
        }
        else{
            isim = "KAYITSIZ_KART";
        }
        tag = rfid_info.tag;
        tag.replace(' ', '_');

        char tar[10];
        char sa[6];
        sprintf(tar,"%02d-%02d-%02d", timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year - 100);
        sprintf(sa,"%02d-%02d", timeinfo.tm_hour, timeinfo.tm_min);

        tarih = tar;
        saat = sa;
    }
};

#endif