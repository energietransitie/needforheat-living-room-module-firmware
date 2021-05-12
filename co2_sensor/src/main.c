#include "../include/usart.h"
#include "../include/spi.h"
#include "../include/util.h"
#include "../include/i2c.h"
#include "../include/ModemSleep.h"
#include "../include/lightsleep.h"
#include "../include/Wifi.h"

#include "../lib/generic_esp_32/generic_esp_32.h"

#include <string.h>
#include <Stdio.h>
#include <stdlib.h>

// TODO: remove
#include <driver/i2c.h>

// TODO: add to sensor module when development on that begins
#define SCD41       0x62


void app_main() 
{
    //const char *str = "Hello, World!\n";
    //usart_init(115200);

    initialize_wifi();

    gpio_init();
    //spi_init();
    eink_init();
    // currently for testing usart and i2c
    uint8_t err;
    char str[256];

    usart_init(115200);
    i2c_init();
    
    // FXIME: remove when not necessary anymore
    // the SCD41 takes 1 second to initialize itself, that's what this delay is for
    delay(1024);
    
    // start periodic measurement
    err = i2c_write(SCD41, 0x21b1, I2C_NO_STOP);

    sprintf(&str[0], "init err = %d\n", err);
    usart_write(&str[0], strlen(&str[0])); 
    
    delay(1000);

    // spi_device_handle_t handle;
    // spi_device_interface_config_t conf = {
    //     .address_bits = 8, // geen idee of dit klopt
    //     .command_bits = 9,
    //     .mode = 1,
    //     .duty_cycle_pos = 128,

    // };
    // spi_bus_add_device(SPI2_HOST, )
    
    while(1)
    { 
    //    uint8_t data[2] = {0, 0x12};
        
        //usart_write(str, strlen(str)); 
    //    spi_write_data(NULL , &data[0], 2);

        uint8_t buffer[9];
        uint16_t word = 0;

        // check if there is a measurement available
        err = i2c_write(SCD41, 0xe4b8, I2C_NO_STOP);
        delay(1);
        err = i2c_read(SCD41, (uint8_t *) &buffer[0], 3);

        // if one of or more of the last 11 bits are non-zero, a measurement
        // is available otherwise wait 100 ms and try again
        word = (buffer[0] << 8) | buffer[1];
        if(!(word & 0x7FFF))
        {delay(100); continue;}

        usart_write("Done waiting!\n", 14);
            

        // read measurement
        err = i2c_write(SCD41, 0xec05, I2C_NO_STOP);
        delay(1);
        err = i2c_read(SCD41, (uint8_t *) &buffer[0], 9);

        // calculate temperature
        uint16_t a = ((buffer[3] << 8) | buffer[4]);
        float temp = -45 + 175 * (float)a / 65536;

        // print ppm, temperature and error code of the last i2c_read
        sprintf(&str[0], "co2: %d, temp: %f, err = %d\n", (buffer[0] << 8) | buffer[1], temp, err);
        usart_write(&str[0], strlen(&str[0])); 

        word = 0;
        delay(1000);

        // // set in modem sleep
        // setModemSleep();
        // printf("\nin modem sleep\n");

        // delay(10000);

        // // wake up from modem sleep
        // wakeModemSleep();
        // printf("\nweer uit modem sleep\n");
        // delay(10000);

        //light_sleep_start();

        send_HTTPS();
    }
}