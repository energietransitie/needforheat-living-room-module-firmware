#ifndef __CRC_H__
#define __CRC_H__

#include <stdint.h>

uint8_t generate_crc(const uint16_t* data, uint16_t count);

#endif // __CRC_H__