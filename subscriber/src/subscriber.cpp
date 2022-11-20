#include <iostream>
#include <string_view>

#include "general.h"
#include "subscriber.h"
#include "json.hpp"
#include "motor.h"
#include "kos/thread.h"
#include <iostream>

using namespace std::literals;
using json = nlohmann::json;

#define MOTOR_THREAD_TAG "[MOTOR_THREAD]: "

std::unordered_map<std::string, Direction> commands = {
    {"forward", FORWARD},
    {"back", BACKWARD},
    {"left", LEFT},
    {"right", RIGHT},
    {"stop", STOP}  
};

int motor_thread(void *ctx) {
    //std::cout << MOTOR_THREAD_TAG << "Started motor thread" << std::endl;
    auto mctx = (MotorThreadCtx*)ctx;
    //std::cout << MOTOR_THREAD_TAG << "Running dir " << mctx->dir << std::endl;
    mctx->motor->Run((Direction)mctx->dir);
    //std::cout << MOTOR_THREAD_TAG << "Sleeping for " << mctx->ms_delay << "ms" << std::endl;
    KosThreadSleep(mctx->ms_delay);
    //std::cout << MOTOR_THREAD_TAG << "Stoping motors" << std::endl;
    mctx->motor->Run(STOP);
    mctx->id = 0;
    KosThreadExit(0);
}

constexpr auto Topic = "korgs/topic"sv;

Subscriber::Subscriber(const char *id, const char *host, int port)
    : mosquittopp(id),  motor(new Motor(MOTOR_CFG_DEFAULT))
{
    std::cout << app::AppTag << "Connecting to MQTT Broker with address "
              << host << " and port " << port << std::endl;

    const int keepAlive = 60;
    motor->Enable();
    motorThreadCtx.id = 0;
    motorThreadCtx.dir = STOP;
    motorThreadCtx.motor = motor;
    connect(host, port, keepAlive);
}

void Subscriber::on_connect(int rc)
{
    std::cout << app::AppTag << "Subscriber connected with code "
              << rc << std::endl;

    if (rc == 0)
    {
        subscribe(NULL, Topic.data());
    }
}

void Subscriber::on_message(const struct mosquitto_message *message)
{
    if (Topic == message->topic)
    {
        const std::string_view payload{reinterpret_cast<char*>(message->payload),
                                       static_cast<size_t>(message->payloadlen)};
        std::cout << app::AppTag << "Got message with topic: " << message->topic
                  << ", payload: " << payload << std::endl;

        json v = json::parse(payload);
        
        try {
            auto cmd = v["cmd"].get<std::string>();
            if (commands.find(cmd) != commands.end()) {
                auto arg = commands[cmd];

                if (cmd == "stop") {
                    if (motorThreadCtx.id != 0)
                        KosThreadSuspend(motorThreadCtx.id);
                    motorThreadCtx.id = 0;
                    motor->Run(STOP);
                } else {
                    auto delay = v["val"].get<float>();

                    if (motorThreadCtx.id == 0) {
                        motorThreadCtx.dir = arg;
                        motorThreadCtx.ms_delay = delay * 1000;
                        std::cout << "Starting thread" << std::endl;
                        KosThreadCreate(&motorThreadCtx.id, ThreadPriorityNormal, ThreadStackSizeDefault,
                                        motor_thread, &motorThreadCtx, 0);
                    }
                }
            }
        }
        catch(std::exception& exc) {

        }
    }
}

void Subscriber::on_subscribe(__rtl_unused int        mid,
                              __rtl_unused int        qos_count,
                              __rtl_unused const int *granted_qos)
{
    std::cout << app::AppTag << "Subscription succeeded." << std::endl;
}
