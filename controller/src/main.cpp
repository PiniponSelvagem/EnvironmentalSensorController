#include "secrets.hpp"
#include "global.hpp"

#include <pinicore.hpp>

using namespace pinicore;

#define TAG_MAIN    "main"


#ifdef GATEWAY
    #include "controller/lora/envsensorgateway.hpp"
    EnvSensorGateway _controller;
#elif defined(TERMINAL)
    #include "controller/lora/envsensorterminal.hpp"
    EnvSensorTerminal _controller;
#elif defined(STANDALONE_SPRIGLABS)
    #include "controller/standalone/envsensorstandalone_spriglabs.hpp"
    EnvSensorStandaloneSprigLabs _controller;
#endif
IEnvSensor* controller = &_controller;

#ifdef USE_NETWORK
    #ifndef USE_GSM_NETWORK
        WiFiComm wifi;
        INetwork* network = (INetwork*)&wifi;
    #else
        MobileComm mobile;
        INetwork* network = (INetwork*)&mobile;
    #endif
#endif


void setup() {
    Serial.begin(115200);
    Serial.println();   // Just to start on a new clean line

    LOG_I(TAG_MAIN, "Firmware: [%d] | Build: [%s, %s]", FIRMWARE_VERSION, __DATE__, __TIME__);
    LOG_I(TAG_MAIN, "Environment Sensor, by: PiniponSelvagem");

    /* Watchdog setup */
    watchdogSetup(WDTG_INTERNAL_TIMER_IN_SECONDS);
    watchdogEnable();

#ifdef USE_NETWORK
    #ifdef USE_GSM_NETWORK
        #error USE_GSM_NETWORK currently not supported. To support it, get an ESP that has GSM module and setup the correct pins.
        mobile.init(23, 4, 5, 27, 26);
        mobile.config("", "");
    #else
        wifi.init();
        wifi.config(WIFI_SSID, WIFI_PASS);
    #endif
        network->enable();
        network->connect();
#endif

#ifdef USE_NETWORK
    _controller.init(network);
#else
    _controller.init();
#endif

    LOG_I(TAG_MAIN, "Inital setup complete");
}

void loop() {
    watchdogIamAlive();
#ifdef USE_NETWORK
    network->maintain();
#endif
    controller->loop();
}
