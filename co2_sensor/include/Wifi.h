#ifndef __WIFI_H__
#define __WIFI_H__

#include <stdint.h>

char *get_time(void);
//void send_https_measurements(uint16_t co2, float temp, uint8_t rh);
//void upload_measurement(const char *variable_interval_upload_url, const char *root_cert, char *bearer, char *msg);
void send_HTTPS(uint16_t co2, float temp, uint8_t rh);
void initialize_wifi();


#endif //__WIFI_H__