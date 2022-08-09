#ifndef __LIGHTSLEEP_H__
#define __LIGHTSLEEP_H__

#include <stdint.h>

void set_light_sleep();
void set_deep_sleep(void);
void set_custom_lightsleep(uint32_t len);
void set_modem_sleep();
void wake_modem_sleep();

#endif //__LIGHTSLEEP_H__