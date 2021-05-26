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