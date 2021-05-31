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

#define ESP_NOW_RECEIVER

void app_main() 
{
    isSending = false;
    // ---- PERIPHERALS INITIALIZATION ---- //
    usart_init(115200);
    i2c_init();
    scd41_init();
    initialize_wifi();
    init_timer();
    
    #ifndef USE_HTTP
        espnow_init();

        #ifdef ESP_NOW_RECEIVER
            // with any power saving on, ESP-NOW doesn't work well
            // (about every 9 out of 10 packets get lost)
            esp_wifi_set_ps(WIFI_PS_NONE);
        #endif // ESP_NOW_RECEIVER
    #endif // USE_HTTP
    
    while(1)
    {
        // ---- MEASURE CO2, TEMP, RHT ---- // 
        //scd41_measure_co2_temp_rht();

        // ---- SEND DATA VIA HTTPS ---- //
        #ifdef USE_HTTP
            read_timer();
            vTaskDelay(10);
            if(isSending){
                send_HTTPS();
                isSending = false;
            }
       #else
            #ifndef ESP_NOW_RECEIVER
                uint32_t data = 0;
                espnow_send(&data, sizeof(uint32_t));
            #endif // ESP_NOW_RECEIVER
        #endif // USE_HTTP

        delay(5000);
    }
}