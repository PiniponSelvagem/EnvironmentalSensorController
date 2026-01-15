#include "secrets.hpp"
#include "global.hpp"

#include <pinicore.hpp>
#include <Arduino.h>
#include <ArduinoHttpClient.h>

#include "controller/envsensorgateway.hpp"
#include "controller/envsensorterminal.hpp"


#define TAG_MAIN    "main"


#define USE_WIFI
#ifdef USE_WIFI
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

//Storage storage;

//Led led;


void setup() {
    Serial.begin(115200);
    Serial.println();   // Just to start on a new clean line

    LOG_I(TAG_MAIN, "Firmware: [%d] | Build: [%s, %s]", FIRMWARE_VERSION, __DATE__, __TIME__);
    LOG_I(TAG_MAIN, "Enviroment Sensor, by: PiniponSelvagem");

    /*
    uint8_t ledPin[1] = { 25 };
    led.init(ledPin, 1);
    led.set(0, true);
    */

    //storage.init(STORAGE_ID, sizeof(STORAGE_ID));

#ifdef GATEWAY
    #ifdef USE_WIFI
        wifi.init();
        wifi.config(WIFI_SSID, WIFI_PASS);
    #else
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
#ifdef GATEWAY
    network->maintain();
#endif
    controller->loop();
}
