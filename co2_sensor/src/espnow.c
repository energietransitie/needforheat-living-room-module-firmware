#include "../include/espnow.h"
#include "../include/usart.h"
#include "../include/errorcode.h"
#include "../include/util.h"

#include <esp_now.h>
#include <esp_wifi.h>
#include <esp_netif.h>
#include <wifi_provisioning/manager.h>
#include <string.h>
#include <stdbool.h>

#define DATATYPE_CO2    2

#define MAC_ADDR_SIZE   6 // bytes

esp_now_peer_info_t info; // needs to be global (ESP argument error if not)

uint8_t mymac_addr[MAC_ADDR_SIZE],
        peermac_addr[MAC_ADDR_SIZE] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

static void espnow_cb_ondatarecv(const uint8_t *mac, const uint8_t *data, const uint32_t len)
{
    usart_write("received, eh... something\n", 28);
}

void espnow_init(void)
{
    //ESP_ERROR_CHECK(esp_netif_init());
    
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    delay(2000);
    ESP_ERROR_CHECK(esp_now_init());

    // config peer with broadcast address
    espnow_config_peer(11, false, &peermac_addr[0]);

    ESP_ERROR_CHECK(esp_now_register_recv_cb((esp_now_recv_cb_t) espnow_cb_ondatarecv));

    ESP_ERROR_CHECK(esp_wifi_get_mac(ESP_IF_WIFI_STA, mymac_addr));
}

void espnow_config_peer(uint8_t channel, bool encrypt, uint8_t *peer_mac)
{

    memcpy(&(info.peer_addr), peer_mac, MAC_ADDR_SIZE);

    info.channel = channel;
    info.encrypt = encrypt;

    char str[40];

    // TESTING ONLY
    sprintf(&str[0], "mac: %x:%x:%x:%x:%x:%x\n", info.peer_addr[0], info.peer_addr[1], info.peer_addr[2], info.peer_addr[3], info.peer_addr[4], info.peer_addr[5]);
    usart_write(&str[0], strlen(&str[0]));

    ESP_ERROR_CHECK(esp_now_add_peer((esp_now_peer_info_t *) &info));
}

uint8_t espnow_deinit(void)
{
    // probably unused
    return 0;
}

esp_err_t espnow_send(uint8_t *data, size_t size)
{
    char str[40];

    // TESTING ONLY
    sprintf(&str[0], "sending msg\n");
    usart_write(&str[0], strlen(&str[0]));

    return esp_now_send(peermac_addr, data, size);
}