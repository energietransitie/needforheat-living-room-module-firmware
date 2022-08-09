#include "../include/espnow.h"
#include "../include/errorcode.h"
#include "../include/util.h"
#include "../include/sleepmodes.h"
#include "../include/Wifi.h"
#include "../lib/sensor_pairing/twomes_sensor_pairing.h"

#include <esp_log.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <string.h>
#include <stdbool.h>

#define TAG "esp-now"

#define DELAY_INTERVAL      2   // milliseconds
#define ESPNOW_ACK          1
#define ESPNOW_NACK         2

#define MAC_ADDR_BROADCAST_NUL_TERM {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00} // null terminated
#define MAC_ADDR_TEST_SENDER        {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}    // LOLIN TFT/I2C SHIELD ESP
#define MAC_ADDR_TEST_RECVR         {0xc4, 0x4f, 0x33, 0x7f, 0xa8, 0x81}    // SHIELD-LESS ESP
#define WIFI_CHANNEL                1

#define MAX_SEND_ATTEMPTS       0xFF
#define RETRY_DELAY             10 * 1000 * 1000 // microseconds (= 10 seconds)

esp_now_peer_info_t info;           // needs to be global (ESP argument error if not)
volatile uint16_t msg_index = 0;
volatile uint8_t msg_ack = 0;

// future TODO: If you want to change this code to add dynamic mac and wifi-channels
//              you can do this by setting the peermac_addr dynamically in espnow_init().
//              At that point you may want to remove the mymac_addr array and the
//              above defines.
uint8_t peer_channel = 0;
uint8_t mymac_addr[MAC_ADDR_SIZE],
        peermac_addr[MAC_ADDR_SIZE] = 
        
         #ifdef ESP_NOW_RECEIVER
            MAC_ADDR_TEST_SENDER;
        #else
            MAC_ADDR_TEST_RECVR;
        #endif // ESP_NOW_RECEIVER

// Function:    espnow_cb_ondatarecv()
// Params:      
//      - (const uint8_t *) mac address of sender
//      - (const uint8_t *) data received
//      - (const uint32_t) size of data buffer
// Returns:     N/A
// Desription:  Used for testing - this function is the onDataReceive callback used to test if sending data works
#ifdef ESP_NOW_RECEIVER
void espnow_cb_ondatarecv(const uint8_t *mac, const uint8_t *data, const uint32_t len)
{
    ESP_LOGI(TAG, "received packet");
    
    espnow_msg_t *msg = (espnow_msg_t *) data;

    ESP_LOGI(TAG, "type: %x, nmeasurements: %x, index: %x, first co2 measurement: %i ppm", (msg->device_type), (msg->nmeasurements), (msg->index), (msg->co2[0]));
}

void espnow_recv_pair(void)
{
    ESP_LOGI(TAG, "Sent peering information to CO2 measuring device");
    uint8_t data = 1;

    ESP_ERROR_CHECK(esp_now_unregister_send_cb());
    ESP_ERROR_CHECK(esp_now_unregister_recv_cb());

    esp_now_send(peermac_addr, &data, 1);

    ESP_ERROR_CHECK(esp_now_register_recv_cb((esp_now_recv_cb_t) espnow_cb_ondatarecv));
}
#endif // ESP_NOW_RECEIVER

// Function:    espnow_cb_ondatasend()
// Params:      
//      - (const uint8_t *) mac address of peer
//      - (esp_now_send_status_t) status
// Returns:     N/A
// Desription:  Used to check if an ack was received
void espnow_cb_ondatasend(const uint8_t *mac, esp_now_send_status_t stat)
{
    if(!stat)
    {
        ESP_LOGI(TAG, "received ACK");       // testing only
        msg_index++;
        msg_ack = ESPNOW_ACK;
    }
    
    else
    {
        ESP_LOGI(TAG, "received NACK -");   // testing only
        msg_ack = ESPNOW_NACK;
    }
}

// Function:    espnow_init()
// Params:      N/A
// Returns:     N/A
// Desription:  Initializes ESP-NOW
void espnow_init(void)
{
    wifi_init_espnow();
    esp_wifi_set_channel(ESPNOW_PAIRING_CHANNEL, WIFI_SECOND_CHAN_NONE);
    ESP_ERROR_CHECK(esp_now_init());

    // register callbacks
    #ifdef ESP_NOW_RECEIVER
        ESP_ERROR_CHECK(esp_now_register_recv_cb((esp_now_recv_cb_t) espnow_cb_ondatarecv)); // testing only
        espnow_config_peer(WIFI_CHANNEL, false, peermac_addr);
    #else
        espnow_config_peer(peer_channel, false, peermac_addr);
        ESP_ERROR_CHECK(esp_now_register_send_cb((esp_now_send_cb_t) espnow_cb_ondatasend));
    #endif // ESP_NOW_RECEIVER
}

