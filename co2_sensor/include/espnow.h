#ifndef __ESPNOW_H__
#define __ESPNOW_H__

#include <stdbool.h>
#include <esp_err.h>

void espnow_init(void);
void espnow_config_peer(uint8_t channel, bool encrypt, uint8_t *peer_mac);
esp_err_t espnow_send(uint8_t *data, size_t size);

#endif // __ESPNOW_H__