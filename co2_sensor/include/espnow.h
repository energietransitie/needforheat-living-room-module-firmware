#ifndef __ESPNOW_H__
#define __ESPNOW_H__

#include <stdbool.h>
#include <esp_err.h>

#define ESPNOW_DATATYPE_CO2         2
#define ESPNOW_NUMBER_OF_BUFFERS    3
#define ESPNOW_MAX_SAMPLES          32

typedef struct espnow_msg_t
{
    uint8_t device_type;
    uint8_t nmeasurements;
    uint16_t index;
    uint16_t interval;
    uint16_t co2[ESPNOW_MAX_SAMPLES];       // 32 * 2 = 64 bytes
    float temperature[ESPNOW_MAX_SAMPLES];  // 32 * 4 = 128 bytes
    uint8_t rht[ESPNOW_MAX_SAMPLES];        // 32 * 1 = 32 bytes
} espnow_msg_t;                             // ----------- +
                                            // 224 bytes

void espnow_init(void);
void espnow_config_peer(uint8_t channel, bool encrypt, uint8_t *peer_mac);
uint8_t espnow_send(uint8_t *data, size_t size);
uint16_t espnow_get_message_index(void);

#endif // __ESPNOW_H__