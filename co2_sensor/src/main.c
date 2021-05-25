#include "../include/usart.h"
#include "../include/spi.h"
#include "../include/util.h"
#include "../include/i2c.h"
#include "../include/sleepmodes.h"
#include "../include/Wifi.h"
#include "../lib/generic_esp_32/generic_esp_32.h"
#include "../include/timer.h"

#include <string.h>
#include <Stdio.h>
#include <stdlib.h>

// TODO: remove
#include <driver/i2c.h>

// TODO: add to sensor module when development on that begins
#define SCD41               0x62     

void app_main() 
{
    isSending = false;
    // ---- PERIPHERALS INITIALIZATION ---- //
    //gpio_init();
    //spi_init();
    usart_init(115200);
    i2c_init();

    // ---- WI-FI ---- //
    initialize_wifi();

    // ---- GLOBAL VARIABLES ---- //
    //char str[256];
    uint8_t err;
    // uint8_t buffer_calibration[3];
    // int data_disable = 0x2416;
    // int check_disable = 0x2313;
    // int mask = 0xffff00;

    // ---- DISABLE AUTOMATIC CALIBRATION ---- //
    // CRC
    //crc8_be(0, &buffer_calibration[0], 3);

    //err = i2c_write(SCD41, ((data_disable << 8) & mask), I2C_NO_STOP);
    //ESP_ERROR_CHECK(err);
    //delay(1);

    // err = i2c_write(SCD41, check_disable, I2C_NO_STOP);
    // ESP_ERROR_CHECK(err);
    // delay(1);

    // CHECK IF DISABLING WENT CORRECTLY
    // err = i2c_read(SCD41, &buffer_calibration[0], 3);
    // ESP_ERROR_CHECK(err);
    // delay(1);
    
    // PRINT OUT THE RESULTS
    // sprintf(&str[0], "return value = %d", ((buffer_calibration[0] << 8) | buffer_calibration[1]));
    // usart_write(&str[0], strlen(&str[0]));

    // ---- remove when not necessary anymore ---- //
    // the SCD41 takes 1 second to initialize itself, that's what this delay is for
    delay(1024);
    
    // ---- START PERIODIC MEASUREMENT ---- //
    err = i2c_write(SCD41, 0x21b1, I2C_NO_STOP, 2);
    delay(1000);

    init_timer();
    
    while(1)
    { 
        // uint8_t read_buffer[9];
        // uint16_t word = 0;

        // // ---- CHECK FOR AVAILABLE MEASUREMENT ---- //
        // err = i2c_write(SCD41, 0xe4b8, I2C_NO_STOP, 2);
        // delay(1);
        // err = i2c_read(SCD41, &read_buffer[0], 3);

        // // if one of or more of the last 11 bits are non-zero, a measurement
        // // is available otherwise wait 100 ms and try again
        // word = (read_buffer[0] << 8) | read_buffer[1];
        // if(!(word & 0x7FFF))
        // {
        //     delay(100); 
        //     continue;
        // }
            
        // // ---- READ MEASUREMENT ---- //
        // err = i2c_write(SCD41, 0xec05, I2C_NO_STOP, 2);
        // delay(1);
        // err = i2c_read(SCD41, (uint8_t *) &read_buffer[0], 9);

        // // ---- CALCULATE CO2 ---- //
        // uint16_t co2 = ((read_buffer[0] << 8) | read_buffer[1]);

        // // ---- CALCULATE TEMPERATURE ---- //
        // uint16_t b = ((read_buffer[3] << 8) | read_buffer[4]);
        // float temp = -45 + 175 * (float)b / 65536;

        // // ---- CALCULATE HUMIDITY ---- //
        // uint16_t c = ((read_buffer[6] << 8) | read_buffer[7]);
        // uint16_t rht = 100 * c / 65536;

        // // ---- PRINT LAST I2C READ INFORMATION (ppm, temp, err_code) ---- //
        // sprintf(&str[0], "co2: %dppm, temp: %.2f°C, rht: %d%%\n",co2 , temp, rht);
        // usart_write(&str[0], strlen(&str[0])); 

        // word = 0;

        read_timer();
        vTaskDelay(10);
        if(isSending){
            send_HTTPS();
            isSending = false;
        }

        // ---- SWITCH BETWEEN MODEM AND LIGHT SLEEP ---- //
        //switch_modes();

        // ---- SEND DATA VIA HTTPS ---- //
        //send_HTTPS();
    }
}