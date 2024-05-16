#ifndef ESP_GS
#define ESP_GS

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "config.h"
#include "data.h"

class GSCommunications{
    String webAppUrl;
    SDHandler* config;
    std::vector<GSData> gsdata_buffer;
    unsigned long gsdata_buffer_send_interval = 10800000; // 3 saat
    unsigned long last_gsdata_buffer_clear_millis = 0;
    uint8_t gsdata_buffer_send_unsuc_tries = 0;
    bool _send(GSData data);
public:
    GSCommunications(String webAppUrl, SDHandler* conf) : webAppUrl(webAppUrl), config(conf){}

    void send(GSData data);
    bool sendBuffer();

    void checkAndHandleSendBufferInterval();
};

#endif