#include "gpio.h"

#include <stdio.h>
#include <gpio/gpio.h>
#include <stdbool.h>
#include <bsp/bsp.h>
#include <sys/cdefs.h>
#include <rtl/countof.h>

#define GPIO_PIN_NUM    RTL_UINT32_C(28)
#define HW_MODULE_NAME  "gpio0"
#define HW_MODULE_CFG   "raspberry_pi4b.default"

#define GPIO_PIN_UART_TX 14
#define GPIO_PIN_UART_RX 15

static const rtl_uint32_t ExceptionPinArr[] = { GPIO_PIN_UART_TX,
                                                GPIO_PIN_UART_RX };

static bool IsExceptionPin(const rtl_uint32_t pin)
{
    bool isExceptionPin = false;

    for (size_t i = 0; i < rtl_countof(ExceptionPinArr); i++)
    {
        if (ExceptionPinArr[i] == pin)
        {
            isExceptionPin = true;
            break;
        }
    }

    return isExceptionPin;
}

GpioHandle handle = GPIO_INVALID_HANDLE;

int gpio_init() {
    Retcode rc = rcFail;

    fprintf(stderr, "Start GPIO_output test.\n");

    /**
     * Initialize the board support package (BSP) driver and set configuration
     * for GPIO pins. It is required for stdout by UART.
     */
    rc = BspInit(NULL);
    if (rc != BSP_EOK)
    {
        fprintf(stderr,
                "Failed to initialize BSP, error code: %d.\n",
                RC_GET_CODE(rc));
    }

    if (rcOk == rc)
    {
        rc = BspSetConfig(HW_MODULE_NAME, HW_MODULE_CFG);
        if (rcOk != rc)
        {
            fprintf(stderr, "Failed to set mux configuration for %s module, "
                    "error code: %d.\n", HW_MODULE_NAME,
                    RC_GET_CODE(rc));
        }
    }

    /* Initialize the GPIO. */
    if (rcOk == rc)
    {
        rc = GpioInit();
        if (rcOk != rc)
        {
            fprintf(stderr, "GpioInit failed, error code: %d.\n",
                    RC_GET_CODE(rc));
        }
    }

    /* Initialize and setup HW_MODULE_NAME port. */
    if (rcOk == rc)
    {
        rc = GpioOpenPort(HW_MODULE_NAME, &handle);
        if (rcOk != rc)
        {
            fprintf(stderr,
                    "GpioOpenPort port %s failed, error code: %d.\n",
                    HW_MODULE_NAME, RC_GET_CODE(rc));
        }
        else if (GPIO_INVALID_HANDLE == handle)
        {
            fprintf(stderr, "GPIO module %s handle is invalid.\n",
                    HW_MODULE_NAME);
            rc = rcFail;
        }
    }

    return rcOk == rc;
}

int gpio_set_mode(int pin, GpioMode mode) {
    if (IsExceptionPin(pin)) return -1;

    return rcOk == GpioSetMode(handle, pin, mode);
}

int gpio_set(int pin, int val) {
    if (IsExceptionPin(pin)) return -1;

    return rcOk == GpioOut(handle, pin, val);
}

int gpio_close() {
    return rcOk == GpioClosePort(handle);
}

int gpio_reg_callback(int pin, GpioEventCb cb, void *ctx) {
    return rcOk == GpioRegisterEventCb(handle, pin, cb, ctx);
}

void gpio_unreg_callback(int pin) {
    GpioUnregisterEventCb(handle, pin);
}