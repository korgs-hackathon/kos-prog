/* © 2021 AO Kaspersky Lab */
#ifndef _MOSQUITTO_SUBSCRIBER_H
#define _MOSQUITTO_SUBSCRIBER_H

#include <mosquittopp.h>
#include <rtl/compiler.h>
#include "kos/thread.h"

class Motor;

struct MotorThreadCtx {
    Tid id;
    Motor *motor;
    int dir;
    int ms_delay;
};

class Subscriber : public mosqpp::mosquittopp
{
public:
    Motor* motor;
    MotorThreadCtx motorThreadCtx;
    Subscriber(const char *id, const char *host, int port);
    ~Subscriber() {};

    void on_connect(int rc) override;
    void on_message(const struct mosquitto_message *message) override;
    void on_subscribe(__rtl_unused int        mid,
                      __rtl_unused int        qos_count,
                      __rtl_unused const int *granted_qos) override;
};

#endif // _MOSQUITTO_SUBSCRIBER_H
