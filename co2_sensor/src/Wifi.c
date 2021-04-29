#include "../lib/generic_esp_32/generic_esp_32.h"
#include "../include/usart.h"
#include "../include/spi.h"
#include "../include/util.h"
#include "../include/i2c.h"
#include "../include/Wifi.h"

void connect_to_WiFi(){
    wifi_prov_mgr_config_t config = initialize_provisioning();
    start_provisioning(config);
}

void initialize_wifi(){
    initialize_nvs();
    initialize();
    ESP_ERROR_CHECK(esp_netif_init());
    connect_to_WiFi();
    initialize_time("CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00");
}

void send_HTTPS(){
    char* url = "https://192.168.178.216:4444/set/house/opentherm";
    static const char *TAG = "Twomes Example Application ESP32";
    char* dataPlain = "{\"deviceMac\":\"8C:AA:B5:85:A2:3D\",\"measurements\": [{\"property\":\"testy\",\"value\":\"hello_world\"}],\"time\":%d}";
    char data [strlen(dataPlain)];
    ESP_LOGI(TAG, "Hello World!");
    post_https(url, data, NULL);
}