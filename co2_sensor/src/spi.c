// INCLUDE FILES
#include <stdio.h>
#include <stdint.h>
#include <driver/spi_common.h>
#include <driver/spi_master.h>
#include "driver/gpio.h"
#include "spi.h"

#include <driver/ledc.h>

#include <string.h>

// DEFINES
    // esp32 pins
//#define PIN_MISO        GPIO_NUM_19
#define PIN_MOSI        GPIO_NUM_23
#define PIN_SCK         GPIO_NUM_18 
#define PIN_CS          GPIO_NUM_26 
#define TFT_DC          GPIO_NUM_5
#define DMA_CHANNEL     2
//#define TFT_RST         GPIO_NUM_17
    // other magic numbers
#define BYTE_SIZE       8
#define CLK_SPEED       5000000
#define DMA_DISABLE     0
#define LOW             0
#define HIGH            1

spi_device_handle_t spi_handle;

// Function:    spi_init()
// Params:      N/A
// Returns:     N/A
// Description: Used to configure SPI
void spi_init()
{
    esp_err_t err;
    
    spi_bus_config_t bus_cfg = {
        .mosi_io_num      = PIN_MOSI,
        .miso_io_num      = -1,         // MISO not used
        .sclk_io_num      = PIN_SCK,
        .quadwp_io_num    = -1,         // no write protect
        .quadhd_io_num    = -1,         // no pin for hold signal
        .max_transfer_sz  = 4096,
    };
    

    // setup EPD (TODO: put this in a seperate module that handles EPD things)
    spi_device_interface_config_t dev_interface_cfg = {
        .command_bits       = BYTE_SIZE,
        .address_bits       = 0,
        .mode               = 0,                     // CPHA = 0, CPOL = 0
        .duty_cycle_pos     = 0,                    // duty cycle of positive clock (50%)
        .clock_speed_hz     = 1000000,              // clock speed 1 MHz (between 1-5 MHz are the frequencies with 50% duty cycles, higher does not work)
        .input_delay_ns     = 0,                    // maximum data valid time of slave
        .spics_io_num       = PIN_CS,              
        .flags              = SPI_DEVICE_HALFDUPLEX, // use half duplex, seems to reolve some reliabillity issues
        .queue_size         = 7,
    };

    //spi_init();
    
    // error args
    err = spi_bus_initialize(SPI2_HOST, &bus_cfg, 0);                 // initialise the SPI bus
    ESP_ERROR_CHECK(err);

    err = spi_bus_add_device(SPI2_HOST, &dev_interface_cfg, &spi_handle);      // add LCD to the SPI bus
    ESP_ERROR_CHECK(err);
}


// Function:    spi_gpio_init()
// Params:      N/A
// Returns:     N/A
// Description: Used to initialise GPIO pins for SPI
void spi_gpio_init(void)
{    
    gpio_set_direction(TFT_DC, GPIO_MODE_OUTPUT);
    gpio_pulldown_en(TFT_DC);
    gpio_set_level(TFT_DC, 0);
}

// Function:    spi_write_command()
// Params:
//      - (spi_device_handle_t) handle for a device on a SPI bus [NOT USED]
//      - (uint8_t) command to be sent
// Returns:     N/A
// Description: Used to write a command to the SPI line
void spi_write_command(spi_device_handle_t spi, uint8_t cmd)
{
    esp_err_t err;
    spi_transaction_ext_t t;

    memset(&t, 0, sizeof(t));

    t.base.length    = 8;                        // 8-bit command 
    t.base.cmd = cmd; 
    t.base.addr = (uint16_t) 0;                             // fill transmit buffer with the command
    t.base.flags = SPI_TRANS_VARIABLE_ADDR | SPI_TRANS_VARIABLE_CMD;
    t.address_bits = 0;
    t.command_bits = 8;

    gpio_set_level(TFT_DC, 0);
    err = spi_device_polling_transmit(spi_handle, &t.base);           // transmit command
    ESP_ERROR_CHECK(err);
}

// Function:    spi_write_data()
// Params:
//      - (spi_device_handle_t) handle for a device on a SPI bus [NOT USED]
//      - (uint8_t*) pointer to data buffer
//      - (int) max length of the data to be sent
// Returns:     N/A
// Description: Used to write data to the SPI line
void spi_write_data(spi_device_handle_t spi, uint8_t* data, int len)
{
    esp_err_t err;
    spi_transaction_ext_t t;

    if (len == 0)   return;             // no need to send data
    memset(&t, 0, sizeof(t));

    t.base.length    = BYTE_SIZE + (len-1) * BYTE_SIZE;                        // 8-bit command 
    t.base.cmd = 0; 
    t.base.addr = (uint16_t) 0;                             // fill transmit buffer with the command
    t.base.flags = SPI_TRANS_VARIABLE_ADDR | SPI_TRANS_VARIABLE_CMD;
    t.base.tx_buffer = data;
    t.address_bits = 0;
    t.command_bits = 0;
    
    gpio_set_level(TFT_DC, 1);
    err = spi_device_polling_transmit(spi_handle, &t.base);
    ESP_ERROR_CHECK(err); 
}