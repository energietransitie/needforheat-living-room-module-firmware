#ifndef _TWOMES_SENSOR_PAIRING_H
#define _TWOMES_SENSOR_PAIRING_H

#include <string.h>
#include <stdlib.h>
#include <esp_log.h>
#include <nvs.h>
#include <nvs_flash.h>
#include <esp_now.h>
#include <esp_system.h>

#define MAC_ADDR_SIZE            6

#define ESPNOW_PAIRING_CHANNEL  1

const char *TAG = "ESPNOW_PAIRING";
uint8_t pairing_macaddr[MAC_ADDR_SIZE + 1];

/**
 * @brief read the P1 Gateway Mac and channel from NVS
 *
 * @param macAddress buffer to store the MAC address
 * @param len  length of the buffer (SHOULD ALWAYS BE 6!)
 * @param channel pointer to uint8_t to store channel
 *
 * @return error code
 */
int getGatewayData(uint8_t *macAddress, size_t len, uint8_t *channel) {
    esp_err_t err;
    nvs_handle gatewayHandle;

    //Open the NVS partition:
    err = nvs_open("gatewaySettings", NVS_READONLY, &gatewayHandle);
    if (err != ESP_OK) {
        return err; //return the error
    }

    //read the channel and MAC address:
    err = nvs_get_blob(gatewayHandle, "MAC", macAddress, &len);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Could not read Mac Address!");
        return err;
    }
    err = nvs_get_u8(gatewayHandle, "channel", channel);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Could not read channel!");
        return err;
    }
    return ESP_OK; //Return OK on succes.
}

//Set this as onDataRecv calback when you want to pair. Remove the callback after pairing is done
//ESP automatically reboots when done.
void onDataReceive(const uint8_t *macAddress, const uint8_t *payload, int length) {
    //Read the MAC of the sender and store it in NVS:
    esp_err_t err;
    nvs_handle gatewayHandle;

    // save mac address to memory
    memcpy(pairing_macaddr, macAddress, MAC_ADDR_SIZE);

    //Open the NVS partition:
    err = nvs_open("gatewaySettings", NVS_READWRITE, &gatewayHandle);
    if (err == ESP_OK) {
        nvs_set_blob(gatewayHandle, "MAC", macAddress, 6);
        nvs_set_u8(gatewayHandle, "channel", *payload);

        esp_restart(); //restart the device after pairing
    }
}



#endif //_TWOMES_SENSOR_PAIRING_H