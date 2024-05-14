#ifndef ESP32_RFID
#define ESP32_RFID

#include <MFRC522.h>
#include "personel.h"

struct RFIDinfo{
    String tag;
    String name;
    bool known = false;
};

class RFID{
    Personel* bilinen_personeller;
    int personel_sayisi = 0;

    MFRC522 rc522;

    void setPersonels(Personel* personel, int personel_sayisi);
public:
    RFID(int rfid_cs, int rfid_rst);

    void init();
    void configure(Personel* personel, int personel_sayisi);

    bool checkRFIDPresence(RFIDinfo& info);

};

//extern int BUZZER_PIN;

#endif