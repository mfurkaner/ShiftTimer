#ifndef ESP_DISPLAY
#define ESP_DISPLAY


#include <Arduino.h>
#include <TFT_eSPI.h>
#include "rfid.h"

#define DATE_TIME_COLOR TFT_WHITE
#define TEXT_COLOR TFT_BLACK
#define RFID_TAG_COLOR TFT_ORANGE

#define TIME_ZONE_HEIGHT 40
#define TIME_ZONE_WIDTH TFT_WIDTH
#define TIME_ZONE_COLOR TFT_DARKGREY

#define CONTENT_ZONE_MARGIN 20
#define CONTENT_ZONE_HEIGHT (TFT_HEIGHT - TIME_ZONE_HEIGHT - CONTENT_ZONE_MARGIN)
#define CONTENT_ZONE_WIDTH (TFT_WIDTH - CONTENT_ZONE_MARGIN)
#define CONTENT_ZONE_COLOR TFT_LIGHTGREY

class Display{
    TFT_eSPI tft = TFT_eSPI();
    bool clearTimeZone = true;
    bool clearContentZone = true;
    struct tm timeinfo;

    char* sirket_adi = "Aspar OTO";

    void drawTimeZone();
    void drawContentZone();
    void drawWelcomeScreen();
    void drawDateTime(bool isConnected);
    void drawRFID(const struct tm& time, const RFIDinfo& info);

    void clearContentZoneAfter(int ms);
public:

    void init();

    void fill(uint32_t color);
    void fillRed();

    void drawImage();

    void drawBulutGuncelleme(const struct tm& time);
    void handleDisplay(const struct tm& time, bool isConnected);
    void handleDisplay(const struct tm& time, bool isConnected, const RFIDinfo& info);

    void drawFail(std::string message);

    SPIClass getSPI();
};

#endif