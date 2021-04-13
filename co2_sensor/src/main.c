#include "../include/usart.h"

#include <string.h>

void app_main() 
{
    const char *str = "Hello, World!\n";
    usart_init(115200);
    

    while(1)
    {       
        usart_write(str, strlen(str)); 
    }
}