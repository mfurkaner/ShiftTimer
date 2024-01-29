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

#define CONFIG_FILE_NAME "config.json"


const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 10800;
const int   daylightOffset_sec = 0;

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

    config.ReadConfiguration(CONFIG_FILE_NAME, SPI, SD_CS);

    //connect to WiFi
    Serial.printf("Connecting to %s ", config.wifi_id);
    WiFi.begin(config.wifi_id, config.wifi_pass);
    WiFi.mode(WIFI_MODE_STA);
    int trying = 10;
    while (WiFi.status() != WL_CONNECTED && trying > 0) {
        delay(500);
        Serial.print(".");
        trying--;
    }
    if( WiFi.status() != WL_CONNECTED){
        Serial.printf("Cant connect to %s", config.wifi_id);
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
        delay(1000);

        ESP.restart();
    }
    Serial.println(" CONNECTED");
    //init and get the time
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    printLocalTime();

    //disconnect WiFi as it's no longer needed
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);


    display.init();
    rfid.init(config.personeller, config.personel_sayisi);

    //display.fillRed();
    //delay(2000);
    //display.drawImage();
}

void loop() {
    // Time - sync      
    delay(1000);
    //printLocalTime();
    struct tm timeinfo;
    getLocalTime(&timeinfo);

    // Handle display
    display.handleDisplay(timeinfo);

    // Handle RFID
    RFIDinfo rfidinfo;
    if( rfid.checkRFIDPresence(rfidinfo) ){
        display.handleDisplay(timeinfo, rfidinfo);
    }

}