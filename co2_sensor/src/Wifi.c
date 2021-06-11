#include "../lib/generic_esp_32/generic_esp_32.h"
#include "../include/usart.h"
#include "string.h"
#include "../include/util.h"
#include "../include/i2c.h"
#include "../include/Wifi.h"
#include "../include/timer.h"

#include "../include/scd41.h"

#define DEVICE_NAME "Generic-Test"

#define MESSAGE_BUFFER_SIZE         4096
#define MEASUREMENT_TYPE_CO2        "\"CO2concentration\""
#define MEASUREMENT_TYPE_RH         "\"relativeHumidity\""
#define MEASUREMENT_TYPE_ROOMTEMP   "\"roomTemp\""

#define MAX_RETRIES                 10

// message send information
char temp[64];
char *msg_start = "{\"upload_time\": \"%d\",\"property_measurements\": [";
char *meas_str  = "{\"property_name\": %s,"
                      "\"timestamp\":\"%d\","
                      "\"timestamp_type\": \"end\","
                      "\"interval\": %d,"
                      "\"measurements\": [";
char *msg_end   = "] }";
    
static const char *TAG = "Twomes Heartbeat Test Application ESP32";

const char *device_activation_url = TWOMES_TEST_SERVER "/device/activate";
const char *variable_interval_upload_url = TWOMES_TEST_SERVER "/device/measurements/fixed-interval";
char *bearer;
const char *rootCA;

// Function:    initialize_wifi()
// Params:      N/A
// Returns:     N/A
// Desription:  Initializes this wifi
void initialize_wifi(){
 
    initialize_nvs();
    initialize();
    /* Initialize TCP/IP */
    ESP_ERROR_CHECK(esp_netif_init());

    wifi_prov_mgr_config_t config = initialize_provisioning();

    //Make sure to have this here otherwise the device names won't match because
    //of config changes made by the above function call.
    prepare_device();
    //Starts provisioning if not provisioned, otherwise skips provisioning.
    //If set to false it will not autoconnect after provisioning.
    //If set to true it will autonnect.
    start_provisioning(config, true);
    //Initialize time with timezone UTC; building timezone is stored in central database
    initialize_time("UTC");

    //Gets time as epoch time.
    ESP_LOGI(TAG, "Getting time!");
    uint32_t now = time(NULL);
    ESP_LOGI(TAG, "Time is: %d", now);

    bearer = get_bearer();
    char *device_name;
    device_name = malloc(DEVICE_NAME_SIZE);
    get_device_service_name(device_name, DEVICE_NAME_SIZE);
    rootCA = get_root_ca();

    if (strlen(bearer) > 1)
    {
        ESP_LOGI(TAG, "Bearer read: %s", bearer);
    }
    else if (strcmp(bearer, "") == 0)
    {
        ESP_LOGI(TAG, "Bearer not found, activating device!");
        activate_device(device_activation_url, device_name, rootCA);
        bearer = get_bearer();
    }
    else if (!bearer)
    {
        ESP_LOGE(TAG, "Something went wrong whilst reading the bearer!");
    }
}

void append_ints(uint32_t *b, size_t size, char *msg_ptr, const char *type)
{
    time_t now = time(NULL);

    // measurement type header
    int msgSize = variable_sprintf_size(meas_str, 3, type, now, (SCD41_SAMPLE_INTERVAL * 1000));
    snprintf(temp, msgSize, meas_str, type, now, (SCD41_SAMPLE_INTERVAL * 1000));
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

    free(b);
}

void append_floats(float *b, size_t size, char *msg_ptr, const char *type)
{
    time_t now = time(NULL);

    // measurement type header
    int msgSize = variable_sprintf_size(meas_str, 3, type, now, (SCD41_SAMPLE_INTERVAL * 1000));
    snprintf(temp, msgSize, meas_str, type, now, (SCD41_SAMPLE_INTERVAL * 1000));
    strcat(msg_ptr, temp);

    // append measurements
    for(uint32_t i = 0; i < size-1; i++)
    {
        msgSize = variable_sprintf_size("\"%f\",", 1, (double) b[i]);
        snprintf(temp, msgSize, "\"%f\",", (double) b[i]);
        strcat(msg_ptr, temp);
    }
    
    // add last measurement and end of this object
    msgSize = variable_sprintf_size("\"%f\"] }", 1, (double) b[size-1]);
    snprintf(temp, msgSize, "\"%f\"] }", (double) b[size-1]);
    strcat(msg_ptr, temp);
}

void upload(uint16_t *b_co2, float *b_temp, uint8_t *b_rh, size_t size)
{
    time_t now = time(NULL);
    char *msg = malloc(MESSAGE_BUFFER_SIZE);

    int msgSize = variable_sprintf_size(msg_start, 1, now);
    snprintf(msg, msgSize, msg_start, now);

    // append co2 data to existing msg
    append_ints(uint16_to_uint32(b_co2, size), size, msg, MEASUREMENT_TYPE_CO2); 
    strcat(msg, ",");

    // append rh data to existing msg
    append_ints(uint8_to_uint32(b_rh, size), size, msg, MEASUREMENT_TYPE_RH);
    strcat(msg, ",");

    // append room temperatures to existing msg
    append_floats(b_temp, size, msg, MEASUREMENT_TYPE_ROOMTEMP);

    // end message
    strcat(msg, "] }");

    // TESTING ONLY
    usart_write("data: ", 6);
    usart_write(msg, strlen(msg));
    usart_write("\n", 1);

    // aaaand... send! :)
    if(post_https(variable_interval_upload_url, msg, rootCA, bearer, NULL, 0) != 200)
        usart_write("Error sending data!\n", 21);
    vTaskDelay(500 / portTICK_PERIOD_MS);
}

// Function:    send_HTTPS()
// Params:      
//      - (uint16_t) the co2 values
//      - (float) the temperature values
//      - (uint8_t) the relative humidity values
//      - (size_t) # of elements in buffer
// Returns:     N/A
// Desription: takes the values and sends them with HTTPS
void send_HTTPS(uint16_t *co2, float *temp, uint8_t *rh, size_t size)
{
    enable_wifi();

    //Wait to make sure Wi-Fi is enabled.
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    
    // upload data
    upload(co2, temp, rh, size);

    //Disconnect WiFi
    disable_wifi();
}
