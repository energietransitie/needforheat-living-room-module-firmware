#ifndef __LIGHTSLEEP_H__
#define __LIGHTSLEEP_H__

#include <stdint.h>

void switch_modes(void);
void set_light_sleep();
void set_custom_lightsleep(uint32_t len);
void set_modem_sleep();
void wake_modem_sleep();

#endif //__LIGHTSLEEP_H__