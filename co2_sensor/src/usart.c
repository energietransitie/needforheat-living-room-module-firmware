#include "../include/usart.h"

#include <driver/uart.h>
#include <driver/gpio.h>

#define USART_BUFFER_SIZE       2048            // bytes
#define USART_BUFFER_DEPTH      10              // # of buffers
#define USART_PORT              UART_NUM_0

#define TIMEOUT                 100             // ticks

QueueHandle_t usartq;

// Function:    usart_init()
// Params:      N/A
// Returns:     N/A
// Description: Used to config usart for debugging purposes
void usart_init(uint32_t baud_rate)
{
    uart_config_t uart_config = {
    .baud_rate = baud_rate,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .rx_flow_ctrl_thresh = 122,
    };

    ESP_ERROR_CHECK(uart_param_config(USART_PORT, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(USART_PORT, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, GPIO_NUM_18, GPIO_NUM_19));
    ESP_ERROR_CHECK(uart_driver_install(USART_PORT, USART_BUFFER_SIZE, USART_BUFFER_SIZE, USART_BUFFER_DEPTH, &usartq, 0));
}

// Function:    usart_write()
// Params:
//      - (char *) pointer to data buffer
//      - (size_t) size of data buffer
// Returns:     N/A
// Description: Used to write text to a monitor console
void usart_write(const char *bfr, size_t size)
{
    uart_write_bytes(USART_PORT, bfr, size);
    ESP_ERROR_CHECK(uart_wait_tx_done(USART_PORT, TIMEOUT));
}