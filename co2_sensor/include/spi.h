#ifndef __SPI_H__
#define __SPI_H__

// INCLUDE FILES
#include <stdint.h>
#include <driver/spi_master.h>

// VARIABLES
spi_device_handle_t spi;

// FUNCTION PROTOTYPES
void gpio_init(void);
void spi_init(void);
void eink_init(void);
void spi_write_command(spi_device_handle_t spi, uint8_t cmd);
void spi_write_data(spi_device_handle_t spi, uint8_t *data, int len);

#endif //__SPI_H__