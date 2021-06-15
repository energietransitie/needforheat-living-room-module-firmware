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

//TODO: heartbeat loop and timers need to be moved to library
#define TIMER_DIVIDER 80
#include "driver/timer.h"
const char *heartbeat_upload_url = TWOMES_TEST_SERVER "/device/measurements/variable-interval";
char *bearer;

int regular_timer_count = 0;
int boiler_timer_count = 0;
int room_temp_timer_count = 0;
int upload_timer_count = 0;
typedef struct
{
    int timer_group;
    int timer_idx;
    int alarm_interval;
    bool auto_reload;
} seconds_timer_info_t;

bool IRAM_ATTR timer_group_isr_callback(void *args)
{
    BaseType_t high_task_awoken = pdFALSE;
    // example_timer_info_t *info = (example_timer_info_t *) args;

    // uint64_t timer_counter_value = timer_group_get_counter_value_in_isr(info->timer_group, info->timer_idx);

    /* Prepare basic event data that will be then sent back to task */
    // example_timer_event_t evt = {
    //     .info.timer_group = info->timer_group,
    //     .info.timer_idx = info->timer_idx,
    //     .info.auto_reload = info->auto_reload,
    //     .info.alarm_interval = info->alarm_interval,
    //     .timer_counter_value = timer_counter_value
    // };

    // if (!info->auto_reload) {
    //     timer_counter_value += info->alarm_interval * TIMER_SCALE;
    //     timer_group_set_alarm_value_in_isr(info->timer_group, info->timer_idx, timer_counter_value);
    // }

    /* Now just send the event data back to the main program task */
    // xQueueSendFromISR(s_timer_queue, &evt, &high_task_awoken);

    regular_timer_count++;
    boiler_timer_count++;
    room_temp_timer_count++;
    upload_timer_count++;
    return high_task_awoken == pdTRUE; // return whether we need to yield at the end of ISR
}
//int timer_interval in microseconds
void initialize_timer(timer_group_t group, timer_idx_t timer, bool auto_reload, int timer_interval)
{
    /* Select and initialize basic parameters of the timer */
    timer_config_t config = {
        .alarm_en = TIMER_ALARM_EN,
        .counter_en = TIMER_PAUSE,
        .intr_type = TIMER_INTR_LEVEL,
        .counter_dir = TIMER_COUNT_UP,
        .auto_reload = (timer_autoreload_t)auto_reload,
        .divider = TIMER_DIVIDER}; // default clock source is APB

    timer_init(group, timer, &config);

    timer_set_alarm_value(group, timer, timer_interval);
    timer_enable_intr(group, timer);

    // example_timer_info_t *timer_info = calloc(1, sizeof(example_timer_info_t));
    // timer_info->timer_group = group;
    // timer_info->timer_idx = timer;
    // timer_info->auto_reload = auto_reload;
    // timer_info->alarm_interval = timer_interval_sec;
    timer_isr_callback_add(group, timer, timer_group_isr_callback, NULL, 0);

    timer_start(group, timer);
}

void heartbeat_loop(void *data)
{
    #define HEARTBEAT_UPLOAD_INTERVAL 3600000     //ms, so one hour
    #define HEARTBEAT_MEASUREMENT_INTERVAL 600000 //ms, so 10 minutes; not yet in effect
    initialize_timer(TIMER_GROUP_0, TIMER_0, true, 10*60*1000*1000);
    while (1)
    {
        //Wait to make sure Wi-Fi is enabled.
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        //Upload heartbeat
        upload_heartbeat(heartbeat_upload_url, root_cert, bearer);
        //Wait to make sure uploading is finished.
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        //Disconnect WiFi
        //Wait HEARTBEAT_UPLOAD_INTERVAL(currently 1 hour)
        vTaskDelay(HEARTBEAT_UPLOAD_INTERVAL / portTICK_PERIOD_MS);
    }
}

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
    initialize_wifi();
    enable_wifi();
    root_cert = get_root_ca();

    xTaskCreatePinnedToCore(&heartbeat_loop, "heartbeat_loop", 4096, NULL, 1, NULL, 1);

    while(1) {
        scd41_measure_co2_temp_rht();
        ESP_LOGI("Main", "Waiting 10 minutes...");
        delay(SCD41_WAIT_TEN_MINUTES_MS);
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