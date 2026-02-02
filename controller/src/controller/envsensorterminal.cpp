#include "controller/envsensorterminal.hpp"
#include "secrets.hpp"
#include "global.hpp"

#define PINI_TAG_EMT    "terminal"

void EnvSensorTerminal::init() {
    sensorsInit();
    batteryInit();
    loraInit();
    sleepInit();
    if (m_manualWakeUp) {   // TODO: improve this logic
        oledInit();
    }
    m_initEndAt = getMillis();
}

void EnvSensorTerminal::loopController() {
    if (!m_uploaded) {  // TODO: improve this logic
        readSensors();
        oledRender();
        loraSendStatus();
        m_uploaded = true;
    }

    if (
        (getMillis() > (m_initEndAt+WAKEUP_TIME_MS)) && 
        (p_lora.getSendQueueSize() == 0)
    ) {
        sleep();
    }
}

bool EnvSensorTerminal::isTerminal() const {
    return true;
}
radioid_t EnvSensorTerminal::myRadioId() const {
    return 0;
}

void EnvSensorTerminal::sensorsInit() {
    m_sensorTH.init(PIN_DHT, EDHT::DHT_11);
}

void EnvSensorTerminal::batteryInit() {
    m_battery.init(PIN_BATTERY, BATTERY_CORRECTION_TTGO_LORA32_DISPLAY);
}

void EnvSensorTerminal::sleepInit() {
    /* Configure wake-up button */    
    pinMode(PIN_WAKEUP, INPUT_PULLUP);
    m_manualWakeUp = !digitalRead(PIN_WAKEUP);
    esp_sleep_enable_ext0_wakeup((gpio_num_t)PIN_WAKEUP, 0);
    LOG_D(PINI_TAG_EMT, "Wake button press %s", m_manualWakeUp?"true":"false");
    // TODO: improve logic
    esp_reset_reason_t resetReason = esp_reset_reason();
    switch (resetReason) {
        case ESP_RST_POWERON:
        case ESP_RST_EXT:
        case ESP_RST_WDT:   // The reset 'button' can trigger this reason
            m_manualWakeUp = true;
            break;
        default:    // There are more cases but not relevant
            break;
    }

    /* Configure timed sleep */
    uint64_t timesleep_ns = SLEEP_TIME_SECS * 1000000LL;   // sec * nano
    esp_sleep_enable_timer_wakeup(timesleep_ns);
}

void EnvSensorTerminal::oledInit() {
    m_oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
}

void EnvSensorTerminal::readSensors() {
    m_sensorsReadAt = getMillis();

    m_humidity = m_sensorTH.readHumidity();
    m_temperature = m_sensorTH.readTemperature();
    m_batteryPercentage = m_battery.percentage();
}

void EnvSensorTerminal::oledRender() {
    if (!m_manualWakeUp) return;
    // TODO: improve after adding SSD1306 support to PiniCore
    m_oled.setTextSize(1);
    m_oled.setTextColor(SSD1306_WHITE);
    m_oled.setCursor(0, 0);
    m_oled.clearDisplay();
    m_oled.printf("%d %%\n\n", m_batteryPercentage);
    m_oled.setTextSize(3);
    m_oled.printf("  %d C\n", m_temperature);
    m_oled.printf("  %d %%\n", m_humidity);
    m_oled.display();
}

void EnvSensorTerminal::loraSendStatus() {
    LoRaES_status_t payload {
        .firmware    = FIRMWARE_VERSION,
        .battery     = m_batteryPercentage,
        .humidity    = (uint8_t)m_humidity,
        .temperature = (int8_t)m_temperature
    };
    p_lora.send(myRadioId(), ELoRaTag::LT_STATUS, true, (uint8_t*)&payload, sizeof(payload));
}

void EnvSensorTerminal::sleep() {
    LOG_I(PINI_TAG_EMT, "Sleeping...");
    if (m_manualWakeUp) {
        // TODO: turn off OLED
        Wire.beginTransmission(0x3C);
        Wire.write(0x00);
        Wire.write(0xAE);
        Wire.endTransmission();
    }
    esp_deep_sleep_start();
}