#ifdef TERMINAL

#include "controller/lora/envsensorterminal.hpp"
#include "secrets.hpp"
#include "global.hpp"

#define PINI_TAG_EMT    "terminal"

#define RADIO_ID    1  // TODO: This must be changed, via a Web interface will be good and saved to flash

void EnvSensorTerminal::init() {
    sensorsInit();
    batteryInit();
    loraInit();
    sleepInit();
    if (m_manualWakeUp) {   // TODO: improve this logic
        oledInit();
    }
    LOG_I(PINI_TAG_EMT, "Terminal radio ID: [%d]", myRadioId());
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
        (getMillis() > (m_initEndAt+WAKEUP_TIME_MS) || !m_manualWakeUp) && 
        (p_lora.getSendQueueSize() == 0)
    ) {
        sleep();
    }
}

bool EnvSensorTerminal::isTerminal() const {
    return true;
}
radioid_t EnvSensorTerminal::myRadioId() const {
    return RADIO_ID;
}

void EnvSensorTerminal::sensorsInit() {
    m_sensorTH.init(PIN_DHT, EDHT::DHT_11);
}

void EnvSensorTerminal::batteryInit() {
    m_battery.init(PIN_BATTERY);
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
}

void EnvSensorTerminal::oledInit() {
    m_oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
}

void EnvSensorTerminal::readSensors() {
    m_sensorsReadAt = getMillis();

    m_humidity = m_sensorTH.readHumidity();
    m_temperature = m_sensorTH.readTemperature();
    m_batteryPercentage = m_battery.getPercentage();
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
    m_oled.printf("  %d C\n", (int)m_temperature);
    m_oled.printf("  %d %%\n", (int)m_humidity);
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
    /* Configure timed sleep */
    uint64_t timesleep_ns = 1;
    if (m_batteryPercentage > 0) {
        timesleep_ns = SLEEP_TIME_SECS;
    }
    else {
        timesleep_ns = SLEEP_TIME_SECS_CRITICAL_BAT;
    }
    timesleep_ns = timesleep_ns * 1000000LL; // sec * nano
    esp_sleep_enable_timer_wakeup(timesleep_ns);

    /* OLED is ON, turn it OFF */
    if (m_manualWakeUp) {
        // TODO: place this in a library, turn off OLED
        Wire.beginTransmission(0x3C);
        Wire.write(0x00);
        Wire.write(0xAE);
        Wire.endTransmission();
    }

    LOG_I(PINI_TAG_EMT, "Sleeping...");

    /* "Turn OFF" pins to reduce battery consumption during sleep */
    /* This pins where tested for TTGO Lora32 */
    pinMode(0,INPUT);
    //pinMode(1,INPUT);     // TX pin, used for debug
    pinMode(2,INPUT);
    //pinMode(3,INPUT);     // RX pin, used for debug
    pinMode(4,INPUT);
    pinMode(5,INPUT);
    pinMode(12,INPUT);
    pinMode(13,INPUT);
    //pinMode(14,INPUT);    // used by PIN_WAKEUP
    pinMode(15,INPUT);
    //pinMode(16,INPUT);    // rst:0x8 (TG1WDT_SYS_RESET)
    //pinMode(17,INPUT);    // rst:0x7 (TG0WDT_SYS_RESET)
    pinMode(18,INPUT);
    pinMode(19,INPUT);
    pinMode(21,INPUT);
    pinMode(22,INPUT);
    pinMode(23,INPUT);
    pinMode(25,INPUT);
    pinMode(26,INPUT);
    pinMode(27,INPUT);
    pinMode(32,INPUT);
    pinMode(33,INPUT);
    pinMode(34,INPUT);
    pinMode(35,INPUT);
    pinMode(36,INPUT);
    pinMode(39,INPUT);
    /* END of "Turn OFF" pins */

    esp_deep_sleep_start();
}

#endif // TERMINAL