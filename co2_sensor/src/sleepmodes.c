#include "stdio.h"
#include "string.h"
#include "esp_sleep.h"
#include "esp_wifi.h"
#include "sleepmodes.h"

#include <../lib/generic_esp_32/generic_esp_32.h>
#include "../include/scd41.h"

#define DEBUG_ENABLED

#ifdef DEBUG_ENABLED
    #define TIME_IN_LIGHTSLEEP      1200
#else
    #define TIME_IN_LIGHTSLEEP      (SCD41_SAMPLE_INTERVAL * 1000 * 1000) - \
                                    (SCD41_SINGLE_SHOT_DELAY * 1000)    // microseconds --> 10 minutes - sensor measuremen delay 
                                                                        // to avoid time shifting
#endif

#define TIME_IN_LIGHTSLEEP_TEST 60000000

// Function:    light_sleep_start()
// Params:      N/A
// Returns:     N/A
// Desription:  Used to enter light sleep, then wake up automatically after 10 minutes
void set_light_sleep()
{
    esp_sleep_enable_timer_wakeup(TIME_IN_LIGHTSLEEP);
    esp_light_sleep_start();

    // woke up when it gets here   
    scd41_measure_co2_temp_rht();
}

// Function:    set_custom_lightsleep()
// Params:      
//      - (uint32_t) length of sleep
// Returns:     N/A
// Desription:  Used to enter light sleep with a non-10-minute sleep time
void set_custom_lightsleep(uint32_t len)
{
    esp_sleep_enable_timer_wakeup(len);
    esp_light_sleep_start();
}

// Function:   set_modem_sleep()
// Params:     N/A
// Returns:    N/A
// Desription: Used to enter modem sleep
void set_modem_sleep()
{
    // set the wifi powersave mode to max
    esp_wifi_set_ps(WIFI_PS_MAX_MODEM);
    esp_wifi_stop();
}

// Function:   wake_modem_sleep()
// Params:     N/A
// Returns:    N/A
// Desription: Used to wake from modem sleep
void wake_modem_sleep()
{
    esp_wifi_start();

    // turn the powersave mode off
    esp_wifi_set_ps(WIFI_PS_NONE);
}
