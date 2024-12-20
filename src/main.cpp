#include <Arduino.h>
#include <ArduinoJson.h>
#include <SD.h>
#include <WiFi.h>
#include <time.h>

#include "config.h"
#include "display.h"
#include "rfid.h"
#include "gs.h"

//#define DEBUG

#define SD_CS 16
#define RFID_CS 17
#define RFID_RST 21

#define SD_RFID_SCK 18
#define SD_RFID_MISO 19
#define SD_RFID_MOSI 23

#define BUZZER_PIN 27   

#define CONFIG_FILE_NAME "/config.json"
#define LOG_FILE_NAME "log.csv"
const int mola_saat = 17;
const int mola_dk = 17;

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 10800;
const int   daylightOffset_sec = 0;

unsigned long last_update_millis = 0;
const unsigned long update_interval_millis = 86400000; // 24 saat

unsigned long last_online_check_millis = 0;
unsigned long online_check_interval_millis = 30000; // Dakikada bir

unsigned long last_buffer_sent_millis = 0;
unsigned long buffer_send_interval_millis = 30000;

#define GSDATA_NOTSENT_BUFFER_SIZE 10


SDHandler config;
Display display;
RFID rfid(RFID_CS, RFID_RST);
GSCommunications gscom(&config);

bool connectToWifiIndex(int index){
    if(WiFi.status() != WL_CONNECTED){
        if(index < config.wifi_sayisi){
            Serial.printf("Connecting to %s ", config.wifi_conf[index].wifi_id);
            WiFi.begin(config.wifi_conf[index].wifi_id, config.wifi_conf[index].wifi_pass);
            WiFi.mode(WIFI_MODE_STA);
            int trying = 10;
            while (WiFi.status() != WL_CONNECTED && trying > 0) {
                delay(500);
                Serial.print(".");
                trying--;
            }
            if( WiFi.status() != WL_CONNECTED){
                Serial.printf("Cant connect to %s", config.wifi_conf[index].wifi_id);
                WiFi.disconnect(true);
                WiFi.mode(WIFI_OFF);
                delay(1000);
                return false;
            }
            Serial.println(" CONNECTED");
        }
    }
    return true;
}

bool connectToWifiAndStayConnected(int index = -1){
    //connect to WiFi
    if(index != -1)
        if(connectToWifiIndex(index))
            return true;
    
    for(int i = 0; i < config.wifi_sayisi ; i++){
        if(connectToWifiIndex(i) == true)
            return true;
    }
    return false;
}

int checkForWifiIfNotConnected(){
    if(WiFi.status() != WL_CONNECTED ){
        int n = WiFi.scanNetworks();
        for(int i = 0; i < config.wifi_sayisi ; i++){
            for(int j = 0; j < n; j++){
                if(WiFi.SSID(n) == config.wifi_conf[i].wifi_id){
                    if (connectToWifiAndStayConnected(i))
                        return i;
                }
            }
        }
    }
    return -1;
}

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
    connectToWifiAndStayConnected();
    if( WiFi.status() == WL_CONNECTED){
        //init and get the time
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
        printLocalTime();
        last_update_millis = millis();

        Serial.println("Successfully updated the time.");
    }
}

void checkMola(const struct tm& timeinfo){
    bool isWeekDay = (timeinfo.tm_wday % 6) != 0;
    if(isWeekDay && timeinfo.tm_hour == mola_saat && timeinfo.tm_min == mola_dk){
        for(int i = 0; i < config.mola_beep_sayisi; i++){
            digitalWrite(BUZZER_PIN, HIGH);
            delay(config.mola_beep_ms_uzunlugu);
            digitalWrite(BUZZER_PIN, LOW);
            delay(config.mola_beep_ms_uzunlugu/2);
        }
    }
}

void setup() {
  // put your setup code here, to run once:
    Serial.begin(115200); 
    WiFi.mode(WIFI_STA);
    Serial.println("Starting the esp...");
    SPI.begin(SD_RFID_SCK, SD_RFID_MISO, SD_RFID_MOSI);
    display.init();
    delay(1000);

    pinMode(RFID_CS, OUTPUT);
    digitalWrite(RFID_CS, HIGH);
    pinMode(RFID_RST, OUTPUT);      
    pinMode(SD_CS, OUTPUT); 
    pinMode(BUZZER_PIN, OUTPUT); 
    digitalWrite(BUZZER_PIN, LOW);
    delay(500);

    bool read_config = config.ReadConfiguration(CONFIG_FILE_NAME, SPI, SD_CS);
    if(read_config == false) {
        display.drawFail(" SD card fail!");
        delay(5000);
        ESP.restart();
    }

    digitalWrite(SD_CS, HIGH);
    digitalWrite(RFID_CS, LOW);
    //SPI.end();
    updateLocalTime();

    delay(500);

    gscom.initFromConfig();

    rfid.init();
    rfid.configure(config.personeller, config.personel_sayisi);
}

void loop() { 
    // Cihaz açık süresi sayacı max değere ulaştığında (~49 gün) cihazı tekrar başlat
    if( millis() < last_update_millis)
        ESP.restart();
    delay(200);

    // Time - sync
    bool got_time = true;
    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
        Serial.println("Failed to obtain time");
        got_time = false;
    }

    // Wifi sync
    if ( millis() > last_online_check_millis + online_check_interval_millis ){
        if(WiFi.status() != WL_CONNECTED)
            checkForWifiIfNotConnected();
        if(WiFi.status() == WL_CONNECTED)
            Serial.println("WIFI STATUS : CONNECTED");
        last_online_check_millis = millis();
    }

    // 24 saatte bir internete bağlanıp cihaz saatini güncelle
    if ( millis() > last_update_millis + update_interval_millis){
        updateLocalTime();
        last_online_check_millis = millis();
    }


    gscom.checkAndHandleSendBufferInterval();

    // Handle RFID
    RFIDinfo rfidinfo;
    bool isConnected = WiFi.status() == WL_CONNECTED;
    if( rfid.checkRFIDPresence(rfidinfo)){
        static GSData gsdata_buffer[GSDATA_NOTSENT_BUFFER_SIZE];
        static int gsdata_buffer_index = 0;
        display.handleDisplay(timeinfo, isConnected, rfidinfo);

        // Handle google sheets
        GSData gsd(rfidinfo, timeinfo);
        if( got_time ){
            config.LogGSData(LOG_FILE_NAME, gsd);
            gscom.send(gsd);
            last_buffer_sent_millis = millis();
        }
    }
    else{
        display.handleDisplay(timeinfo, isConnected);
        if (isConnected && millis() > last_buffer_sent_millis + buffer_send_interval_millis && got_time){
            gscom.sendBuffer();
            last_buffer_sent_millis = millis();
        }
    }

    
}