#ifndef __WIFI_H__
#define __WIFI_H__

#include <stdint.h>

#define WIFI_ESPNOW_OFF     0
#define WIFI_ESPNOW_ON      1

void wifi_init_espnow(void);
uint8_t wifi_espnow_enabled(void);
void send_HTTPS(uint16_t *co2, uint16_t *temp, uint16_t *rh, size_t size);
void initialize_wifi();


#endif //__WIFI_H__