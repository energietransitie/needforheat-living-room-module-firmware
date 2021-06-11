#include "../lib/generic_esp_32/generic_esp_32.h"
#include "../include/usart.h"
#include "string.h"
//#include "../include/spi.h"
#include "../include/util.h"
#include "../include/i2c.h"
#include "../include/Wifi.h"
#include "../include/timer.h"

#include "../include/scd41.h"

#define DEVICE_NAME "Generic-Test"
//#define HEARTBEAT_UPLOAD_INTERVAL 3600000     //ms, so one hour
//#define HEARTBEAT_MEASUREMENT_INTERVAL 600000 //ms, so 10 minutes; not yet in effect

#define MESSAGE_BUFFER_SIZE         4096
#define FLOAT_COMPENSATION          4
#define MEASUREMENT_TYPE_CO2        "\"CO2concentration\""
#define MEASUREMENT_TYPE_RH         "\"relativeHumidity\""
#define MEASUREMENT_TYPE_ROOMTEMP   "\"roomTemp\""

#define BUFFER_TYPE_CO2             0
#define BUFFER_TYPE_RH              1

char temp[64];
char *msg_start = "{\"upload_time\": \"%d\",\"property_measurements\": [";
char *meas_str  = "{\"property_name\": %s,"
                      "\"timestamp\":\"%d\","
                      "\"timestamp_type\": \"end\","
                      "\"interval\": %d,"
                      "\"measurements\": [";
char *msg_end   = "] }"    ;
    
static const char *TAG = "Twomes Heartbeat Test Application ESP32";
char strftime_buf[64];

const char *device_activation_url = TWOMES_TEST_SERVER "/device/activate";
const char *variable_interval_upload_url = TWOMES_TEST_SERVER "/device/measurements/fixed-interval";
char *bearer;
const char *rootCA;

typedef struct https_meas_t
{
    uint16_t co2;
    float temp;
    uint8_t rh;
} https_meas_t;

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

// Function:    upload_measurement_int()
// Params:      
//  	- (const char) a pointer to the url to upload
// Returns:     N/A
// Desription:  Initializes the wifi
void upload_measurement_int(const char *variable_interval_upload_url, const char *root_cert, char *bearer, int value){
    char *measurementType = "\"CO2concentration\""; //was measurements
    //Updates Epoch Time
    time_t now = time(NULL);
    //Plain JSON request where values will be inserted.
    char *msg_plain = "{\"upload_time\": \"%d\",\"property_measurements\": [    {"
                      "\"property_name\": %s,"
                      "\"timestamp\":\"%d\","
                      "\"timestamp_type\": \"start\","
                      "\"interval\": 0,"
                      "\"measurements\": ["
                      "\"%d\""
                      "]}]}";
    //Get size of the message after inputting variables.
    int msgSize = variable_sprintf_size(msg_plain, 4, now, measurementType, now, value);
    //Allocating enough memory so inputting the variables into the string doesn't overflow
    char *msg = malloc(msgSize);
    //Inputting variables into the plain json string from above(msgPlain).
    snprintf(msg, msgSize, msg_plain, now, measurementType, now, value);
    //usart_write(msg, strlen(msg));
    //Posting data over HTTPS, using url, msg and bearer token.
    ESP_LOGI(TAG, "Data: %s", msg);
    post_https(variable_interval_upload_url, msg, root_cert, bearer, NULL, 0);
    vTaskDelay(500 / portTICK_PERIOD_MS);
}

void upload_measurement_float(const char *variable_interval_upload_url, const char *root_cert, char *bearer, double value){
    char *measurementType = "\"CO2concentration\""; //was measurements
    //Updates Epoch Time
    time_t now = time(NULL);
    //Plain JSON request where values will be inserted.
    char *msg_plain = "{\"upload_time\": \"%d\",\"property_measurements\": [    {"
                      "\"property_name\": %s,"
                      "\"timestamp\":\"%d\","
                      "\"timestamp_type\": \"start\","
                      "\"interval\": 0,"
                      "\"measurements\": ["
                      "\"%f\""
                      "]}]}";
    //Get size of the message after inputting variables.
    int msgSize = variable_sprintf_size(msg_plain, 4, now, measurementType, now, value);
    //Allocating enough memory so inputting the variables into the string doesn't overflow
    char *msg = malloc(msgSize);
    //Inputting variables into the plain json string from above(msgPlain).
    snprintf(msg, msgSize, msg_plain, now, measurementType, now, value);
    //usart_write(msg, strlen(msg));
    //Posting data over HTTPS, using url, msg and bearer token.
    ESP_LOGI(TAG, "Data: %s", msg);
    post_https(variable_interval_upload_url, msg, root_cert, bearer, NULL, 0);
    vTaskDelay(500 / portTICK_PERIOD_MS);
}

<<<<<<< HEAD
// Function:    send_HTTPS()
// Params:      
//      - (uint16_t) the co2 values
//      - (float) the temperature values
//      - (uint8_t) the relative humidity values
// Returns:     N/A
// Desription: takes the values and sends them with HTTPS
void send_HTTPS(uint16_t co2, float temp, uint8_t rh)
=======
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
    //char temp[64];
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

    append_ints(uint16_to_uint32(b_co2, size), size, msg, MEASUREMENT_TYPE_CO2); 
    strcat(msg, ",");
    append_ints(uint8_to_uint32(b_rh, size), size, msg, MEASUREMENT_TYPE_RH);
    strcat(msg, ",");
    append_floats(b_temp, size, msg, MEASUREMENT_TYPE_ROOMTEMP);

    strcat(msg, "] }");

    usart_write("data: ", 6);
    usart_write(msg, strlen(msg));
    usart_write("\n", 1);

    post_https(variable_interval_upload_url, msg, rootCA, bearer, NULL, 0); // msg is freed by this function
    vTaskDelay(500 / portTICK_PERIOD_MS);
}

void send_HTTPS(uint16_t *co2, float *temp, uint8_t *rh, size_t size)
>>>>>>> 84bee98b1ac34f440d5c40d699de5ef62635c211
{
    //bearer = get_bearer();
    enable_wifi();
    //Wait to make sure Wi-Fi is enabled.
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    //Upload heartbeat
    //send_https_measurements(co2, temp, rh);

    // upload_measurement_int(variable_interval_upload_url, rootCA, bearer, (int) co2);
    // upload_measurement_int(variable_interval_upload_url, rootCA, bearer, (int) rh);
    // upload_measurement_float(variable_interval_upload_url, rootCA, bearer, (double) temp);

    upload(co2, temp, rh, size);

    //upload_measurements(variable_interval_upload_url, rootCA, bearer, vals);
    
    //Wait to make sure uploading is finished.
    vTaskDelay(500 / portTICK_PERIOD_MS);
    //Disconnect WiFi
    disable_wifi();
}
