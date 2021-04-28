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
#define PIN_SCK         GPIO_NUM_18  // 18
#define PIN_CS          GPIO_NUM_26 // 17
#define TFT_DC          GPIO_NUM_5
#define DMA_CHANNEL     2
//#define TFT_RST         GPIO_NUM_17
    // other magic numbers
#define BYTE_SIZE       8
#define CLK_SPEED       5000000
#define DMA_DISABLE     0
#define LOW             0
#define HIGH            1

// void spi_init()
// {
//     ledc_timer_config_t t_config = 
//     {
//         .speed_mode = LEDC_HIGH_SPEED_MODE,
//         .timer_num = 1,
//         .freq_hz = 20 * 1000 * 1000,
//         .duty_resolution = LEDC_TIMER_1_BIT,
//     };

//     ledc_channel_config_t c_config = 
//     {
//         .channel = 0,
//         .gpio_num = PIN_SCK,
//         .timer_sel = 1,
//         .hpoint = 1,
//         .speed_mode = LEDC_HIGH_SPEED_MODE    
//     };

//     esp_err_t err = ledc_timer_config(&t_config);
//     ESP_ERROR_CHECK(err);

//     err = ledc_channel_config(&c_config);
//     ESP_ERROR_CHECK(err);
// }

// // Function:    gpio_init()
// // Params:      N/A
// // Returns:     N/A
// // Description: Used to configure the correct GPIO pins used for SPI
// void gpio_init()
// {
//     //gpio_set_direction(PIN_MOSI, GPIO_MODE_OUTPUT);
//     //gpio_set_direction(PIN_SCK, GPIO_MODE_OUTPUT);
//     //gpio_set_direction(PIN_CS, GPIO_MODE_OUTPUT);
//     //gpio_set_level(TFT_DC, LOW);                   // Because we are using 3-wire SPI (without MISO), TFT_DC needs to be driven LOW
// }
// void set_dc_callback(spi_transaction_t *t);
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
        .max_transfer_sz  = 4096,        // max data transfer size 16 bytes
        //.flags            = -1,         // no flags used
        //.intr_flags       = -1          // no interrupt flags used
    };
    
    spi_device_interface_config_t dev_interface_cfg = {
        .command_bits       = BYTE_SIZE,        // amount of bits in command phase
        .address_bits       = 0, //BYTE_SIZE,        // amount of bits in address phase
        //.dummy_bits         = 0,                   // bits to insert between address and data phase
        .mode               = 0, //1,                    // SPI mode 2 - can drive up to 3 slaves
        .duty_cycle_pos     = 0,                    // duty cycle of positive clock
        //.cs_ena_pretrans    = 3,                   // amount of SPI bit-cycles the cs should be activated before the transmission
        //.cs_ena_posttrans   = 14,                   // amount of SPI bit-cycles the cs should stay active after the transmission
        .clock_speed_hz     = 1000000,//SPI_MASTER_FREQ_8M, //spi_get_actual_clock(APB_CLK_FREQ, 20*1000*1000, 50),   // clock speed 8 MHz
        .input_delay_ns     = 0,                    // maximum data valid time of slave
        .spics_io_num       = PIN_CS,               // setup chip select pin
        .flags              = SPI_DEVICE_HALFDUPLEX,                   // no flags used
        .queue_size         = 7,                    // transaction queue size
        //.pre_cb=set_dc_callback
    };

    //spi_init();
    
    // error args
    err = spi_bus_initialize(SPI2_HOST, &bus_cfg, 0);                 // initialise the SPI bus
    ESP_ERROR_CHECK(err);

    err = spi_bus_add_device(SPI2_HOST, &dev_interface_cfg, &spi_handle);      // add LCD to the SPI bus
    ESP_ERROR_CHECK(err);
}

// void set_dc_callback(spi_transaction_t *t)
// {
//     gpio_set_level(TFT_DC, (uint32_t) (t->user));
// }

// Function:    eink_init()
// Params:      N/A
// Returns:     N/A
// Description: Used to initialise the e-ink display
void eink_init()
{
    //gpio_set_direction(TFT_RST, GPIO_MODE_OUTPUT);
    //gpio_set_level(TFT_RST, 0);

    
    gpio_set_direction(TFT_DC, GPIO_MODE_OUTPUT);
    gpio_pulldown_en(TFT_DC);
    gpio_set_level(TFT_DC, 0);

    //gpio_set_direction(PIN_CS, GPIO_MODE_OUTPUT);
    //gpio_pullup_en(TFT_DC);
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
    spi_transaction_ext_t t;

    //uint32_t data = 0x000012;

    memset(&t, 0, sizeof(t));

    t.base.length    = 8;                        // 8-bit command 
    t.base.cmd = cmd; 
    t.base.addr = (uint16_t) 0;                             // fill transmit buffer with the command
    t.base.flags = SPI_TRANS_VARIABLE_ADDR | SPI_TRANS_VARIABLE_CMD;
    t.address_bits = 0;
    t.command_bits = 8;
    //t.user      = (void*)0;                         // DC low (command)

    gpio_set_level(TFT_DC, 0);
    err = spi_device_polling_transmit(spi_handle, &t.base);//spi_device_transmit(spi_handle, &t);             // transmit command spi_device_queue_trans(spi_handle, &t, 1000); //
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
    spi_transaction_ext_t t;

    if (len == 0)   return;             // no need to send data
    memset(&t, 0, sizeof(t));

    //t.length = BYTE_SIZE + (len-1) * BYTE_SIZE;         // len is in bytes, transaction len in bits
    //t.tx_buffer = data;                 // fill transmit buffer with data
    //t.rx_buffer = NULL;                 // we don't need to receive data

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