#ifdef STANDALONE_SPRIGLABS

#include "controller/standalone/envsensorstandalone_spriglabs.hpp"
#include "secrets.hpp"
#include "global.hpp"

#define PINI_TAG_SA_SL    "standalone_sl"

#define ESA_MQTT_TOPIC_FMT_BASE_PATH    "environment/sensors_async_sad/v0/%s"
//
#define ESA_MQTT_TOPIC_P_FMT             ESA_MQTT_TOPIC_FMT_BASE_PATH "/up"
#define ESA_MQTT_TOPIC_P_FMT_ONLINE      ESA_MQTT_TOPIC_P_FMT         "/lwt"
#define ESA_MQTT_TOPIC_P_FMT_VERSION     ESA_MQTT_TOPIC_P_FMT         "/version"
#define ESA_MQTT_TOPIC_P_FMT_TYPE        ESA_MQTT_TOPIC_P_FMT         "/type"
#define ESA_MQTT_TOPIC_P_FMT_BATTERY     ESA_MQTT_TOPIC_P_FMT         "/battery"
#define ESA_MQTT_TOPIC_P_FMT_LUX         ESA_MQTT_TOPIC_P_FMT         "/lux"
#define ESA_MQTT_TOPIC_P_FMT_TEMPERATURE ESA_MQTT_TOPIC_P_FMT         "/temperature"
#define ESA_MQTT_TOPIC_P_FMT_HUMIDITY    ESA_MQTT_TOPIC_P_FMT         "/humidity"


#define ESA_MQTT_P_BUFFER_STATUS_SIZE   64

#define ESA_VERSION_BUFFER_MAX  16
#define ESA_VALUE32_BUFFER_MAX  16

#define STANDALONE_TYPE "SprigLabs"


void EnvSensorStandaloneSprigLabs::init(INetwork* network) {
    m_network = network;

    m_sprigC.init();
    m_sprigRoot.init();

    mqttBuildTopics();
    mqttInit();
    mqttSetCallbacks();
    m_mqtt.connect();

    LOG_I(PINI_TAG_SA_SL, "SprigLabs - Standalone ID: [%s]", getUniqueId());
}

void EnvSensorStandaloneSprigLabs::loop() {
    loopController();
}


void EnvSensorStandaloneSprigLabs::loopController() {
    if (m_network->isConnected()) {
        m_mqtt.maintain();

        if (m_mqtt.isConnected()) {
            if (m_lastSensorUploadAt + REPORT_WAIT_TIME_MS < getMillis() || m_lastSensorUploadAt == 0) {
                char subTopic[ESA_MQTT_P_BUFFER_STATUS_SIZE];
                char payload[ESA_VALUE32_BUFFER_MAX];

                // BATTERY
                snprintf(payload, sizeof(payload), "%d", (int)m_sprigC.getBatteryPercentage());
                m_mqtt.publish(m_mqttTopicPublish_battery, payload, false);

                // LUX
                snprintf(subTopic, sizeof(subTopic), "%s/%d", m_mqttTopicPublish_lux, 0);
                snprintf(payload, sizeof(payload), "%d", (int)m_sprigRoot.getLux());
                m_mqtt.publish(subTopic, payload, false);

                // TEMPERATURE
                snprintf(subTopic, sizeof(subTopic), "%s/%d", m_mqttTopicPublish_temperature, 0);
                snprintf(payload, sizeof(payload), "%d", (int)m_sprigRoot.getAmbientTemperature());
                m_mqtt.publish(subTopic, payload, false);
                
                // HUMIDITY
                snprintf(subTopic, sizeof(subTopic), "%s/%d", m_mqttTopicPublish_humidity, 0);
                snprintf(payload, sizeof(payload), "%d", (int)m_sprigRoot.getAmbientHumidity());
                m_mqtt.publish(subTopic, payload, false);

                // HUMIDITY - SOIL
                snprintf(subTopic, sizeof(subTopic), "%s/%d", m_mqttTopicPublish_humidity, 1);
                snprintf(payload, sizeof(payload), "%d", (int)m_sprigRoot.getSoilHumidity());
                m_mqtt.publish(subTopic, payload, false);

                m_lastSensorUploadAt = getMillis();
            }
        }
    }
}

void EnvSensorStandaloneSprigLabs::mqttBuildTopics() {
    /**** PUBLISH ****/
    snprintf(m_mqttTopicPublish_online,      ESA_MQTT_TOPIC_MAX_SIZE, ESA_MQTT_TOPIC_P_FMT_ONLINE,      getUniqueId());
    snprintf(m_mqttTopicPublish_version,     ESA_MQTT_TOPIC_MAX_SIZE, ESA_MQTT_TOPIC_P_FMT_VERSION,     getUniqueId());
    snprintf(m_mqttTopicPublish_type,        ESA_MQTT_TOPIC_MAX_SIZE, ESA_MQTT_TOPIC_P_FMT_TYPE,        getUniqueId());
    snprintf(m_mqttTopicPublish_battery,     ESA_MQTT_TOPIC_MAX_SIZE, ESA_MQTT_TOPIC_P_FMT_BATTERY,     getUniqueId());
    snprintf(m_mqttTopicPublish_lux,         ESA_MQTT_TOPIC_MAX_SIZE, ESA_MQTT_TOPIC_P_FMT_LUX,         getUniqueId());
    snprintf(m_mqttTopicPublish_temperature, ESA_MQTT_TOPIC_MAX_SIZE, ESA_MQTT_TOPIC_P_FMT_TEMPERATURE, getUniqueId());
    snprintf(m_mqttTopicPublish_humidity,    ESA_MQTT_TOPIC_MAX_SIZE, ESA_MQTT_TOPIC_P_FMT_HUMIDITY,    getUniqueId());

    /**** SUBSCRIBE ****/

    LOG_D(PINI_TAG_SA_SL, "MQTT topics callbacks configured");
}

void EnvSensorStandaloneSprigLabs::mqttInit() {
    m_mqtt.setClient(m_network->getClient(), getUniqueId());
    m_mqtt.setServer(MQTT_SERVER, MQTT_PORT);
    m_mqtt.setCredentials(MQTT_USER, MQTT_PASS);

    m_mqtt.setWill(m_mqttTopicPublish_online, "0", 2, true);
}

void EnvSensorStandaloneSprigLabs::mqttSetCallbacks() {
    m_mqtt.onConnect(
        [this]() {
            m_mqtt.publish(m_mqttTopicPublish_online, "1", true);

            char version[ESA_VERSION_BUFFER_MAX];
            snprintf(version, sizeof(version), "%d", FIRMWARE_VERSION);
            m_mqtt.publish(m_mqttTopicPublish_version, version, false);

            m_mqtt.publish(m_mqttTopicPublish_type, STANDALONE_TYPE, false);
        }
    );

    m_mqtt.onSubscribe(
        [this](const char* topic) {
            LOG_D(PINI_TAG_SA_SL, "MQTT subscribe: [topic: '%s']", topic);
        }
    );

    LOG_D(PINI_TAG_SA_SL, "MQTT topics callbacks configured");
}

#endif // STANDALONE_SPRIGLABS