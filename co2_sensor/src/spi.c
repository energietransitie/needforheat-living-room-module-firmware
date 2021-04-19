// INCLUDE FILES
#include <stdio.h>
#include <stdint.h>
#include <driver/spi_common.h>
#include <driver/spi_master.h>
#include "driver/gpio.h"
#include "spi.h"

// DEFINES
    // esp32 pins
#define PIN_MOSI        23
#define PIN_SCK         18
#define PIN_CS          16
#define TFT_DC          5
#define TFT_RST         17
    // other magic numbers
#define BYTE_SIZE       8
#define CLK_SPEED       5000000
#define DMA_DISABLE     0
#define LOW             0
#define HIGH            1

// Function:    gpio_init()
// Params:      N/A
// Returns:     N/A
// Description: Used to configure the correct GPIO pins used for SPI
void gpio_init()
{
    gpio_set_direction(PIN_MOSI, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_SCK, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_CS, GPIO_MODE_OUTPUT);
    gpio_set_level(TFT_DC, LOW);                   // Because we are using 3-wire SPI (without MISO), TFT_DC needs to be driven LOW
}

spi_device_handle_t spi_handle;
// Function:    spi_init()
// Params:      N/A
// Returns:     N/A
// Description: Used to configure SPI
void spi_init()
{
    esp_err_t err;
    

    spi_bus_config_t bus_cfg = {
        .mosi_io_num      = PIN_MOSI,   // setup MOSI pin
        .miso_io_num      = -1,         // MISO not used
        .sclk_io_num      = PIN_SCK,    // setup Clock pin
        .quadwp_io_num    = -1,         // no write protect
        .quadhd_io_num    = -1,         // no pin for hold signal
        .max_transfer_sz  =  16,        // max data transfer size 16 bytes
        .flags            = -1,         // no flags used
        .intr_flags       = -1          // no interrupt flags used
    };

    spi_device_interface_config_t dev_interface_cfg = {
        .command_bits       = BYTE_SIZE+1,        // amount of bits in command phase
        .address_bits       = BYTE_SIZE,        // amount of bits in address phase
        .dummy_bits         = 0,                   // bits to insert between address and data phase
        .mode               = 1,                    // SPI mode 2 - can drive up to 3 slaves
        .duty_cycle_pos     = 128,                    // duty cycle of positive clock
        .cs_ena_pretrans    = 0,                   // amount of SPI bit-cycles the cs should be activated before the transmission
        .cs_ena_posttrans   = 1,                   // amount of SPI bit-cycles the cs should stay active after the transmission
        .clock_speed_hz     = SPI_MASTER_FREQ_8M,   // clock speed 8 MHz
        .input_delay_ns     = 0,                    // maximum data valid time of slave
        .spics_io_num       = PIN_CS,               // setup chip select pin
        .flags              = -1,                   // no flags used
        .queue_size         = 1,                    // transaction queue size
    };

    err = spi_bus_initialize(SPI2_HOST, &bus_cfg, DMA_DISABLE);         // initialise the SPI bus
    ESP_ERROR_CHECK(err);

    err = spi_bus_add_device(SPI2_HOST, &dev_interface_cfg, &spi_handle);      // add LCD to the SPI bus
    ESP_ERROR_CHECK(err);
}

// Function:    eink_init()
// Params:      N/A
// Returns:     N/A
// Description: Used to initialise the e-ink display
void eink_init()
{
    gpio_set_direction(TFT_RST, GPIO_MODE_OUTPUT);
    gpio_set_level(TFT_RST, 0);
}

// Function:    spi_write_command()
// Params:
//      - (spi_device_handle_t) handle for a device on a SPI bus
//      - (uint8_t) command to be sent
// Returns:     N/A
// Description: Used to write a command to the SPI line
void spi_write_command(spi_device_handle_t spi, uint8_t cmd)
{
    esp_err_t err;
    spi_transaction_t t;
    t.length    = BYTE_SIZE+1;                        // 8-bit command                   
    t.tx_buffer = &cmd;                             // fill transmit buffer with the command
    t.user      = (void*)0;                         // DC low (command)
    err = spi_device_transmit(spi, &t);     // transmit command 
    ESP_ERROR_CHECK(err);
}

// Function:    spi_write_data()
// Params:
//      - (spi_device_handle_t) handle for a device on a SPI bus
//      - (uint8_t*) pointer to data buffer
//      - (int) max length of the data to be sent
// Returns:     N/A
// Description: Used to write data to the SPI line
void spi_write_data(spi_device_handle_t spi, uint8_t* data, int len)
{
    esp_err_t err;
    spi_transaction_t t;
    if (len == 0)   return;             // no need to send data
    t.length = len * BYTE_SIZE;         // len is in bytes, transaction len in bits
    t.tx_buffer = data;                 // fill transmit buffer with data
    t.rx_buffer = NULL;                 // we don't need to receive data
    t.user      = (void*)1;             // DC high (data)
    err = spi_device_transmit(spi_handle, &t);
    ESP_ERROR_CHECK(err); 
}