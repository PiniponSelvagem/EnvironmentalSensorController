#ifdef STANDALONE_SPRIGLABS

#include "controller/standalone/envsensorstandalone_spriglabs.hpp"
#include "secrets.hpp"
#include "global.hpp"

#define PINI_TAG_SA_SL    "standalone_sl"

#define ESA_MQTT_TOPIC_FMT_BASE_PATH    "environment/sensors_async_sad/v0/%s"
//
//
// PUBLISH Topics format
#define ESA_MQTT_TOPIC_P_FMT               ESA_MQTT_TOPIC_FMT_BASE_PATH "/up"           // Publish base path
#define ESA_MQTT_TOPIC_P_FMT_ONLINE        ESA_MQTT_TOPIC_P_FMT         "/lwt"          // Publish topic: online (only in Always On mode)
#define ESA_MQTT_TOPIC_P_FMT_VERSION       ESA_MQTT_TOPIC_P_FMT         "/version"      // Publish topic: firmware version
#define ESA_MQTT_TOPIC_P_FMT_TYPE          ESA_MQTT_TOPIC_P_FMT         "/type"         // Publish topic: standalone type
#define ESA_MQTT_TOPIC_P_FMT_BATTERY       ESA_MQTT_TOPIC_P_FMT         "/battery"      // Publish topic: battery percentage
#define ESA_MQTT_TOPIC_P_FMT_LUX           ESA_MQTT_TOPIC_P_FMT         "/lux"          // Publish topic: lux sensor
#define ESA_MQTT_TOPIC_P_FMT_TEMPERATURE   ESA_MQTT_TOPIC_P_FMT         "/temperature"  // Publish topic: temperature sensor base path
#define ESA_MQTT_TOPIC_P_FMT_HUMIDITY      ESA_MQTT_TOPIC_P_FMT         "/humidity"     // Publish topic: humidity sensor base path
//
//
// SUBSCRIBE Topics format
#define ESA_MQTT_TOPIC_S_FMT               ESA_MQTT_TOPIC_FMT_BASE_PATH "/down"    // Subscribe base path
//
#define ESA_MQTT_TOPIC_S_FMT_ADMIN         ESA_MQTT_TOPIC_S_FMT         "/admin"   // Subscribe admin commands base path
#define ESA_MQTT_TOPIC_S_FMT_ADMIN_RESTART ESA_MQTT_TOPIC_S_FMT_ADMIN   "/restart" // Subscribe topic: restart


#define ESA_MQTT_P_BUFFER_STATUS_SIZE   64

#define ESA_VERSION_BUFFER_MAX  16
#define ESA_VALUE32_BUFFER_MAX  16

#define STANDALONE_TYPE         "SprigLabs"
#define STANDALONE_TYPE_BATT    STANDALONE_TYPE " Battery"


void EnvSensorStandaloneSprigLabs::init(INetwork* network) {
    m_network = network;

#ifdef BATTERY
    // TODO: This should be replaced later on with configuration loading
    m_isBattery = true;
#endif

    Wire.begin(2, 3); // TODO: PiniCore should manage I2C usage, using a driver that requires it, should init I2C
    m_sprigC.init();
    m_sprigRoot.init();

    mqttBuildTopics();
    mqttInit();
    mqttSetCallbacks();
    m_mqtt.connect();

    LOG_I(PINI_TAG_SA_SL, "%s - Standalone ID: [%s]", m_isBattery?STANDALONE_TYPE_BATT:STANDALONE_TYPE, getUniqueId());
}

void EnvSensorStandaloneSprigLabs::loop() {
    loopController();
    
    if (m_isBattery && (
            m_lastSensorUploadAt > 0 ||        // Sensor data sent
            (getMillis() > CONNECT_TIMEOUT_MS) // Connection timeout
    )) {
        sleep();
    }
}


