// ------------------------------------------------------------------------------------------------------------ //
// --------------- WE MIGHT NOT NEED THIS CODE IN OUR PROJECT, SO MAYBE IT CAN BE REMOVED LATER --------------- //
// ------------------------------------------------------------------------------------------------------------ //

// void init_timer()
// {
//     timer_config_t timer_cfg = {
//         .alarm_en       = TIMER_ALARM_EN,           // enable alarm to use interrupts
//         .counter_en     = true,
//         .intr_type      = TIMER_INTR_T0,
//         .counter_dir    = TIMER_COUNT_UP,
//         .auto_reload    = TIMER_AUTORELOAD_EN,
//         .divider        = TIM_PRESC                 // see bottom of this file for value explanation
//     };

//     timer_init(TIMER_GROUP_0, TIMER_0, &timer_cfg);
//     timer_set_counter_value(TIMER_GROUP_0, TIMER_0, TIM_STARTVAL);
//     //timer_enable_intr(TIMER_GROUP_0, TIMER_0);
//     //timer_isr_callback_add(TIMER_GROUP_0, TIMER_0, timer_isr, NULL, ESP_INTR_FLAG_LEVEL4);
//     timer_start(TIMER_GROUP_0, TIMER_0);

//     //timer_isr_handle_t isr_handler;
//     //timer_isr_register(TIMER_GROUP_0, TIMER_0, )
// }

// void read_timer()
// {
//     uint64_t task_counter_value;
    
//     int cnt_val = timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &task_counter_value);

//     sprintf(&str[0], "Timer value: %d\n", cnt_val);
//     usart_write(&str[0], strlen(&str[0]));
// }

// void IRAM_ATTR timer_isr()
// {
//     timer_spinlock_take(TIMER_GROUP_0);
//     // critical section inbetween
//     timer_spinlock_give(TIMER_GROUP_0);

//     if (!(intr_count < MAX_INTR_COUNT)){}    do stuff;
//     intr_count++;   
// }

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
