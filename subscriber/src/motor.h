#pragma once

#include "gpio.h"
#include "stdio.h"

#define MOTOR_TAG "[MOTOR]: "

struct MotorConfig {
    int in1, in2, in3, in4, ena, enb, encl, encr;
};

enum Direction {
    STOP,
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

MotorConfig MOTOR_CFG_DEFAULT = {
    .in1 = 12,
    .in2 = 13,
    .in3 = 20,
    .in4 = 21,
    .ena = 6,
    .enb = 26,
    .encl = 1,
    .encr = 2
};

class Motor {
private:
    MotorConfig cfg;
    Direction dir;

    int InitMotorPin(int pin) {
        if (!gpio_set_mode(pin, GPIO_DIR_OUT)) {
            fprintf(stderr, MOTOR_TAG "InitMotorPin failed: pin %d", pin);
            return rcFail;
        }
        return rcOk;
    }

    int SetPinsValues(int in1, int in2, int in3, int in4) {
        gpio_set(cfg.in1, in1);
        gpio_set(cfg.in2, in2);
        gpio_set(cfg.in3, in3);
        gpio_set(cfg.in4, in4);

        return 0;
    }
public:
    Motor(MotorConfig cfg) : cfg(cfg) {
        if (!gpio_init()) {
            fprintf(stderr, MOTOR_TAG "gpio_init failed");
        }

        InitMotorPin(cfg.in1);
        InitMotorPin(cfg.in2);
        InitMotorPin(cfg.in3);
        InitMotorPin(cfg.in4);
        InitMotorPin(cfg.ena);
        InitMotorPin(cfg.enb);

        gpio_set_mode(cfg.encl, (GpioMode)(GPIO_DIR_IN | GPIO_EVENT_RISE_EDGE));
        gpio_set_mode(cfg.encr, (GpioMode)(GPIO_DIR_IN | GPIO_EVENT_RISE_EDGE));

    }

    void Disable() {
        gpio_set(cfg.ena, GPIO_LOW_LEVEL);
        gpio_set(cfg.enb, GPIO_LOW_LEVEL);
    }

    void Enable() {
        gpio_set(cfg.ena, GPIO_HIGH_LEVEL);
        gpio_set(cfg.enb, GPIO_HIGH_LEVEL);
    }

    void Run(Direction dir) {
        this->dir = dir;

        switch (dir)
        {
        case STOP:
            SetPinsValues(0, 0, 0, 0);
            //fprintf(stderr, MOTOR_TAG "STOP\n");
            break;
        case FORWARD:
            SetPinsValues(1, 0, 0, 1);
            //fprintf(stderr, MOTOR_TAG "FORWARD\n");
            break;
        case BACKWARD:
            SetPinsValues(0, 1, 1, 0);
            //fprintf(stderr, MOTOR_TAG "BACKWARD\n");
            break;
        case LEFT:
            SetPinsValues(1, 0, 0, 0);
            //fprintf(stderr, MOTOR_TAG "LEFT\n");
            break;
        case RIGHT:
            SetPinsValues(0, 0, 0, 1);
            //fprintf(stderr, MOTOR_TAG "RIGHT\n");
            break;
        default:
            SetPinsValues(0, 0, 0, 0);
            break;
        }
    }
};