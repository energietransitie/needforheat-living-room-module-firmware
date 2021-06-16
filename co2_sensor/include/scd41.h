#ifndef __SCD41_H__
#define __SCD41_H__

#include <stdint.h>
#include "../include/espnow.h"

#define SCD41_SAMPLE_INTERVAL_S       (5 * 60) // seconds (5 min * 60 s/min)
#define SCD41_SINGLE_SHOT_DELAY_MS     1350 // ms

#define SCD41_MEASUREMENT_INTERVAL_MS  ((SCD41_SAMPLE_INTERVAL_S * 1000) - SCD41_SINGLE_SHOT_DELAY_MS) // milliseconds
#define SCD41_MEASUREMENT_INTERVAL_US  ((SCD41_SAMPLE_INTERVAL_S * 1000 * 1000) - (SCD41_SINGLE_SHOT_DELAY_MS * 1000)) // microseconds
#define SCD41_MEASUREMENT_INTERVAL_TXT "Waiting 5 minutes for next SCD41 measurement..."

#ifdef USE_HTTP
    #define SCD41_BUFFER_SIZE       2 // uploads (10 min / 5 min/upload)
#else
    #define SCD41_BUFFER_SIZE       ESPNOW_MAX_SAMPLES
#endif // USE_HTPP
void scd41_init(void);
void scd41_disable_asc(void);
void scd41_measure_co2_temp_rht(void);
void scd41_store_measurements(uint8_t *read_buffer);
void scd41_send_data_espnow(void);
void scd41_reset_buffers(void);
void scd41_store_in_nvs(void);
void scd41_fetch_nvs(void);
void scd41_print_serial_number(void);

#endif // __SCD41_H__
