#ifndef ESP_DISPLAY
#define ESP_DISPLAY


#include <Arduino.h>
#include <TFT_eSPI.h>
#include "rfid.h"

class Display{
    TFT_eSPI tft = TFT_eSPI();
    bool clearRequired = true;
    struct tm timeinfo;

    char* sirket_adi = "Cihan Gencal Elektrik Ltd Sti";

    void drawDateTime();
    void drawZones();
    void drawRFID(const RFIDinfo& info);
public:

    void init();

    void fill(uint32_t color);
    void fillRed();

    void drawImage();

    void handleDisplay(const struct tm& time);
    void handleDisplay(const struct tm& time, const RFIDinfo& info);
};

#endif