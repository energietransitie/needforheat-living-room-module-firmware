#include "../include/usart.h"
#include "../include/spi.h"
#include <string.h>

void app_main() 
{
    //const char *str = "Hello, World!\n";
    //usart_init(115200);

    gpio_init();
    spi_init();
    eink_init();

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
        uint8_t data[2] = {0, 0x12};
        
        //usart_write(str, strlen(str)); 
        spi_write_data(NULL , &data[0], 2);
    }
}