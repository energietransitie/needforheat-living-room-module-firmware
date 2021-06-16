const char *device_type_name = "CO2-meter-SCD4x";
static const char *TAG = "Twomes CO₂ meter ESP32";

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

#include "../lib/generic_esp_32/generic_esp_32.h"
const char *root_cert;

#ifndef USE_HTTP
void main_esp_now(void)
{
    #ifdef ESP_NOW_RECEIVER
        espnow_init();
        // with any power saving on, ESP-NOW doesn't work well
        // (about every 9 out of 10 packets get lost)
        esp_wifi_set_ps(WIFI_PS_NONE);
        espnow_recv_pair();
    #else
        espnow_init_on_first_boot();
    #endif // ESP_NOW_RECEIVER

    #ifndef ESP_NOW_RECEIVER
        set_modem_sleep();
    #endif

    while(1)
    {
        #ifndef ESP_NOW_RECEIVER
            //set_light_sleep();
            scd41_measure_co2_temp_rht();
            scd41_store_in_nvs();
            set_deep_sleep();
        #else
            delay(5000); // fix for watchdog issues
        #endif // ESP_NOW_RECEIVER
    }
}
#else
void main_https(void)
{
    twomes_device_provisioning(device_type_name);

    xTaskCreatePinnedToCore(&heartbeat_task, "heartbeat_task", 4096, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(&timesync_task, "timesync_task", 4096, NULL, 1, NULL, 1);

    while(1) {
        ESP_LOGI(TAG, "Taking SCD41 measurements");
        scd41_measure_co2_temp_rht();
        ESP_LOGI(TAG, SCD41_MEASUREMENT_INTERVAL_TXT);
        delay(SCD41_MEASUREMENT_INTERVAL_MS - HTTPS_PRE_WAIT_MS - HTTPS_POST_WAIT_MS);
    }
}
#endif // USE_HTTP

void app_main() 
{
    initialize_nvs();

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