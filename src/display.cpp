#include "../include/display.h"
#include "earth.cpp" 

void Display::drawWelcomeScreen(){
    tft.fillScreen(TFT_BLACK);

    drawTimeZone();
    drawContentZone();

    // empty date
    tft.setTextSize(2);
    tft.setTextColor(DATE_TIME_COLOR);
    tft.setCursor(10, 15);
    tft.printf("--/--/--");

    // empty time
    tft.setTextSize(3);
    tft.setCursor(135, 10);
    tft.printf("--:--", timeinfo.tm_hour, timeinfo.tm_min);

    // Welcome, connecting message
    tft.setTextSize(3);
    tft.setTextColor(TEXT_COLOR);
    tft.setCursor(20, 120);
    tft.println(sirket_adi);
    tft.setTextSize(2);
    tft.setCursor(20, 160);
    tft.printf("Baglanti kuruluyor...");
}

void Display::drawTimeZone(){
    // Time zone
    tft.fillRoundRect(0,0, TIME_ZONE_WIDTH, TIME_ZONE_HEIGHT, 5, TIME_ZONE_COLOR);
}

void Display::drawContentZone(){
    // Content zone
    tft.fillRoundRect(CONTENT_ZONE_MARGIN/2, TIME_ZONE_HEIGHT + CONTENT_ZONE_MARGIN/2,
        CONTENT_ZONE_WIDTH, CONTENT_ZONE_HEIGHT, 10, CONTENT_ZONE_COLOR);
}

void Display::init(){
    tft.init();

    drawWelcomeScreen();
}


void Display::fill(uint32_t color){
    tft.fillScreen(color);
}

void Display::fillRed(){
    tft.fillScreen(TFT_RED);
}

void Display::drawImage(){
    //tft.drawBitmap(0, 0, Earth_from_Space, 240, 240, TFT_WHITE, TFT_BLACK);
    tft.pushImage(20, 60, 200, 100, aspar_200x100);
}

void Display::drawDateTime(){
    // Tarih
    tft.setTextSize(2);
    tft.setTextColor(DATE_TIME_COLOR);
    tft.setCursor(10, 15);
    tft.printf("%02d/%02d/%02d", timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year - 100);

    // Saat
    tft.setTextSize(3);
    tft.setCursor(135, 10);
    tft.printf("%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
}

void Display::drawRFID(const RFIDinfo& info){
    tft.setTextSize(2);
    tft.setTextColor(TEXT_COLOR);

    if(info.known){
        tft.setTextSize(3);
        tft.setCursor(20, 85);
        tft.println("Hosgeldin");
        tft.setCursor(20, 115);
        tft.printf("%s", info.name);
    }
    else{
        tft.setCursor(20, 85);
        tft.println("Bilinmeyen kart!");
        tft.setCursor(20, 105);
        tft.printf("Tag:");
        tft.setTextColor(RFID_TAG_COLOR);
        tft.printf("%s", info.tag);
        tft.setTextColor(TEXT_COLOR);
    }

}

void Display::handleDisplay(const struct tm& time){
    if (time.tm_min != timeinfo.tm_min || time.tm_hour != timeinfo.tm_hour || time.tm_yday != timeinfo.tm_yday){
        timeinfo = time;
        clearTimeZone = true;
    }
    if (clearTimeZone){
        if (clearContentZone){
            fill(TFT_BLACK);
        }
        drawTimeZone();
        drawDateTime();

        clearTimeZone = false;
    }
    if (clearContentZone){
        drawContentZone();
        drawImage();
        clearContentZone = false;
    }
}

void Display::handleDisplay(const struct tm& time, const RFIDinfo& info){
    fill(TFT_BLACK);
    drawTimeZone();
    drawContentZone();
    drawDateTime();
    drawRFID(info);

    clearContentZoneAfter(2000);
}
struct ClearAfterStruct{
    int ms = 0;
    bool* clearFlag;
};
void clearAfter(void* caStructPtr){
    ClearAfterStruct* cas = (ClearAfterStruct*)caStructPtr;

    delay(cas->ms);
    *(cas->clearFlag) = true;

    delete cas;
    vTaskDelete( NULL );
}


void Display::clearContentZoneAfter(int ms){
    ClearAfterStruct* cas = new ClearAfterStruct();
    cas->clearFlag = &clearContentZone;
    cas->ms = ms;

    xTaskCreate(clearAfter, "ClearAfter", 5000, (void*)cas, 1, NULL);
}


SPIClass Display::getSPI(){
    return tft.getSPIinstance(); 
}