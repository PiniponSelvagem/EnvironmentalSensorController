#ifdef STANDALONE_SPRIGLABS

#include "controller/standalone/envsensorstandalone_spriglabs.hpp"
#include "secrets.hpp"
#include "global.hpp"

#define PINI_TAG_SA_SL    "standalone_sl"

#define ESA_MQTT_TOPIC_FMT_BASE_PATH    "environment/sensors_async_sad/v0/%s"
//
#define ESA_MQTT_TOPIC_P_FMT             ESA_MQTT_TOPIC_FMT_BASE_PATH "/up"
#define ESA_MQTT_TOPIC_P_FMT_VERSION     ESA_MQTT_TOPIC_P_FMT         "/version"
#define ESA_MQTT_TOPIC_P_FMT_TYPE        ESA_MQTT_TOPIC_P_FMT         "/type"
#define ESA_MQTT_TOPIC_P_FMT_LUX         ESA_MQTT_TOPIC_P_FMT         "/lux"
#define ESA_MQTT_TOPIC_P_FMT_TEMPERATURE ESA_MQTT_TOPIC_P_FMT         "/temperature"
#define ESA_MQTT_TOPIC_P_FMT_HUMIDITY    ESA_MQTT_TOPIC_P_FMT         "/humidity"


#define ESA_MQTT_P_BUFFER_STATUS_SIZE   64

#define ESA_VERSION_BUFFER_MAX  16

#define STANDALONE_TYPE "SprigLabs"


void EnvSensorStandaloneSprigLabs::init(INetwork* network) {
    m_network = network;

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
    }
}

void EnvSensorStandaloneSprigLabs::mqttBuildTopics() {
    /**** PUBLISH ****/
    snprintf(m_mqttTopicPublish_version,     ESA_MQTT_TOPIC_MAX_SIZE, ESA_MQTT_TOPIC_P_FMT_VERSION,     getUniqueId());
    snprintf(m_mqttTopicPublish_type,        ESA_MQTT_TOPIC_MAX_SIZE, ESA_MQTT_TOPIC_P_FMT_TYPE,        getUniqueId());
    snprintf(m_mqttTopicPublish_lux,         ESA_MQTT_TOPIC_MAX_SIZE, ESA_MQTT_TOPIC_P_FMT_LUX,         getUniqueId());
    snprintf(m_mqttTopicPublish_temperature, ESA_MQTT_TOPIC_MAX_SIZE, ESA_MQTT_TOPIC_P_FMT_TEMPERATURE, getUniqueId());
    snprintf(m_mqttTopicPublish_humidity,    ESA_MQTT_TOPIC_MAX_SIZE, ESA_MQTT_TOPIC_P_FMT_HUMIDITY,    getUniqueId());

    /**** SUBSCRIBE ****/
}

void EnvSensorStandaloneSprigLabs::mqttInit() {
    m_mqtt.setClient(m_network->getClient(), getUniqueId());
    m_mqtt.setServer(MQTT_SERVER, MQTT_PORT);
    m_mqtt.setCredentials(MQTT_USER, MQTT_PASS);
}

void EnvSensorStandaloneSprigLabs::mqttSetCallbacks() {
    m_mqtt.onConnect(
        [this]() {
            char version[ESA_VERSION_BUFFER_MAX];
            snprintf(version, ESA_VERSION_BUFFER_MAX, "%d", FIRMWARE_VERSION);
            m_mqtt.publish(m_mqttTopicPublish_version, version, false);

            m_mqtt.publish(m_mqttTopicPublish_type, STANDALONE_TYPE, false);
        }
    );
}

#endif // STANDALONE_SPRIGLABS