#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdint.h>
#include <stddef.h>

void delay(uint32_t ms);
uint32_t *uint16_to_uint32(uint16_t *b, size_t s);
uint32_t *uint8_to_uint32(uint8_t *b, size_t s);

#endif // __UTIL_H__