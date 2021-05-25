// ------------------------------------------------------------------------------------------------------------ //
// --------------- WE MIGHT NOT NEED THIS CODE IN OUR PROJECT, SO MAYBE IT CAN BE REMOVED LATER --------------- //
// ------------------------------------------------------------------------------------------------------------ //

#include "../include/timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/timer.h"
#include "usart.h"
#include "../include/Wifi.h"

char str[256];

void IRAM_ATTR timer_isr(void *args)
{
    timer_set_alarm(TIMER_GROUP_0,TIMER_0,TIMER_ALARM_EN);
    //send_HTTPS();
    isSending = true;
    
}

void init_timer()
{
    timer_config_t timer_cfg = {
        .alarm_en       = TIMER_ALARM_EN,           // enable alarm to use interrupts
        .counter_en     = false,
        .intr_type      = TIMER_INTR_T0,
        .counter_dir    = TIMER_COUNT_UP,
        .auto_reload    = TIMER_AUTORELOAD_EN,
        .divider        = 40000                // see bottom of this file for value explanation
    };

    timer_init(TIMER_GROUP_0, TIMER_0, &timer_cfg);
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);

    timer_set_alarm_value(TIMER_GROUP_0,TIMER_0, 10000);
    timer_set_alarm(TIMER_GROUP_0,TIMER_0,TIMER_ALARM_EN);
    timer_enable_intr(TIMER_GROUP_0, TIMER_0);
    timer_isr_callback_add(TIMER_GROUP_0, TIMER_0, timer_isr, NULL, NULL);
    timer_start(TIMER_GROUP_0, TIMER_0);

    //timer_isr_handle_t isr_handler;
    //timer_isr_register(TIMER_GROUP_0, TIMER_0, )
}



void read_timer()
{
    uint64_t task_counter_value;
    
    timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &task_counter_value);

    printf("Timer value: %lld\n", task_counter_value);
}



// ---------------------------------------------------------------------------------------------------------------------------------------- //
//                                                  TIMER VALUES EXPLANATION                                                                //
// ---------------------------------------------------------------------------------------------------------------------------------------- //
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
