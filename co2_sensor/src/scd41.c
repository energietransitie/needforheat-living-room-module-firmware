#include "../include/scd41.h"
#include "../include/i2c.h"
#include "../include/crc.h"
#include "../include/usart.h"
#include "../include/util.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define SCD41_ADDR              0x62

#define SCD41_CMD_SERIALNUM     0x3682
#define SCD41_CMD_SET_ASC_EN    0x2416
#define SCD41_CMD_GET_ASC_EN    0x2313
#define SCD41_CMD_READMEASURE   0xec05
#define SCD41_CMD_SINGLESHOT    0x219d

#define SCD41_CMD_GET_TEMP_OFF  0x2318


#define SCD41_BUFFER_SIZE       128
#define SCD41_STR_SIZE          64

#ifdef USE_FIXEDPOINT   // TODO
    #define FLOAT_TYPE  uint16_t
#else
    #define FLOAT_TYPE  float
#endif // USE_FIXEDPOINT

char str[SCD41_STR_SIZE];

volatile uint16_t *buffer_co2;
volatile FLOAT_TYPE *buffer_temp; // TODO: FIXED POINT?
volatile uint8_t *buffer_rht;
volatile uint8_t loc = 0;

// Function:    scd41_init()
// Params:      N/A
// Returns:     N/A
// Desription:  Initializes this module and the SCD41
void scd41_init(void)
{
    // the SCD41 takes 1 second to initialize itself, that's what this delay is for
    delay(1000);
    
    buffer_co2 = malloc(SCD41_BUFFER_SIZE * sizeof(uint16_t));
    buffer_temp = malloc(SCD41_BUFFER_SIZE * sizeof(FLOAT_TYPE));
    buffer_rht = malloc(SCD41_BUFFER_SIZE * sizeof(uint8_t));

    // TODO: non-periodic measurement stuff
    scd41_disable_asc();
    scd41_print_serial_number();
}

// Function:    scd41_disable_asc()
// Params:      N/A
// Returns:     N/A
// Desription:  Disables ASC (Automatic Self-Calibration)
void scd41_disable_asc(void)
{
    uint8_t cmd_buffer[5] = {(uint8_t)(SCD41_CMD_SET_ASC_EN >> 8), (uint8_t) SCD41_CMD_SET_ASC_EN & 0xFF, (uint8_t) 0, (uint8_t) 0, (uint8_t) 0};

    // generate CRC for data buffer
    uint16_t data[2] = {0, 0};
    cmd_buffer[4] = generate_crc((const uint16_t *) &data, 2);

    // TESTING ONLY
    sprintf(&str[0], "SCD41: CRC: %x\n", cmd_buffer[4]);
    usart_write(&str[0], strlen(&str[0]));

    // write command and data
    i2c_write(SCD41_ADDR, &cmd_buffer[0], I2C_STOP, 5); 
    
    // check if ASC is disabled or not
    cmd_buffer[0] = (uint8_t) (SCD41_CMD_GET_ASC_EN >> 8) & 0xFF;
    cmd_buffer[1] = (uint8_t) SCD41_CMD_GET_ASC_EN & 0xFF;
    cmd_buffer[2] = 0;

    delay(2);
    
    uint16_t r = 0;
    // get current ASC ENABLED value
    i2c_write(SCD41_ADDR, &cmd_buffer[0], I2C_NO_STOP, 2);
    delay(2);                               
    
    // I2C reads back previous command before giving us the
    // actual data we want, not supposed to happen
    // but it does so anyway
    i2c_read(SCD41_ADDR, (uint8_t *) &cmd_buffer[0], 3);

    // starting at 2 since [0] and [1] are the previous command 
    r = (cmd_buffer[0] << 8)  | cmd_buffer[1];
    if(r)
    {
        sprintf(&str[0], "SCD41: Warning: ASC not disabled (0x%x)\n", r);
        usart_write(&str[0], strlen(&str[0]));
    }
}

