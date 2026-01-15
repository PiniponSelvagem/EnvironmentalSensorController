#include "controller/envsensorterminal.hpp"
#include "secrets.hpp"
#include "global.hpp"

#define PINI_TAG_EMT    "terminal"

time_t sendTimer = 0;

void EnvSensorTerminal::init() {
    sensorsInit();
    batteryInit();
    loraInit();
}

void EnvSensorTerminal::loopController() {
    if (getMillis() > sendTimer + 60*1000) {
        loraSendStatus();
        sendTimer = getMillis();
    }
}

bool EnvSensorTerminal::isTerminal() const {
    return true;
}
radioid_t EnvSensorTerminal::myRadioId() const {
    return 0;
}

void EnvSensorTerminal::sensorsInit() {
    m_sensorTH.init(22, EDHT::DHT_11);
}

void EnvSensorTerminal::batteryInit() {
    m_battery.init(35, BATTERY_CORRECTION_TTGO_LORA32_DISPLAY);
}

void EnvSensorTerminal::loraSendStatus() {
    float humidity  = m_sensorTH.readHumidity();
    int temperature = m_sensorTH.readTemperature();
    LoRaES_status_t payload {
        .firmware    = FIRMWARE_VERSION,
        .battery     = m_battery.percentage(),
        .humidity    = (uint8_t)humidity,
        .temperature = (int8_t)temperature
    };
    p_lora.send(myRadioId(), ELoRaTag::LT_STATUS, true, (uint8_t*)&payload, sizeof(payload));
}
