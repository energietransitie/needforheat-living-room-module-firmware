#include "../include/espnow.h"
#include "../include/usart.h"
#include "../include/errorcode.h"
#include "../include/util.h"

#include <esp_now.h>
#include <esp_wifi.h>
#include <esp_netif.h>
#include <wifi_provisioning/manager.h>
#include <string.h>
#include <stdbool.h>

#define DELAY_INTERVAL      2 // milliseconds
#define ESPNOW_ACK          1
#define ESPNOW_NACK         2

#define MAC_ADDR_SIZE       6   // bytes

#define MAC_ADDR_TEST_SENDER    {0xc4, 0x4f, 0x33, 0x7f, 0xae, 0x95} // LOLIN TFT/I2C SHIELD ESP
#define MAC_ADDR_TEST_RECVR     {0xc4, 0x4f, 0x33, 0x7f, 0xa8, 0x81} // SHIELD-LESS ESP
#define WIFI_CHANNEL            11

#define MAX_SEND_ATTEMPTS       0xFFFF

esp_now_peer_info_t info;           // needs to be global (ESP argument error if not)
volatile uint16_t msg_index = 0;
volatile uint8_t msg_ack = 0;

uint8_t mymac_addr[MAC_ADDR_SIZE],
        peermac_addr[MAC_ADDR_SIZE] = 
        
         #ifdef ESP_NOW_RECEIVER
            MAC_ADDR_TEST_SENDER;
        #else
            MAC_ADDR_TEST_RECVR;
        #endif 

// Function:    espnow_cb_ondatarecv()
// Params:      
//      - (const uint8_t *) mac address of sender
//      - (const uint8_t *) data received
//      - (const uint32_t) size of data buffer
// Returns:     N/A
// Desription:  Used for testing - this function is the onDataReceive callback used to test if sending data works
void espnow_cb_ondatarecv(const uint8_t *mac, const uint8_t *data, const uint32_t len)
{
    usart_write("received, eh... something\n", 28);

    espnow_msg_t *msg = (espnow_msg_t *) data;

    char str[128];

    // TESTING ONLY
    sprintf(&str[0], "type: %x, nmeasurements: %x, index: %x, co2: %i ppm\n", (msg->device_type), (msg->nmeasurements), (msg->index), (msg->co2[0]));
    usart_write(&str[0], strlen(&str[0]));
}

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
        usart_write("received ACK\n", 13); // testing only
        msg_index++;
        msg_ack = ESPNOW_ACK;
    }
    else
    {
        usart_write("received NACK -\n", 16); // testing only
        msg_ack = ESPNOW_NACK;
    }
}

// Function:    espnow_init()
// Params:      N/A
// Returns:     N/A
// Desription:  Initializes ESP-NOW
void espnow_init(void)
{
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_now_init());

    espnow_config_peer(WIFI_CHANNEL, false, &peermac_addr[0]);

    // register callbacks
    ESP_ERROR_CHECK(esp_now_register_recv_cb((esp_now_recv_cb_t) espnow_cb_ondatarecv)); // testing only
    ESP_ERROR_CHECK(esp_now_register_send_cb((esp_now_send_cb_t) espnow_cb_ondatasend));

    ESP_ERROR_CHECK(esp_wifi_get_mac(ESP_IF_WIFI_STA, mymac_addr));
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

    char str[40];

    // TESTING ONLY
    sprintf(&str[0], "peer mac: %x:%x:%x:%x:%x:%x\n", info.peer_addr[0], info.peer_addr[1], info.peer_addr[2], info.peer_addr[3], info.peer_addr[4], info.peer_addr[5]);
    usart_write(&str[0], strlen(&str[0]));

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
    char str[40];

    uint32_t attempt = 0;
    uint16_t cur_index = msg_index; 

    while((cur_index == msg_index) && (msg_ack != ESPNOW_ACK))
    {
        esp_now_send(peermac_addr, data, size);

        // wait until ACK or timeout
        while(!msg_ack)
            delay(DELAY_INTERVAL);

        msg_ack = 0;

        // TESTING ONLY
        sprintf(&str[0], "sent msg, index: %x, msg_ack: %x\n", msg_index, msg_ack);
        usart_write(&str[0], strlen(&str[0]));
        
        if(attempt++ > MAX_SEND_ATTEMPTS-1)
           {msg_ack = 0; return ERROR_CODE_FAIL;}
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