void EnvSensorStandaloneSprigLabs::loopController() {
    if (m_network->isConnected()) {
        m_mqtt.maintain();

        if (m_mqtt.isConnected()) {
            if (m_lastSensorUploadAt + REPORT_WAIT_TIME_MS < getMillis() || m_lastSensorUploadAt == 0) {
                char subTopic[ESA_MQTT_P_BUFFER_STATUS_SIZE];
                char payload[ESA_VALUE32_BUFFER_MAX];
                
                readSensors();  // Populate sensors variables with latest data

                // BATTERY
                snprintf(payload, sizeof(payload), "%d", (int)m_batteryPercentage);
                m_mqtt.publish(m_mqttTopicP_battery, payload, false);

                // LUX
                snprintf(subTopic, sizeof(subTopic), "%s/%d", m_mqttTopicP_lux, 0);
                snprintf(payload, sizeof(payload), "%d", (int)m_lux);
                m_mqtt.publish(subTopic, payload, false);

                // TEMPERATURE
                snprintf(subTopic, sizeof(subTopic), "%s/%d", m_mqttTopicP_temperature, 0);
                snprintf(payload, sizeof(payload), "%d", (int)m_ambientTemperature);
                m_mqtt.publish(subTopic, payload, false);
                
                // HUMIDITY
                snprintf(subTopic, sizeof(subTopic), "%s/%d", m_mqttTopicP_humidity, 0);
                snprintf(payload, sizeof(payload), "%d", (int)m_ambientHumidity);
                m_mqtt.publish(subTopic, payload, false);

                // HUMIDITY - SOIL
                snprintf(subTopic, sizeof(subTopic), "%s/%d", m_mqttTopicP_humidity, 1);
                snprintf(payload, sizeof(payload), "%d", (int)m_soilHumidity);
                m_mqtt.publish(subTopic, payload, false);

                m_lastSensorUploadAt = getMillis();
            }
        }
    }
}

void EnvSensorStandaloneSprigLabs::readSensors() {
    m_batteryPercentage = m_sprigC.getBatteryPercentage();
    m_lux = m_sprigRoot.getLux();
    m_ambientTemperature= m_sprigRoot.getAmbientTemperature();
    m_ambientHumidity = m_sprigRoot.getAmbientHumidity();
    m_soilHumidity = m_sprigRoot.getSoilHumidity();
}

void EnvSensorStandaloneSprigLabs::mqttBuildTopics() {
    /**** PUBLISH ****/
    if (!m_isBattery) {
        snprintf(m_mqttTopicP_online,  sizeof(m_mqttTopicP_online), ESA_MQTT_TOPIC_P_FMT_ONLINE,           getUniqueId());
    }
    snprintf(m_mqttTopicP_version,     sizeof(m_mqttTopicP_version), ESA_MQTT_TOPIC_P_FMT_VERSION,         getUniqueId());
    snprintf(m_mqttTopicP_type,        sizeof(m_mqttTopicP_type), ESA_MQTT_TOPIC_P_FMT_TYPE,               getUniqueId());
    snprintf(m_mqttTopicP_battery,     sizeof(m_mqttTopicP_battery), ESA_MQTT_TOPIC_P_FMT_BATTERY,         getUniqueId());
    snprintf(m_mqttTopicP_lux,         sizeof(m_mqttTopicP_lux), ESA_MQTT_TOPIC_P_FMT_LUX,                 getUniqueId());
    snprintf(m_mqttTopicP_temperature, sizeof(m_mqttTopicP_temperature), ESA_MQTT_TOPIC_P_FMT_TEMPERATURE, getUniqueId());
    snprintf(m_mqttTopicP_humidity,    sizeof(m_mqttTopicP_humidity), ESA_MQTT_TOPIC_P_FMT_HUMIDITY,       getUniqueId());

    /**** SUBSCRIBE ****/
    if (!m_isBattery) {
        snprintf(m_mqttTopicS_adminRestart, sizeof(m_mqttTopicS_adminRestart), ESA_MQTT_TOPIC_S_FMT_ADMIN_RESTART, getUniqueId());
    }

    LOG_D(PINI_TAG_SA_SL, "MQTT topics built");
}