// Function:    espnow_init_on_first_boot()
// Params:      N/A
// Returns:     N/A
// Desription:  Initializes ESP-NOW only if it is not initialized yet (aka nothing stored in NVS)
//              to avoid always enabling Wi-Fi on a DEEPSLEEP_RESET
void espnow_init_on_first_boot(void)
{
    ESP_LOGI(TAG, "checking NVS for P1 pairing information..."); 
    
    if(getGatewayData(&peermac_addr[0], MAC_ADDR_SIZE, &peer_channel) != ESP_OK)
        espnow_config();
}

// Function:    espnow_config()
// Params:      N/A
// Returns:     N/A
// Desription:  Searches for pairing information and configures the peer
void espnow_config(void)
{
    wifi_init_espnow();
    esp_wifi_set_channel(ESPNOW_PAIRING_CHANNEL, WIFI_SECOND_CHAN_NONE);
    ESP_ERROR_CHECK(esp_now_init());

    #ifdef NO_GATEWAY_PAIRING
        // config peer with defines
        espnow_config_peer(WIFI_CHANNEL, false, &peermac_addr[0]);
    #else       
        espnow_pair_gateway();
    #endif // NO_GATEWAY_PAIRING
}

// Function:    espnow_pair_gateway()
// Params:      N/A
// Returns:     N/A
// Desription:  Pairs this device with the P1 gateway
void espnow_pair_gateway(void)
{
    ESP_LOGI(TAG, "P1 pairing information not found, pairing...");

    uint8_t temp_addr[MAC_ADDR_SIZE + 1] = MAC_ADDR_BROADCAST_NUL_TERM;
    memcpy(pairing_macaddr, temp_addr, MAC_ADDR_SIZE);

    ESP_ERROR_CHECK(esp_now_register_recv_cb((esp_now_recv_cb_t) onDataReceive));

    // wait until pairing is done by checking if the temporary address is equal
    // to newly received address (is broadcast if nothing is received yet)
    while(!strcmp((char *) pairing_macaddr, (char *) temp_addr))
        delay(10);

    // will never get here (esp will restart)
    ESP_ERROR_CHECK(esp_now_unregister_recv_cb());
}

// Function:    espnow_config_peer()
// Params:      
//      - (uint8_t) wifi channel used for communicating
//      - (bool) true if the data should be encrypted
//      - (uint8_t *) mac address of peer
// Returns:     N/A
// Desription:  Configures peer
void espnow_config_peer(uint8_t channel, bool encrypt, uint8_t *peer_mac)
{
    memcpy(&(info.peer_addr), peer_mac, MAC_ADDR_SIZE);

    info.channel = channel;
    info.encrypt = encrypt;

    // TESTING ONLY
    ESP_LOGI(TAG, "peer mac: %x:%x:%x:%x:%x:%x\n", info.peer_addr[0], info.peer_addr[1], info.peer_addr[2], info.peer_addr[3], info.peer_addr[4], info.peer_addr[5]);

    ESP_ERROR_CHECK(esp_now_add_peer((esp_now_peer_info_t *) &info));
}

// Function:    espnow_send()
// Params:      
//      - (uint8_t *) data to send
//      - (size_t) size of data to send
// Returns: 
//      - (uint8_t) error code
// Desription:  Sends data to the peer (currently supports only one peer)
uint8_t espnow_send(uint8_t *data, size_t size)
{
    uint32_t attempt = 0;
    uint16_t cur_index = msg_index; 

    if(!wifi_espnow_enabled())
        espnow_init();

    while((cur_index == msg_index) && (msg_ack != ESPNOW_ACK))
    {
        esp_now_send(peermac_addr, data, size);

        // wait until ACK or timeout
        while(!msg_ack)
            delay(DELAY_INTERVAL);
        
        // TESTING ONLY
        ESP_LOGI(TAG, "sent msg, index: %x, msg_ack: %x\n", msg_index, msg_ack);

        if(msg_ack == ESPNOW_ACK)
            {msg_ack = 0; break;}
        
        if(attempt++ > MAX_SEND_ATTEMPTS-1)
           {msg_ack = 0; return ERROR_CODE_FAIL;}

        msg_ack = 0;

        // delay for retry by going back to light sleep
        set_modem_sleep();
        set_custom_lightsleep(RETRY_DELAY);
        wake_modem_sleep();

        delay(200); // I hate watchdogs        
    }

    return ERROR_CODE_SUCCESS;
}

// Function:    espnow_get_message_index()
// Params:      N/A
// Returns:  
//      - (uint16_t) next message index to be used
// Desription:  Returns next message index to be used for the espnow_msg_t struct
uint16_t espnow_get_message_index(void)
{
    return msg_index;
}