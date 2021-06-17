#include "../lib/generic_esp_32/generic_esp_32.h"
#include "string.h"
#include "../include/util.h"
#include "../include/i2c.h"
#include "../include/wifi.h"
#include "../include/timer.h"

#include "../include/scd41.h"

#define RETRY_DELAY                 10 * 1000 // milliseconds (10 s * 1000 ms/s)

#define HTTPS_STATUS_OK             200

#define MESSAGE_BUFFER_SIZE         4096
#define MEASUREMENT_TYPE_CO2        "\"CO2concentration\""
#define MEASUREMENT_TYPE_RH         "\"relativeHumidity\""
#define MEASUREMENT_TYPE_ROOMTEMP   "\"roomTemp\""

char temp[64];
char *msg_start = "{\"upload_time\": \"%d\",\"property_measurements\": [";
char *meas_str  = "{\"property_name\": %s,"
                      "\"timestamp\":\"%d\","
                      "\"timestamp_type\": \"end\","
                      "\"interval\": %d,"
                      "\"measurements\": [";
char *msg_end   = "] }";
    
char strftime_buf[64]; // FIXME: weird things happen when you remove this one

char *bearer;
const char *rootCA;


uint8_t wifi_espnow_on = WIFI_ESPNOW_OFF;

// Function:    wifi_init_espnow()
// Params:      N/A
// Returns:     N/A
// Description: used to initialise Wi-Fi for ESP-NOW
void wifi_init_espnow(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_start());

    wifi_espnow_on = WIFI_ESPNOW_ON;
}

// Function:    wifi_init_espnow()
// Params:      N/A
// Returns:
//      - (uint8_t) Returns WIFI_ESPNOW_OFF if wifi for esp now is off, otherwise returns
//                  WIFI_ESPNOW_ON
// Description: You can use this function to know if wifi is enabled for esp now
uint8_t wifi_espnow_enabled(void)
{
    return wifi_espnow_on;
}

// Function:        append_uint16()
// Params:
//      - (uint32_t *)      buffer [TODO]
//      - (size_t)          [TODO]
//      - (char *)          pointer to message
//      - (const char *)    type of measurement (CO2, temp or RH)
// Returns:         N/A
// Description:     puts measurements at the end of the buffer [TODO]
void append_uint16(uint16_t *b, size_t size, char *msg_ptr, const char *type)
{
    time_t now = time(NULL);

    // measurement type header
    int msgSize = variable_sprintf_size(meas_str, 3, type, now, (SCD41_SAMPLE_INTERVAL_S));
    snprintf(temp, msgSize, meas_str, type, now, (SCD41_SAMPLE_INTERVAL_S));
    strcat(msg_ptr, temp);

    // append measurements
    for(uint32_t i = 0; i < size-1; i++)
    {
        msgSize = variable_sprintf_size("\"%u\",", 1, b[i]);
        snprintf(temp, msgSize, "\"%u\",", b[i]);
        strcat(msg_ptr, temp);
    }
    
    // add last measurement and end of this object
    msgSize = variable_sprintf_size("\"%u\"] }", 1, b[size-1]);
    snprintf(temp, msgSize, "\"%u\"] }", b[size-1]);
    strcat(msg_ptr, temp);
}

// Function:        upload()
// Params:
//      - (uint16_t *)      buffer for CO2 measurements
//      - (float *)         buffer for temperature measurements
//      - (uint8_t *)       buffer for relative humidity measurements
//      - (size_t)          [TODO]
// Returns:         N/A
// Description:     used to upload measurements to the API
void upload(uint16_t *b_co2, uint16_t *b_temp, uint16_t *b_rh, size_t size)
{
    time_t now = time(NULL);
    char *msg = malloc(MESSAGE_BUFFER_SIZE);

    int msgSize = variable_sprintf_size(msg_start, 1, now);
    snprintf(msg, msgSize, msg_start, now);

    append_uint16(b_co2, size, msg, MEASUREMENT_TYPE_CO2); 
    strcat(msg, ",");
    append_uint16(b_rh, size, msg, MEASUREMENT_TYPE_RH);
    strcat(msg, ",");
    append_uint16(b_temp, size, msg, MEASUREMENT_TYPE_ROOMTEMP);

    strcat(msg, "] }");

    ESP_LOGI("test", "data: %s", msg );

    if(post_https(VARIABLE_INTERVAL_UPLOAD_URL, msg, rootCA, bearer, NULL, 0) != HTTPS_STATUS_OK)
    {
        ESP_LOGI("HTTPS", "Sending failed, attempting retry...");
        delay(RETRY_DELAY); // wait ten seconds (does shift measurements by 10 seconds too)
        upload(b_co2, b_temp, b_rh, size);
    }
    //TODO: should we free(msg);
}

// Function:        send_HTTPS()
// Params:
//      - (uint16_t *)      co2 buffer
//      - (float *)         temperature buffer
//      - (uint8_t *)       relative humidity buffer
//      - (size_t)          [TODO]
// Returns:         N/A
// Description:     sends measurements to the API
void send_HTTPS(uint16_t *co2, uint16_t *temp, uint16_t *rh, size_t size)
{
    //TODO: use thread safe counter (https://www.freertos.org/CreateCounting.html) to count #threads using wifi; only call enable_wifi() if counter is increased from 0 to 1 here.
    enable_wifi();
    //Wait to make sure Wi-Fi is enabled.
    vTaskDelay(HTTPS_PRE_WAIT_MS / portTICK_PERIOD_MS);
    //Upload SCD41 measurements
    upload(co2, temp, rh, size);              
    //Wait to make sure uploading is finished.
    vTaskDelay(HTTPS_POST_WAIT_MS / portTICK_PERIOD_MS);
    //Disconnect WiFi
    //TODO: use thread safe counter (https://www.freertos.org/CreateCounting.html) to count #threads using wifi; only call enable_wifi() if counter is increased from 0 to 1 here.
    disable_wifi();
}
