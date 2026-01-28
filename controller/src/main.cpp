#include "secrets.hpp"
#include "global.hpp"

#include <pinicore.hpp>

#include "controller/envsensorgateway.hpp"
#include "controller/envsensorterminal.hpp"

#define TAG_MAIN    "main"

#ifndef USE_GSM_NETWORK
    WiFiComm wifi;
    INetwork* network = (INetwork*)&wifi;
#else
    MobileComm mobile;
    INetwork* network = (INetwork*)&mobile;
#endif

#ifdef GATEWAY
    EnvSensorGateway _controller;
#else
    EnvSensorTerminal _controller;
#endif
IEnvSensor* controller = &_controller;


void setup() {
    Serial.begin(115200);
    Serial.println();   // Just to start on a new clean line

    LOG_I(TAG_MAIN, "Firmware: [%d] | Build: [%s, %s]", FIRMWARE_VERSION, __DATE__, __TIME__);
    LOG_I(TAG_MAIN, "Enviroment Sensor, by: PiniponSelvagem");

    /* Watchdog setup */
    watchdogSetup(WDTG_INTERNAL_TIMER_IN_SECONDS);
    watchdogEnable();

#ifdef GATEWAY
    #ifndef USE_GSM_NETWORK
        wifi.init();
        wifi.config(WIFI_SSID, WIFI_PASS);
    #else
        #error USE_GSM_NETWORK currently not supported. To support it, get an ESP that has GSM module and setup the correct pins.
        mobile.init(23, 4, 5, 27, 26);
        mobile.config("", "");
    #endif
        network->enable();
        network->connect();
#endif

#ifdef GATEWAY
    _controller.init(network);
#else
    _controller.init();
#endif

    LOG_I(TAG_MAIN, "Inital setup complete");
}

void loop() {
    watchdogIamAlive();
#ifdef GATEWAY
    network->maintain();
#endif
    controller->loop();
}
