#ifndef __USART_H__
#define __USART_H__

#include <stdint.h>
#include <stddef.h>

void usart_init(uint32_t baud_rate);
void usart_write(const char *bfr, size_t size);

#endif // __USART_H__