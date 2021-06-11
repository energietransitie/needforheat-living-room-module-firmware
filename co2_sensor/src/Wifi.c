#include "../lib/generic_esp_32/generic_esp_32.h"
#include "../include/usart.h"
#include "string.h"
//#include "../include/spi.h"
#include "../include/util.h"
#include "../include/i2c.h"
#include "../include/Wifi.h"
#include "../include/timer.h"

#define DEVICE_NAME "Generic-Test"
//#define HEARTBEAT_UPLOAD_INTERVAL 3600000     //ms, so one hour
//#define HEARTBEAT_MEASUREMENT_INTERVAL 600000 //ms, so 10 minutes; not yet in effect

#define FLOAT_COMPENSATION          4
#define MEASUREMENT_TYPE_CO2        "\"CO2concentration\""
#define MEASUREMENT_TYPE_RH         "\"relativeHumidity\""
#define MEASUREMENT_TYPE_ROOMTEMP   "\"roomTemp\""

char *MSG_PLAIN_INT = "{\"upload_time\": \"%ld\",\"property_measurements\": [    {" 
                      "\"property_name\": %s," 
                      "\"timestamp\":\"%ld\"," 
                      "\"timestamp_type\": \"start\"," 
                      "\"interval\": 0," 
                      "\"measurements\": [" "\"%d\"" "]}]}";

char *MSG_PLAIN_FLOAT = "{\"upload_time\": \"%ld\",\"property_measurements\": [    {" 
                        "\"property_name\": %s," 
                        "\"timestamp\":\"%ld\"," 
                        "\"timestamp_type\": \"start\"," 
                        "\"interval\": 0," 
                        "\"measurements\": [" "\"%f\"" "]}]}";


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

// Function:    send_HTTPS()
// Params:      
//      - (uint16_t) the co2 values
//      - (float) the temperature values
//      - (uint8_t) the relative humidity values
// Returns:     N/A
// Desription: takes the values and sends them with HTTPS
void send_HTTPS(uint16_t co2, float temp, uint8_t rh)
{
    //bearer = get_bearer();
    enable_wifi();
    //Wait to make sure Wi-Fi is enabled.
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    //Upload heartbeat
    //send_https_measurements(co2, temp, rh);

    https_meas_t vals = { .co2 = co2, .temp = temp, .rh = rh};
    upload_measurement_int(variable_interval_upload_url, rootCA, bearer, (int) co2);
    upload_measurement_int(variable_interval_upload_url, rootCA, bearer, (int) rh);
    upload_measurement_float(variable_interval_upload_url, rootCA, bearer, (double) temp);

    //upload_measurements(variable_interval_upload_url, rootCA, bearer, vals);
    
    //Wait to make sure uploading is finished.
    vTaskDelay(500 / portTICK_PERIOD_MS);
    //Disconnect WiFi
    disable_wifi();
}