void EnvSensorStandaloneSprigLabs::mqttInit() {
    m_mqtt.setClient(m_network->getClient(), getUniqueId());
    m_mqtt.setServer(MQTT_SERVER, MQTT_PORT);
    m_mqtt.setCredentials(MQTT_USER, MQTT_PASS);

    if (!m_isBattery) {
        m_mqtt.setWill(m_mqttTopicP_online, "0", 2, true);
    }
}

void EnvSensorStandaloneSprigLabs::mqttSetCallbacks() {
    m_mqtt.onConnect(
        [this]() {
            if (!m_isBattery) {
                m_mqtt.publish(m_mqttTopicP_online, "1", true);
            }
            
            char version[ESA_VERSION_BUFFER_MAX];
            snprintf(version, sizeof(version), "%d", FIRMWARE_VERSION);
            m_mqtt.publish(m_mqttTopicP_version, version, false);
            
            m_mqtt.publish(m_mqttTopicP_type, m_isBattery?STANDALONE_TYPE_BATT:STANDALONE_TYPE, false);
        }
    );

    // Only Always On mode subscribes to topics
    if (!m_isBattery) {
        m_mqtt.onSubscribe(
            [this](const char* topic) {
                LOG_D(PINI_TAG_SA_SL, "MQTT subscribe: [topic: '%s']", topic);
            }
        );

        // Subscribe: Admin restart command
        m_mqtt.onTopic(
            m_mqttTopicS_adminRestart,
            [this](const char* payload, uint32_t length) {
                if (length > 0) {
                    char *endptr;
                    int seppuku = (int)strtol((char *)payload, &endptr, 10);
                    if (payload == endptr) {
                        // Unable to parse secret action, ignoring
                        return;
                    }
                    if (seppuku == 666) {
                        restart();
                    }
                }
            }
        );
    }

    LOG_D(PINI_TAG_SA_SL, "MQTT topics callbacks configured");
}

void EnvSensorStandaloneSprigLabs::sleep() {
    m_batteryPercentage = m_sprigC.getBatteryPercentage();

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

    LOG_I(PINI_TAG_SA_SL, "Sleeping...");

/**
 * TODO: Test and find out which PINs shouldbe turned off
 */
//    /* "Turn OFF" pins to reduce battery consumption during sleep */
//    /* This pins where tested for TTGO Lora32 */
//    pinMode(0,INPUT);
//    //pinMode(1,INPUT);     // TX pin, used for debug
//    pinMode(2,INPUT);
//    //pinMode(3,INPUT);     // RX pin, used for debug
//    pinMode(4,INPUT);
//    pinMode(5,INPUT);
//    pinMode(12,INPUT);
//    pinMode(13,INPUT);
//    //pinMode(14,INPUT);    // used by PIN_WAKEUP
//    pinMode(15,INPUT);
//    //pinMode(16,INPUT);    // rst:0x8 (TG1WDT_SYS_RESET)
//    //pinMode(17,INPUT);    // rst:0x7 (TG0WDT_SYS_RESET)
//    pinMode(18,INPUT);
//    pinMode(19,INPUT);
//    pinMode(21,INPUT);
//    pinMode(22,INPUT);
//    pinMode(23,INPUT);
//    pinMode(25,INPUT);
//    pinMode(26,INPUT);
//    pinMode(27,INPUT);
//    pinMode(32,INPUT);
//    pinMode(33,INPUT);
//    pinMode(34,INPUT);
//    pinMode(35,INPUT);
//    pinMode(36,INPUT);
//    pinMode(39,INPUT);
//    /* END of "Turn OFF" pins */

    esp_deep_sleep_start();
}

#endif // STANDALONE_SPRIGLABS