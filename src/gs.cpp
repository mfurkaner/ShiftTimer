#include "../include/gs.h"


void GSCommunications::initFromConfig(){
    webAppUrl = config->webapp_url;
    Serial.printf("GSwebAppUrl set to : %s\n", webAppUrl);
}

bool GSCommunications::_send(GSData data){
    if (WiFi.status() == WL_CONNECTED) {
        // Create a URL for sending or writing data to Google Sheets.
        String Send_Data_URL = webAppUrl + "?";
        Send_Data_URL += "tarih=" + data.tarih;
        Send_Data_URL += "&saat=" + data.saat;
        Send_Data_URL += "&isim=" + data.isim;
        Send_Data_URL += "&tag=" + data.tag;

        Serial.println();
        Serial.println("-------------");
        Serial.println("Send data to Google Spreadsheet...");
        Serial.print("URL : ");
        Serial.println(Send_Data_URL);

        //::::::::::::::::::The process of sending or writing data to Google Sheets.
        // Initialize HTTPClient as "http".
        HTTPClient http;
    
        // HTTP GET Request.
        http.begin(Send_Data_URL.c_str());
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    
        // Gets the HTTP status code.
        int httpCode = http.GET(); 
        Serial.print("HTTP Status Code : ");
        Serial.println(httpCode);

        // Getting response from google sheets.
        String payload;
        if (httpCode > 0) {
            payload = http.getString();
            Serial.println("Payload : " + payload);    
        }
        else{
            http.end();
            return false;
        }
        
        http.end();
        //::::::::::::::::::

        Serial.println("-------------");
        return true;
    }
    return false;
}


void GSCommunications::send(GSData data){
    gsdata_buffer.push_back(data);
    if(WiFi.status() == WL_CONNECTED || gsdata_buffer.size() >= 5){
        
        bool buffer_empty = sendBuffer();
        if ( buffer_empty == false )
            Serial.println("Buffer could not be emptied before sending!");
    
    }
}

bool GSCommunications::sendBuffer(){
    if(gsdata_buffer.size() > 0){

        if(gsdata_buffer_send_unsuc_tries > 5 && gsdata_buffer_send_unsuc_tries <= 10)
            gsdata_buffer_send_interval = 18000000; // 5 saat
        if(gsdata_buffer_send_unsuc_tries > 10)
            gsdata_buffer_send_interval = 43200000; // 12 saat
        
        std::vector<GSData> not_sent;
        if (WiFi.status() != WL_CONNECTED){
            for(int i = 0; i < config->wifi_sayisi ; i++){
                Serial.printf("Connecting to %s ", config->wifi_conf[i].wifi_id);
                WiFi.begin(config->wifi_conf[i].wifi_id, config->wifi_conf[i].wifi_pass);
                WiFi.mode(WIFI_MODE_STA);
                int trying = 5;
                while (WiFi.status() != WL_CONNECTED && trying > 0) {
                    delay(500);
                    Serial.print(".");
                    trying--;
                }
                if( WiFi.status() != WL_CONNECTED){
                    Serial.printf("Cant connect to %s", config->wifi_conf[i].wifi_id);
                    WiFi.disconnect(true);
                    WiFi.mode(WIFI_OFF);
                    delay(1000);
                    continue;
                }
                Serial.println(" CONNECTED");
            }
            if (WiFi.status() != WL_CONNECTED){
                Serial.println(" No connection to send the buffer!");
                return false;
            }
        }

        //send the buffer
        for(int i = 0; i < gsdata_buffer.size(); i++){
            if(_send(gsdata_buffer[i]) == false)
                not_sent.push_back(gsdata_buffer[i]);
            delay(50);
        }
        gsdata_buffer.clear();

        Serial.println("Successfully sent the buffer.");
        
        if ( gsdata_buffer.size() > 0)
            return false;
        if(not_sent.size() > 0){
            for(int i = 0; i < not_sent.size(); i++)
                gsdata_buffer.push_back(not_sent[i]);
            return false;
        }
    }
    last_gsdata_buffer_clear_millis = millis();
    gsdata_buffer_send_interval = 10800000; // 3 saat
    gsdata_buffer_send_unsuc_tries = 0;
    return true;
}

void GSCommunications::checkAndHandleSendBufferInterval(){
    // Give up...
    if ( gsdata_buffer_send_unsuc_tries > 20 )
        return;

    if(millis() > last_gsdata_buffer_clear_millis + gsdata_buffer_send_interval){
        if(sendBuffer() == false){
            gsdata_buffer_send_unsuc_tries++;
        }
    }
}