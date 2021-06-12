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

// TODO:
// - remove unnecessary functions (last to be done)
// - improve scd41 readings (comment floats and divisions)
// - instead of delay(1350), custom_lightsleep(1350 * 1000) watch wifi_stop and begin
// - ESP_LOGI instead of usart_write
// - improve ifndefs
// - increase HTTP buffer size in scd41 (is this possible with current code?)
// - proper interval for https measurements: 10 minutes [done] --> needs some more testing (time with stopwatch)
// - dynamic MAC ESP-NOW ! ! ! ! !

#ifndef USE_HTTP
void main_esp_now(void)
{
    wifi_init_espnow();
    espnow_init();

    #ifdef ESP_NOW_RECEIVER
        // with any power saving on, ESP-NOW doesn't work well
        // (about every 9 out of 10 packets get lost)
        esp_wifi_set_ps(WIFI_PS_NONE);
    #endif // ESP_NOW_RECEIVER

    #ifndef ESP_NOW_RECEIVER
        set_modem_sleep();
    #endif

    while(1)
    {
        #ifndef ESP_NOW_RECEIVER
            set_light_sleep();
        #else
            delay(5000); // fix for watchdog issues
        #endif // ESP_NOW_RECEIVER
    }
}
#else
void main_https(void)
{
    initialize_wifi();

    while(1) {
        set_light_sleep(); // creates the right timing and will measure after
        delay(1000); // watchdogs aren't welcome
    }
}
#endif // USE_HTTP

void app_main() 
{
    #ifndef ESP_NOW_RECEIVER
        i2c_init();
        scd41_init();
    #endif // ESP_NOW_RECEIVER

    #ifndef USE_HTTP
        main_esp_now();
    #else
        main_https();
    #endif // USE_HTTP
}