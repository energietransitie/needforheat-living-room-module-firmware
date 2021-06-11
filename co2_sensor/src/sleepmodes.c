// INCLUDE FILES
#include "stdio.h"
#include "usart.h"
#include "string.h"
#include "esp_sleep.h"
#include "esp_wifi.h"
#include "sleepmodes.h"
#include <../lib/generic_esp_32/generic_esp_32.h>

#include "../include/scd41.h"

// DEFINES
#define TIME_IN_LIGHTSLEEP      60 * 10 * 1000 * 1000   // microseconds --> 10 minutes
#define TIME_IN_LIGHTSLEEP_TEST 60000000

// GLOBAL VARIABLES
char str[256];

// Function:    light_sleep_start()
// Params:      N/A
// Returns:     N/A
// Desription:  Used to enter light sleep, then wake up automatically after 10 minutes
void set_light_sleep()
{
    esp_wifi_stop();
    esp_sleep_enable_timer_wakeup(TIME_IN_LIGHTSLEEP_TEST);
    //usart_write(&str[0], strlen(&str[0]));
    esp_light_sleep_start();

    // woke up when it gets here
    esp_wifi_start();   // maybe this can be removed? (add to modem_sleep wake-up)
    scd41_measure_co2_temp_rht();
}

// Function:    disable_bluetooth()
// Params:      N/A
// Returns:     N/A
// Desription:  Used to disable bluetooth
// void disable_bluetooth()
// {
//     esp_bluedroid_disable();
//     esp_bt_controller_disable();
//     sprintf(&str[0], "disabling bluetooth succesfull");
//     usart_write(&str[0], strlen(&str[0]));
// }

// Function:   set_modem_sleep()
// Params:     N/A
// Returns:    N/A
// Desription: Used to enter modem sleep
void set_modem_sleep()
{
    // set the wifi powersave mode to max
    esp_wifi_set_ps(WIFI_PS_MAX_MODEM);
}

// Function:   wake_modem_sleep()
// Params:     N/A
// Returns:    N/A
// Desription: Used to wake from modem sleep
void wake_modem_sleep()
{
    // turn the powersave mode off
    esp_wifi_set_ps(WIFI_PS_NONE);
}

// Function:   switch_modes()
// Params:     N/A
// Returns:    N/A
// Desription: Used to switch between light sleep and modem sleep
void switch_modes()
{
    printf("in light sleep\n");
    set_light_sleep();
    printf("uit light sleep\n");
    set_modem_sleep();
    printf("in modem sleep\n");
    wake_modem_sleep();
    printf("uit modem sleep\n");
}