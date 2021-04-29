#include "../include/i2c.h"
#include "../include/errorcode.h"

#include <driver/i2c.h>
#include <driver/gpio.h>
#include <esp32/rom/crc.h>

#include <stdlib.h>

// ==============================
//          DEFINES
// ==============================
#define I2C_MASTER_SDA_IO       GPIO_NUM_21
#define I2C_MASTER_SCL_IO       GPIO_NUM_22
#define I2C_MASTER_FREQ         10000    // hz

#define TIMEOUT_VAL             1040000

#define I2C_PORT                I2C_NUM_0

#define ACK_EN                  (bool) 1

#define I2C_MAX_TRIES           3

#define I2C_STATUS_REGISTER     0x3FF5302Cu     // see chapter 11.4 in reference manual
#define I2C_STATUS_TIMEOUT      (1 << 9)        // see chapter 11.4 in reference manual

uint8_t flag = 0;

void i2c_isr(void *arg);

// Function:    i2c_init()
// Params:      N/A
// Returns:     N/A
// Description: Used to config i2c for use by the sensor modules
void i2c_init(void)
{
    // config i2c (master mode)
    i2c_config_t conf = {
    .mode = I2C_MODE_MASTER,
    .sda_io_num = I2C_MASTER_SDA_IO,
    .sda_pullup_en = GPIO_PULLUP_ENABLE,
    .scl_io_num = I2C_MASTER_SCL_IO,
    .scl_pullup_en = GPIO_PULLUP_ENABLE,
    .master.clk_speed = I2C_MASTER_FREQ,  
    };
    
    // configure driver
    ESP_ERROR_CHECK(i2c_param_config(I2C_PORT, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_PORT, conf.mode, 0, 0, ESP_INTR_FLAG_LEVEL3)); 

    // configure ISR (could not be tested properly, yet)
    //intr_handle_t handle;
    //ESP_ERROR_CHECK(esp_intr_alloc_intrstatus(ETS_I2C_EXT0_INTR_SOURCE, ESP_INTR_FLAG_LEVEL3, I2C_STATUS_REGISTER, I2C_STATUS_TIMEOUT, i2c_isr, NULL, &handle));
    //ESP_ERROR_CHECK(i2c_isr_register(I2C_PORT, i2c_isr, NULL, ESP_INTR_FLAG_LEVEL3, &handle));
    
    // set a max. time out for 13ms
    i2c_set_timeout(I2C_PORT, TIMEOUT_VAL);
}

// Function:        i2c_isr()
// Params:
//      - (void *) arg --> ignored
// Returns:         N/A
// Description:     [FOR FUTURE REFERENCE] --> this may be needed in the future
//                  i2c interrupt handler, which is used to allow clock stretching
//                  it is (for now) only called when a time out occurs
void IRAM_ATTR i2c_isr(void *arg)
{
    // reset flag
    uint32_t *status = (uint32_t *) I2C_STATUS_REGISTER;

    i2c_reset_tx_fifo(I2C_PORT);
    i2c_reset_rx_fifo(I2C_PORT);

    flag = 0xff;

    *(status) &= ~I2C_STATUS_TIMEOUT;
}

// Function:    i2c_read()
// Params:      
//      - (uint8_t) slave address
//      - (uint8_t) amound of bytes to be received
//      - (uint8_t) error
// Returns:      
//      - (uint8_t) pointer to data (malloc(), so you need to free() when done)
// Description: Used to write a command and read the result byte from the i2c bus (can only read 8 bits at a time)
uint8_t i2c_read(uint8_t address, uint8_t *buffer, size_t nbytes)
{
    static uint8_t tries = 0;
    uint8_t *data = buffer;

    if(tries >= I2C_MAX_TRIES-1)
        return ERROR_CODE_FAIL; // tried max. times so it's time to stop and give up
        
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_READ, 1);

    // read all bytes and send an ACK value (0)
    for(uint32_t i = 0; i < (nbytes-1); ++i)
        i2c_master_read_byte(cmd, &data[i], 0);

    // read last byte and send NACK value
    i2c_master_read_byte(cmd, &data[nbytes-1], 1);      
    i2c_master_stop(cmd);

    // send command and byte as soon as possible
    esp_err_t err = i2c_master_cmd_begin(I2C_PORT, cmd, 100);
    i2c_cmd_link_delete(cmd);

    if(err == ESP_FAIL || err == ESP_ERR_TIMEOUT)
    {
        // try again if fail
        tries++;
        i2c_read(address, buffer, nbytes);
    }
    else if(err == ESP_ERR_INVALID_STATE || err == ESP_ERR_INVALID_ARG)
        return ERROR_CODE_INVALID; 
    
    
    tries = 0;
    return ERROR_CODE_SUCCESS;
}

// Function:    i2c_write()
// Params:
//      - (uint8_t) slave address
//      - (uint16_t) word to write
// Returns:     
//      - (uint8_t) Error code
// Desription: Used to write a byte to the i2c bus
uint8_t i2c_write(uint8_t address, uint16_t word, uint8_t stop)
{
    static uint8_t tries = 0;

    if(tries >= I2C_MAX_TRIES-1)
        return ERROR_CODE_FAIL; // tried max. times so it's time to stop and give up
    
    uint8_t buffer[2] = {(word >> 8) & 0xFF, (word & 0xFF)};

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, ACK_EN);
    i2c_master_write_byte(cmd, (uint8_t) buffer[0], ACK_EN);
    i2c_master_write_byte(cmd, (uint8_t) buffer[1], ACK_EN);
    
    // when a command is sent that has a result that needs to be read
    // the SCD41 does not expect a STOP condition
    // so that's why this is here
    if(stop)
        i2c_master_stop(cmd);

    // send command and byte as soon as possible
    esp_err_t err = i2c_master_cmd_begin(I2C_PORT, cmd, 100);
    i2c_cmd_link_delete(cmd);

    if(err == ESP_FAIL || err == ESP_ERR_TIMEOUT)
    {
        // try again if fail
        tries++;
        i2c_write(address, word, stop);
    }
    else if(err == ESP_ERR_INVALID_STATE || err == ESP_ERR_INVALID_ARG)
        return ERROR_CODE_INVALID;
    
    tries = 0;
    return ERROR_CODE_SUCCESS;
}