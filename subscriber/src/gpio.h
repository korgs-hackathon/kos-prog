#include <gpio/gpio.h>

#define GPIO_HIGH_LEVEL 1
#define GPIO_LOW_LEVEL  0

int gpio_init();
int gpio_set_mode(int pin, GpioMode mode);
int gpio_set(int pin, int val);
int gpio_close();