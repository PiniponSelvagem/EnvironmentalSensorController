#ifdef GATEWAY

#include "controller/lora/envsensorgateway.hpp"
#include "secrets.hpp"
#include "global.hpp"

#define PINI_TAG_ESG    "gateway"

#define ESG_MQTT_TOPIC_FMT_BASE_PATH    "environment/sensors_async/v0/%llu"
//
#define ESG_MQTT_TOPIC_P_FMT            ESG_MQTT_TOPIC_FMT_BASE_PATH "/up"
#define ESG_MQTT_TOPIC_P_FMT_ONLINE     ESG_MQTT_TOPIC_P_FMT         "/lwt"
#define ESG_MQTT_TOPIC_P_FMT_VERSION    ESG_MQTT_TOPIC_P_FMT         "/version"
#define ESG_MQTT_TOPIC_P_FMT_STATUS     ESG_MQTT_TOPIC_P_FMT         "/status"
//
#define ESG_MQTT_TOPIC_S_FMT            ESG_MQTT_TOPIC_FMT_BASE_PATH "/down"


#define ESG_MQTT_P_BUFFER_STATUS_SIZE   64

#define ESG_VERSION_BUFFER_MAX  16

#define RADIO_ID    0   // TODO: This must be changed, via a Web interface will be good and saved to flash


void EnvSensorGateway::init(INetwork* network) {
    m_network = network;

    mqttBuildTopics();
    mqttInit();
    mqttSetCallbacks();
    m_mqtt.connect();

    loraInit();
    loraSetCallbacks();
    LOG_I(PINI_TAG_ESG, "Gateway radio ID: [%d]", myRadioId());
}


void EnvSensorGateway::loopController() {
    if (m_network->isConnected()) {
        m_mqtt.maintain();
    }
}

bool EnvSensorGateway::isTerminal() const {
    return false;
}
radioid_t EnvSensorGateway::myRadioId() const {
    return RADIO_ID;
}

void EnvSensorGateway::mqttBuildTopics() {
    /**** PUBLISH ****/
    snprintf(m_mqttTopicPublish_online,  sizeof(m_mqttTopicPublish_online),  ESG_MQTT_TOPIC_P_FMT_ONLINE,  myRadioId());
    snprintf(m_mqttTopicPublish_version, sizeof(m_mqttTopicPublish_version), ESG_MQTT_TOPIC_P_FMT_VERSION, myRadioId());
    snprintf(m_mqttTopicPublish_status,  sizeof(m_mqttTopicPublish_status),  ESG_MQTT_TOPIC_P_FMT_STATUS,  myRadioId());

    /**** SUBSCRIBE ****/

    LOG_D(PINI_TAG_ESG, "MQTT topics callbacks configured");
}

void EnvSensorGateway::mqttInit() {
    m_mqtt.setClient(m_network->getClient(), getUniqueId());
    m_mqtt.setServer(MQTT_SERVER, MQTT_PORT);
    m_mqtt.setCredentials(MQTT_USER, MQTT_PASS);
    
    m_mqtt.setWill(m_mqttTopicPublish_online, "0", 2, true);
}

void EnvSensorGateway::mqttSetCallbacks() {
    m_mqtt.onConnect(
        [this]() {
            m_mqtt.publish(m_mqttTopicPublish_online, "1", true);

            char version[ESG_VERSION_BUFFER_MAX];
            snprintf(version, sizeof(version), "%d", FIRMWARE_VERSION);
            m_mqtt.publish(m_mqttTopicPublish_version, version, false);
        }
    );

    m_mqtt.onSubscribe(
        [this](const char* topic) {
            LOG_D(PINI_TAG_ESG, "MQTT subscribe: [topic: '%s']", topic);
        }
    );
}

void EnvSensorGateway::loraSetCallbacks() {
    p_lora.onReceive(
        ELoRaTag::LT_STATUS,
        [this](radioid_t radioId, const uint8_t* payload, size_t size, int rssi, float snr) {
            LOG_D(PINI_TAG_ESG, "radioId=%llu size=%d rssi=%d snr=%f", radioId, size, rssi, snr);
            /*
            Serial.print("Payload: ");
            for (size_t i = 0; i < size; ++i) {
                Serial.print(payload[i], HEX);
                Serial.print(' ');
            }
            Serial.println();
            */

            if (size != sizeof(LoRaES_status_t)) {
                LOG_W(PINI_TAG_ESG,
                    "Received 'LT_STATUS' with invalid size: expected %d bytes, actual %d bytes. Skipping...",
                    size, sizeof(LoRaES_status_t)
                );
                return;
            }
            LoRaES_status_t* statusPayload = (LoRaES_status_t*)payload;
            LOG_D(PINI_TAG_ESG,
                "Received 'LT_STATUS': [radioId: %llu] [firmware: %d] [battery: %d] [temperature: %d] [humidity: %d]",
                radioId, statusPayload->firmware, statusPayload->battery,
                statusPayload->temperature, statusPayload->humidity
            );

            char mqttPayload[ESG_MQTT_P_BUFFER_STATUS_SIZE];
            snprintf(mqttPayload, sizeof(mqttPayload),
                "%llu,%d,%d,%d,%d,%d",
                radioId, statusPayload->firmware, rssi, statusPayload->battery, statusPayload->temperature, statusPayload->humidity
            );
            m_mqtt.publish(m_mqttTopicPublish_status, mqttPayload, false);
        }
    );
}

#endif // GATEWAY