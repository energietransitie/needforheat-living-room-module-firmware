#include "../include/usart.h"
#include "../include/i2c.h"
#include "../include/sleepmodes.h"
#include "../include/wifi.h"
#include "../include/crc.h"
#include "../lib/generic_esp_32/generic_esp_32.h"
#include "../include/timer.h"

#include "../include/scd41.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void app_main() 
{
    isSending = false;
    // ---- PERIPHERALS INITIALIZATION ---- //
    usart_init(115200);
    i2c_init();
    scd41_init();
    initialize_wifi();
    init_timer();
    
    while(1)
    {
        // printf("going into light sleep!\n");
        // set_light_sleep();
        // printf("going out of light sleep!\n");
        // printf("going into modem sleep!\n");
        // set_modem_sleep();
        // // ---- MEASURE CO2, TEMP, RHT ---- // 
        // printf("start measurement!\n");
        // scd41_measure_co2_temp_rht();
        // printf("going out of modem sleep!\n");
        // wake_modem_sleep();
        // ---- SEND DATA VIA HTTPS ---- //
        vTaskDelay(10);
        if(isSending){
            printf("start sending\n");
            send_HTTPS();
            isSending = false;
        }
        vTaskDelay(1000);
    }
}