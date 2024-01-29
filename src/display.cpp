#include "../include/display.h"
#include "earth.cpp" 

void Display::drawZones(){
    //tft.drawLine(0, 40, TFT_WIDTH, 40, TFT_WHITE);
    tft.fillRoundRect(0,0, TFT_WIDTH, 40, 5, TFT_DARKGREY);
}

void Display::init(){
    tft.init();
}


void Display::fill(uint32_t color){
    tft.fillScreen(color);
}

void Display::fillRed(){
    tft.fillScreen(TFT_RED);
}

void Display::drawImage(){
    //tft.drawBitmap(0, 0, Earth_from_Space, 240, 240, TFT_WHITE, TFT_BLACK);
    tft.pushImage(20, 60, 200, 100, cgElektrik);
}

void Display::drawDateTime(){
    // Tarih
    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(10, 15);
    tft.printf("%02d/%02d/%02d", timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year - 100);

    // Saat
    tft.setTextSize(3);
    tft.setCursor(135, 10);
    tft.printf("%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
}

void Display::drawRFID(const RFIDinfo& info){
    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE);

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
        tft.setTextColor(TFT_YELLOW);
        tft.printf("%s", info.tag);
        tft.setTextColor(TFT_WHITE);
    }

}

void Display::handleDisplay(const struct tm& time){
    if (time.tm_min != timeinfo.tm_min || time.tm_hour != timeinfo.tm_hour || time.tm_yday != timeinfo.tm_yday){
        timeinfo = time;
        clearRequired = true;
    }
    if (clearRequired){
        fill(TFT_BLACK);
        clearRequired = false;
    }
    drawZones();
    drawDateTime();
    drawImage();
}


void Display::handleDisplay(const struct tm& time, const RFIDinfo& info){
    fill(TFT_BLACK);
    drawZones();
    drawDateTime();
    drawRFID(info);
    delay(2000);
    clearRequired = true;
}