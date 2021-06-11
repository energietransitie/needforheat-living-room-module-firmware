#include "../include/util.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// Function:    delay()
// Params:      
//      - (uint32_t) milliseconds to wait 
// Returns:     N/A
// Desription:  Waits specified number of milliseconds
void delay(uint32_t ms)
{
    vTaskDelay(ms / portTICK_PERIOD_MS);
}

// Function:    uint16_to_uint32()
// Params:      
//      - (uint16_t *) buffer of uint16_t *
//      - (size_t) # of elements in buffer
// Returns:     N/A
// Desription:  copies a uint16_t * buffer to a uint32_t * buffer
uint32_t *uint16_to_uint32(uint16_t *b, size_t s)
{
    uint32_t *nb = malloc(s * sizeof(uint32_t));
    
    for(uint32_t i = 0; i < s; ++i)
        nb[i] = (uint32_t) b[i];

    return nb;
}

// Function:    uint8_to_uint32()
// Params:      
//      - (uint8_t *) buffer of uint16_t *
//      - (size_t) # of elements in buffer
// Returns:     N/A
// Desription:  copies a uint8_t * buffer to a uint32_t * buffer
uint32_t *uint8_to_uint32(uint8_t *b, size_t s)
{
    uint32_t *nb = malloc(s * sizeof(uint32_t));
    
    for(uint32_t i = 0; i < s; ++i)
        nb[i] = (uint32_t) b[i];
        
    return nb;
}