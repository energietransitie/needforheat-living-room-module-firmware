#include "../include/timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/timer.h"
#include "usart.h"
#include "../include/Wifi.h"

#define interval 20000  // 1 is 0.0005s. with this you can calculate the interval

char str[256];

// Function:   timer_isr()
// Params:     N/A
// Returns:    N/A
// Desription: interrupt from the 24 hour timer
bool IRAM_ATTR timer_isr(void *args)
{
    timer_set_alarm(TIMER_GROUP_0,TIMER_0,TIMER_ALARM_EN);      // set the alarm to go off again
    return isSending = true;                                    // set a bool to true so the device knows it should send
}

// Function:   init_timer()
// Params:     N/A
// Returns:    N/A
// Desription: Used to initialise the 24 hour timer
void init_timer()
{
    timer_config_t timer_cfg = {
        .alarm_en       = TIMER_ALARM_EN,           // enable alarm to use interrupts
        .counter_en     = false,
        .intr_type      = TIMER_INTR_T0,
        .counter_dir    = TIMER_COUNT_UP,
        .auto_reload    = TIMER_AUTORELOAD_EN,      // enable that the counter resets when it hits the value
        .divider        = 40000                     // see bottom of this file for value explanation
    };

    timer_init(TIMER_GROUP_0, TIMER_0, &timer_cfg);
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);         // set the counter to 0 when it starts

    timer_set_alarm_value(TIMER_GROUP_0,TIMER_0, interval);     // set the alarm value to the interval defined above
    timer_set_alarm(TIMER_GROUP_0,TIMER_0,TIMER_ALARM_EN);
    timer_enable_intr(TIMER_GROUP_0, TIMER_0);
    timer_isr_callback_add(TIMER_GROUP_0, TIMER_0, timer_isr, NULL, 0);     // add the interupt to the timer
    timer_start(TIMER_GROUP_0, TIMER_0);                                    // start the timer
}

// Function:   read_timer()
// Params:     N/A
// Returns:    N/A
// Desription: Used to read the 24 hour timer (TESTING PURPOSES ONLY)
void read_timer()
{
    uint64_t task_counter_value;
    
    timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &task_counter_value);

    printf("Timer value: %lld\n", task_counter_value);
}

// --------------------------------------------------------------------------------------------------------------------- //
//                                                  TIMER VALUES EXPLANATION                                             //
// --------------------------------------------------------------------------------------------------------------------- //
// Fclk = 80MHz
// max presc = 65.536
// Fclk / max presc = ~1220Hz
// this means the max time delay we can get is 1 / ~1220 = ~0.00082s
// an easier value to calulate with in the interrupt would be 0.0005s
// a prescaler of 40.000 gives us a overflow every 0.0005s
// to time 5 minutes in the interrupt --> 300s / 0.0005s = 600.000
// this would mean we need to count up to 600.000 in the interrupt to time 5 minutes
// to time 24 hours in the interrupt --> 86400s / 0.0005s = 172.800.000
// this woukd mean we need to count up to 172.800.000 in the interrupt to time 24 hours
