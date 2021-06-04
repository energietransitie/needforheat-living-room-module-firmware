#include "../include/usart.h"
#include "../include/i2c.h"
#include "../include/sleepmodes.h"
#include "../include/wifi.h"
#include "../include/crc.h"
#include "../lib/generic_esp_32/generic_esp_32.h"
#include "../include/timer.h"
#include "../include/util.h"
#include "../include/espnow.h"

#include "../include/scd41.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//#define ESP_NOW_RECEIVER

#ifndef USE_HTTP
void main_esp_now(void)
{
    espnow_init();

    #ifdef ESP_NOW_RECEIVER
        // with any power saving on, ESP-NOW doesn't work well
        // (about every 9 out of 10 packets get lost)
        esp_wifi_set_ps(WIFI_PS_NONE);
    #endif // ESP_NOW_RECEIVER

    while(1)
    {
        set_light_sleep();
    }
}
#else
void main_https(void)
{
    while(1)
    {
            read_timer();
            vTaskDelay(10);
            if(isSending){
                send_HTTPS();
                isSending = false;
            }
    }
}
#endif // USE_HTTP

void app_main() 
{
    isSending = false;
    // ---- PERIPHERALS INITIALIZATION ---- //
    usart_init(115200);

    #ifndef ESP_NOW_RECEIVER
        i2c_init();
        scd41_init();
    #endif // ESP_NOW_RECEIVER

    initialize_wifi();
    init_timer();
    
    #ifndef USE_HTTP
        main_esp_now();
    #else
        main_https();
    #endif // USE_HTTP
}