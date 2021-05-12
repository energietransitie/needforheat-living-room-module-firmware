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
#define SCD41               0x62     
#define CAL_DISABLE_DATA    0x2416          // see chapter 4 in SCD41 datasheet
#define CAL_CHECK_DISABLE   0x2313          // see chapter 4 in SCD41 datasheet
#define CAL_DATA_MASK       0x241600


void app_main() 
{
    // ---- PERIPHERALS INITIALIZATION ---- //
    // gpio_init();
    // spi_init();
    // eink_init();
    //delay(1000);
    //usart_init(115200);
    //i2c_init();

    // ---- NEEDED FOR WI-FI ---- //
    // initialize_nvs();
    // initialize();
    // ESP_ERROR_CHECK(esp_netif_init());
    // enableWiFi();
    // initialize_time("CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00");

    // ---- GLOBAL VARIABLES ---- //
    //char str[256];
    //uint8_t err;
    //uint8_t buffer[3];
    initialize_wifi();

    // ---- DISABLE AUTOMATIC CALIBRATION ---- //
    //err = i2c_write(SCD41, (CAL_DISABLE_DATA & CAL_DATA_MASK), I2C_NO_STOP);
    //ESP_ERROR_CHECK(err);
    //delay(1);

    //err = i2c_write(SCD41, CAL_CHECK_DISABLE, I2C_NO_STOP);
    //delay(1);

    //err = i2c_read(SCD41, &buffer[0], 3);
    //delay(1);
    usart_init(115200);
    i2c_init();
    
    // FXIME: remove when not necessary anymore
    // the SCD41 takes 1 second to initialize itself, that's what this delay is for
    delay(1024);
    
    // start periodic measurement
    //err = i2c_write(SCD41, 0x21b1, I2C_NO_STOP);

    //sprintf(&str[0], "init err = %d\n", err);
    //usart_write(&str[0], strlen(&str[0])); 
    
    //delay(1000);

    //sprintf(&str[0], "return value = %d", ((buffer[0] << 8) | buffer[1]));
    //usart_write(&str[0], strlen(&str[0]));
    // ---- END OF CALIBRATION CODE ---- //

    // ---- FXIME: remove when not necessary anymore ---- //
    // the SCD41 takes 1 second to initialize itself, that's what this delay is for
    //delay(1024);
    
    // ---- START PERIODIC MEASUREMENT ---- //
    //err = i2c_write(SCD41, 0x21b1, I2C_NO_STOP);
    //sprintf(&str[0], "init err = %d\n", err);
    //usart_write(&str[0], strlen(&str[0])); 
    //delay(1000);
    
    while(1)
    { 
        //delay(1000);
        // uint8_t data[10] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
    //    uint8_t data[2] = {0, 0x12};
        
        // usart_write(str, strlen(str));

        // spi_write_data(NULL , &data[0], 2);

        // spi_write_command(NULL, 0x12);
        // spi_write_command(NULL, 0x24);
        // spi_write_data(NULL, &data[0], 10);
        // spi_write_command(NULL, 0x20);

        // uint8_t data[2] = {0, 0x12};
        // usart_write(str, strlen(str)); 
        // spi_write_data(NULL , &data[0], 2);

        //uint8_t buffer[9];
        //uint16_t word = 0;

        // ---- CHECK FOR AVAILABLE MEASUREMENT ---- //
        //err = i2c_write(SCD41, 0xe4b8, I2C_NO_STOP);
        //delay(1);
        //err = i2c_read(SCD41, &buffer[0], 3);

        // if one of or more of the last 11 bits are non-zero, a measurement
        // is available otherwise wait 100 ms and try again
        //word = (buffer[0] << 8) | buffer[1];
        //if(!(word & 0x7FFF))
        //{delay(100); continue;}

        //usart_write("Done waiting!\n", 14);
            

        // read measurement
        //err = i2c_write(SCD41, 0xec05, I2C_NO_STOP);
        //delay(1);
        //err = i2c_read(SCD41, (uint8_t *) &buffer[0], 9);

        // ---- CALCULATE TEMPERATURE ---- //
        //uint16_t a = ((buffer[3] << 8) | buffer[4]);
        //float temp = -45 + 175 * (float)a / 65536;

        // ---- PRINT LAST I2C READ INFORMATION (ppm, temp, err_code) ---- //
        //sprintf(&str[0], "co2: %d, temp: %f, err = %d\n", (buffer[0] << 8) | buffer[1], temp, err);
        //usart_write(&str[0], strlen(&str[0])); 

        //word = 0;
        //delay(1000);

        // ---- LIGHT SLEEP ---- //
        // light_sleep_start();

        // delay(1000);

        // ---- MODEM SLEEP ---- //
        // set ESP32 in modem sleep
        setModemSleep();
        printf("\nin modem sleep\n");
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
        // wake ESP32 up from modem sleep
        wakeModemSleep();
        printf("\nout of modem sleep\n");
        delay(5000);
        //send_HTTPS();
    }
}