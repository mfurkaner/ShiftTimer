#include <Arduino.h>

// put function declarations here:
int myFunction(int, int);

uint16_t id = 0xff;


uint8_t _ESP_START[3] = {'a', 'b', 'c'};
uint8_t _ESP_END[3] = {'c', 'b', 'a'};

enum ESP_CMD : uint8_t{
    ESP_OK_ = 1,
    ESP_ID_REQ = 10,
};

uint8_t buf[256];

int16_t buf_contains_end(){
    for( int i = 0; i < 253; i++ ){
        if ( buf[i] == _ESP_END[0] &&  buf[i+1] == _ESP_END[1]  &&  buf[i+2] == _ESP_END[2] ){
            return i;
        }
    }
    return -1;
}

bool buf_contains_start(){
    for( int i = 0; i < 253; i++ ){
        if ( buf[i] == _ESP_START[0] &&  buf[i+1] == _ESP_START[1]  &&  buf[i+2] == _ESP_START[2] ){
            return true;
        }
    }
    return false;
}

void copy_to_buf(uint8_t new_buf[], int len){
    int buf_index = 0;
    for(int i = 0; i < 256; i){
        buf_index = i;
        if ( buf[i] == 0 ){
            break;
        }
    }

    for(int j = 0; j < len; j++){
        if( j + buf_index > 255  ||  new_buf[j] == 0){
            break;
        }

        buf[buf_index + j] = new_buf[j];
    }
}

void _setup() {
  // put your setup code here, to run once:
    Serial.begin(115200); 
}

void _loop() {
    if (Serial.available()){

        int read_bytes = Serial.readBytes(buf, 256);
        int16_t end_index = 0;

        int timeout_count = 0;
        while(timeout_count < 20){
            if (Serial.available()){
                uint8_t new_buf[256];

                int new_read_bytes = Serial.readBytes(new_buf, 256);

                copy_to_buf(new_buf, new_read_bytes);
                read_bytes += new_read_bytes;
                end_index = buf_contains_end();
                timeout_count = 0;
            }
            else if (end_index == -1){
                break;
            }
            else{
                delay(50);
                timeout_count++;
            }
        }

        //Serial.write(buf, read_bytes);

        if ( read_bytes ){
            int len = buf[0];
            ESP_CMD cmd = ESP_CMD(buf[1]);
            ESP_CMD res_cmd;
            uint8_t res_len = 0;

            switch (cmd){
                case ESP_ID_REQ:
                    res_cmd = ESP_OK_;
                    res_len = 6;
                    Serial.write(res_len);
                    Serial.write(res_cmd);
                    Serial.write(id);
                    Serial.write(_ESP_END, 3);
                    break;
                case ESP_OK_:
                    break;
                default:
                    break;
            }
        }   
    }
    delay(50);
}
