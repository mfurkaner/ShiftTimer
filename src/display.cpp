#include "../include/display.h"
#include "aspar.h" 

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
    tft.setCursor(20, 100);
    tft.print(" ");
    tft.println(sirket_adi);
    tft.setTextSize(2);
    tft.setCursor(20, 160);
    tft.printf(" Baglaniliyor...");
}

void Display::drawFail(std::string message){
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
    tft.setCursor(20, 100);
    tft.print(" ");
    tft.println(sirket_adi);
    tft.setTextSize(2);
    tft.setCursor(20, 160);
    tft.printf(message.c_str());
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
    tft.setSwapBytes(true);
    /*
    TFT_eSprite sprite = TFT_eSprite(&tft);
    sprite.createSprite(200,100);
    sprite.pushImage(0,0,200,100, aspar_200x100);
    sprite.pushSprite(20, 60);*/
    tft.pushImage((CONTENT_ZONE_WIDTH + CONTENT_ZONE_MARGIN - ASPAR_200X100_WIDTH)/2,
                  (CONTENT_ZONE_HEIGHT + CONTENT_ZONE_MARGIN - ASPAR_200X100_HEIGHT)/2 + TIME_ZONE_HEIGHT, 
                  ASPAR_200X100_WIDTH, ASPAR_200X100_HEIGHT, aspar_200x100);
    tft.setSwapBytes(false);
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


#define RFID_SCREEN_TIME_UPPER_MARGIN 15
#define RFID_SCREEN_TIME_NAME_MARGIN 10
#define RFID_SCREEN_TIME_Y TIME_ZONE_HEIGHT + CONTENT_ZONE_MARGIN + RFID_SCREEN_TIME_UPPER_MARGIN

void Display::drawRFID(const struct tm& time, const RFIDinfo& info){
    tft.setTextColor(TEXT_COLOR);
    char saat[8];
    sprintf(saat, "  %02d:%02d", time.tm_hour, time.tm_min);
    tft.setTextSize(4);
    tft.setCursor(CONTENT_ZONE_MARGIN, RFID_SCREEN_TIME_Y);
    tft.println(saat);
    tft.setTextSize(3);
    if(info.known){
        if(info.name.length() > 12){
            String name = info.name;
            name.replace(' ', '\n');
            int size_syl = 0;
            int syl_count = 1;
            bool smaller = false;
            for(int i = 0; i < name.length() ; i++){
                if(name[i] == '\n'){
                    if (size_syl > 12){
                        smaller = true;
                        break;
                    }
                    size_syl = 0;
                    syl_count++;
                }
                else
                    size_syl++;
            }
            if(smaller)
                tft.setTextSize(2);
            
            int ind = 0;
            int cursor_y = RFID_SCREEN_TIME_Y + 40 + RFID_SCREEN_TIME_NAME_MARGIN;
            for(int i = 0; i < syl_count ; i++){
                tft.setCursor(CONTENT_ZONE_MARGIN, cursor_y);
                for(; ind < name.length(); ind++){
                    if(name[ind] == '\n')
                        break;
                    tft.print(name[ind]);
                }
                ind++;
                cursor_y += (smaller ? 20 : 30);
            }
        }
        else{
            tft.setCursor(CONTENT_ZONE_MARGIN, RFID_SCREEN_TIME_Y + 40 + RFID_SCREEN_TIME_NAME_MARGIN);
            tft.printf("%s", info.name);
        }
            
    }
    else{
        tft.setTextSize(2);
        tft.setCursor(CONTENT_ZONE_MARGIN, RFID_SCREEN_TIME_Y + 40 + RFID_SCREEN_TIME_NAME_MARGIN);
        tft.println("Bilinmeyen kart!");
        tft.setCursor(CONTENT_ZONE_MARGIN, RFID_SCREEN_TIME_Y + 40 + RFID_SCREEN_TIME_NAME_MARGIN + 40);
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
        Serial.println("Time changed on display");
    }
    if (clearContentZone){
        fill(TFT_BLACK);
        drawContentZone();
        drawImage();
        clearContentZone = false;
        clearTimeZone = true;
    }
    if (clearTimeZone){
        drawTimeZone();
        drawDateTime();
        clearTimeZone = false;
    }

}

void Display::handleDisplay(const struct tm& time, const RFIDinfo& info){
    if (time.tm_min != timeinfo.tm_min || time.tm_hour != timeinfo.tm_hour || time.tm_yday != timeinfo.tm_yday){
        timeinfo = time;
    }
    clearContentZone = false;
    fill(TFT_BLACK);
    drawTimeZone();
    drawDateTime();
    drawContentZone();
    drawRFID(time, info);

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