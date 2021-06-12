#ifndef __WIFI_H__
#define __WIFI_H__

#include <stdint.h>

void wifi_init_espnow(void);
void send_HTTPS(uint16_t *co2, uint16_t *temp, uint16_t *rh, size_t size);
void initialize_wifi();


#endif //__WIFI_H__