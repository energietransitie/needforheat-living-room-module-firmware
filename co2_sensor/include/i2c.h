#ifndef __I2C_H__
#define __I2C_H__

#include <stdint.h>
#include <stddef.h>

#define I2C_STOP        1
#define I2C_NO_STOP     0

void i2c_init(void);
uint8_t i2c_read(uint8_t address, uint8_t *buffer, size_t nbytes);
uint8_t i2c_write(uint8_t address, uint16_t word, uint8_t stop, uint8_t bytes);

#endif // __I2C_H__