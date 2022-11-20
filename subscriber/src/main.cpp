#include <cstring>
#include <exception>
#include <iostream>
#include <memory>
#include <thread>

#include <kos_net.h>

#include "general.h"
#include "subscriber.h"
#include "gpio.h"

namespace consts {
constexpr const char *DefaultMqttAddress = "10.0.2.2";
constexpr int DefaultMqttUnencryptedPort = 1883;
constexpr int PublicationIntervalInSec = 5;
} // namespace consts

static std::string GetBrokerAddress()
{
    const char *brokerAddress = getenv("MQTT_BROKER_ADDRESS");
    if (!brokerAddress)
    {
        std::cerr << app::AppTag
                  << "Failed to get MQTT broker address. Using default MQTT "
                     "broker address ("
                  << consts::DefaultMqttAddress << ")" << std::endl;
        return consts::DefaultMqttAddress;
    }
    return brokerAddress;
}

static int GetBrokerPort()
{
    const char *brokerPortEnvVariable = getenv("MQTT_BROKER_PORT");
    if (!brokerPortEnvVariable)
    {
        std::cerr << app::AppTag
                  << "Failed to get MQTT broker port. Using default MQTT "
                     "broker port ("
                  << consts::DefaultMqttUnencryptedPort << ")" << std::endl;
        return consts::DefaultMqttUnencryptedPort;
    }

    try
    {
        return std::stoi(brokerPortEnvVariable);
    }
    catch (const std::invalid_argument &ex)
    {
        std::cerr << app::AppTag
                  << "Failed to get MQTT broker port: " << ex.what()
                  << "Using default port ("
                  << consts::DefaultMqttUnencryptedPort << ")" << std::endl;
        return consts::DefaultMqttUnencryptedPort;
    }
}

/**
 * ExceptionPinArr is an array of GPIO pin numbers that should be excluded
 * from the example. This may be necessary if some of the pins are already
 * used for other functions, for example, if the pins are used for UART
 * connections during debugging.
 */

void gpio_thread() {
    /*if (!gpio_init()) 
        fprintf(stderr, "gpio_init err");
    
    if (!gpio_set_mode(26, GPIO_DIR_OUT))
        fprintf(stderr, "gpio_set_mode err");

    for (int i = 0; i < 25; i++) {
        if (!gpio_set(26, GPIO_HIGH_LEVEL))
            fprintf(stderr, "gpio_set err");
        sleep(2);
        if (!gpio_set(26, GPIO_LOW_LEVEL))
            fprintf(stderr, "gpio_set err");
        sleep(2);
    }*/
}

int main(void)
{
    //std::thread gpio_thr(gpio_thread);
    //gpio_thread();
    if (!wait_for_network())
    {
        std::cerr << app::AppTag << "Error: Wait for network failed!"
                  << std::endl;
        return EXIT_FAILURE;
    }

    mosqpp::lib_init();

    auto sub = std::make_unique<Subscriber>(
        "subscriber", GetBrokerAddress().c_str(), GetBrokerPort());
    if (sub)
    {
        sub->loop_forever();
    }

    mosqpp::lib_cleanup();
    return EXIT_SUCCESS;
}