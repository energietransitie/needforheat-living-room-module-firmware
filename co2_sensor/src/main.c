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
        // ---- MEASURE CO2, TEMP, RHT ---- // 
        scd41_measure_co2_temp_rht();

        // ---- SEND DATA VIA HTTPS ---- //
        read_timer();
        vTaskDelay(10);
        if(isSending){
            send_HTTPS();
            isSending = false;
        }
    }
}