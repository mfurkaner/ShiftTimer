#include <Arduino.h>
#include <ArduinoJson.h>
#include <SD.h>
#include <WiFi.h>
#include <time.h>

#include "config.h"
#include "display.h"
#include "rfid.h"

#define SD_CS 16
#define RFID_CS 17
#define RFID_RST 21

#define SD_RFID_SCK 18
#define SD_RFID_MISO 19
#define SD_RFID_MOSI 23

#define CONFIG_FILE_NAME "/config.json"

const int mola_saat = 11;
const int mola_dk = 31;

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 10800;
const int   daylightOffset_sec = 0;

unsigned long last_update_millis = 0;
const unsigned long update_interval_millis = 86400000; // 24 saat

Configuration config;
Display display;
RFID rfid(RFID_CS, RFID_RST);

void printLocalTime()
{
    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
        Serial.println("Failed to obtain time");
        return;
    }
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void updateLocalTime(){
    //connect to WiFi
    for(int i = 0; i < config.wifi_sayisi ; i++){
        Serial.printf("Connecting to %s ", config.wifi_conf[i].wifi_id);
        WiFi.begin(config.wifi_conf[i].wifi_id, config.wifi_conf[i].wifi_pass);
        WiFi.mode(WIFI_MODE_STA);
        int trying = 10;
        while (WiFi.status() != WL_CONNECTED && trying > 0) {
            delay(500);
            Serial.print(".");
            trying--;
        }
        if( WiFi.status() != WL_CONNECTED){
            Serial.printf("Cant connect to %s", config.wifi_conf[i].wifi_id);
            WiFi.disconnect(true);
            WiFi.mode(WIFI_OFF);
            delay(1000);

            continue;
        }
        Serial.println(" CONNECTED");
        //init and get the time
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
        printLocalTime();
        last_update_millis = millis();

        //disconnect WiFi as it's no longer needed
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);

        Serial.println("Successfully updated the time.");
        return;
    }
    
}

void checkMola(const struct tm& timeinfo){
    bool isWeekDay = (timeinfo.tm_wday % 6) != 0;
    if(isWeekDay && timeinfo.tm_hour == mola_saat && timeinfo.tm_min == mola_dk){
        for(int i = 0; i < config.mola_beep_sayisi; i++){
            digitalWrite(27, HIGH);
            delay(config.mola_beep_ms_uzunlugu);
            digitalWrite(27, LOW);
            delay(config.mola_beep_ms_uzunlugu/2);
        }
    }
}

void setup() {
  // put your setup code here, to run once:
    Serial.begin(115200); 
    Serial.println("Starting the esp...");
    delay(1000);


    SPI.begin(SD_RFID_SCK, SD_RFID_MISO, SD_RFID_MOSI, SD_CS);
    pinMode(RFID_CS, OUTPUT);
    pinMode(RFID_RST, OUTPUT);      
    pinMode(SD_CS, OUTPUT); 
    pinMode(27, OUTPUT); 
    digitalWrite(27, LOW);
    delay(500);
    
    config.ReadConfiguration(CONFIG_FILE_NAME, SPI, SD_CS);
    //SPI.end();
    updateLocalTime();

    delay(500);

    display.init();
    rfid.init(config.personeller, config.personel_sayisi);

}

void loop() { 
    delay(500);
    // Time - sync
    struct tm timeinfo;
    getLocalTime(&timeinfo);
    checkMola(timeinfo);
    
    // 24 saatte bir internete bağlanıp cihaz saatini güncelle
    if ( millis() > last_update_millis + update_interval_millis){
        updateLocalTime();

    }
    // Cihaz açık süresi sayacı max değere ulaştığında (~49 gün) cihazı tekrar başlat
    else if( millis() < last_update_millis){
        ESP.restart();
    }

    // Handle RFID
    RFIDinfo rfidinfo;
    if( rfid.checkRFIDPresence(rfidinfo) ){
        display.handleDisplay(timeinfo, rfidinfo);

        // Handle file
    }
    else{
        display.handleDisplay(timeinfo);
    }

}