// Function:    scd41_measure_co2_temp_rht()
// Params:      N/A
// Returns:     N/A
// Desription:  Measures RHT, temperature and CO2 ppm in single shot mode
//              and stores this in a buffer
void scd41_measure_co2_temp_rht(void)
{
    uint8_t read_buffer[9];
    uint8_t cmd_buffer[2];
    
    // --- START SINGLE SHOT MEASUREMENT
    cmd_buffer[0] = (uint8_t) (SCD41_CMD_SINGLESHOT >> 8) & 0xFF;
    cmd_buffer[1] = (uint8_t) SCD41_CMD_SINGLESHOT & 0xFF;
    
    uint8_t err = i2c_write(SCD41_ADDR, (uint8_t *) &cmd_buffer[0], I2C_STOP, 2);
    delay(1350);

    // --- READ MEASUREMENT --- //
    cmd_buffer[0] = (uint8_t)(SCD41_CMD_READMEASURE >> 8);
    cmd_buffer[1] = (uint8_t) SCD41_CMD_READMEASURE & 0xFF;
    err = i2c_write(SCD41_ADDR, (uint8_t *) &cmd_buffer[0], I2C_NO_STOP, 2);
    delay(1);
    err = i2c_read(SCD41_ADDR, (uint8_t *) &read_buffer[0], 9);

    // ---- CALCULATE CO2 ---- //
    uint16_t co2 = ((read_buffer[0] << 8) | read_buffer[1]);
    buffer_co2[loc] = co2;

    // ---- CALCULATE TEMPERATURE ---- //
    uint16_t b = ((read_buffer[3] << 8) | read_buffer[4]);
    float temp = -45 + 175 * (float)b / 65536; // TODO: fixed point?
    buffer_temp[loc] = temp;

    // ---- CALCULATE HUMIDITY ---- //
    uint16_t c = ((read_buffer[6] << 8) | read_buffer[7]);
    uint8_t rht = (uint8_t) (100 * c / 65536);
    buffer_rht[loc] = rht;

    if(loc++ >= SCD41_BUFFER_SIZE)
    {
        // TODO: send
        scd41_reset_buffers();
    }

    // FOR TESTING ONLY
    sprintf(&str[0], "co2: %dppm, temp: %.2f°C, rht: %d%%\n", co2 , temp, rht);
    usart_write(&str[0], strlen(&str[0])); 
}

// Function:    scd41_reset_buffers()
// Params:      N/A
// Returns:     N/A
// Desription:  Resets all buffers
void scd41_reset_buffers(void)
{
    loc = 0;
    memset((uint16_t *) buffer_co2, 0, SCD41_BUFFER_SIZE);
    memset((uint16_t *) buffer_temp, 0, SCD41_BUFFER_SIZE);
    memset((uint8_t *) buffer_rht, 0, SCD41_BUFFER_SIZE);
}

// Function:    scd41_print_serial_number()
// Params:      N/A
// Returns:     N/A
// Desription:  Prints the serial number of the SCD41 to the
//              serial monitor via USART
void scd41_print_serial_number(void)
{
    uint8_t buffer[9];

    uint8_t cmd_buffer[2] = { (uint8_t)(SCD41_CMD_SERIALNUM >> 8), (uint8_t) SCD41_CMD_SERIALNUM & 0xFF};

    // get serial number
    uint8_t err = i2c_write(SCD41_ADDR, &cmd_buffer[0], I2C_NO_STOP, 2);
    delay(1);
    err = i2c_read(SCD41_ADDR, (uint8_t *) &buffer[0], 9);

    // since its a big endian number it's already in the right order
    uint64_t sn = ((uint64_t) buffer[0] << 40) | ((uint64_t) buffer[1] << 32) | ((uint64_t) buffer[3] << 24) | ((uint64_t) buffer[4] << 16) | ((uint64_t) buffer[6] << 8) | (uint64_t) buffer[7];

    sprintf(&str[0], "SCD41: Serial number: %lld\n", sn);
    usart_write(&str[0], strlen(&str[0]));
}