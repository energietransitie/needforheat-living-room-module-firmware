#include <../lib/generic_esp_32/generic_esp_32.h>

void setModemSleep(){
    // set the wifi powersave mode to max
    esp_wifi_set_ps(WIFI_PS_MAX_MODEM);
}

void enableWiFi(){
    wifi_prov_mgr_config_t config = initialize_provisioning();
    start_provisioning(config);
}

void wakeModemSleep(){
    // turn the powersave mode off
    esp_wifi_set_ps(WIFI_PS_NONE);
}







