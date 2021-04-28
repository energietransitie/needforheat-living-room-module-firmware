#include <../lib/generic_esp_32/generic_esp_32.h>

// Function:    set_modem_sleep()
// Params:      N/A
// Returns:     N/A
// Desription: Used to enter modem sleep
void set_modem_sleep(void)
{
    // set the wifi powersave mode to max
    esp_wifi_set_ps(WIFI_PS_MAX_MODEM);
}

// Function:    wake_modem_sleep()
// Params:      N/A
// Returns:     N/A
// Desription: Used to wake from modem sleep
void wake_modem_sleep(void)
{
    // turn the powersave mode off
    esp_wifi_set_ps(WIFI_PS_NONE);
}







