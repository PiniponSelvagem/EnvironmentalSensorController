/**
* @file     envsensorstandalone_spriglabs.hpp
* @brief    Environment standalone controller logic, SprigLabs variant.
* @author   PiniponSelvagem
*
* Copyright(C) PiniponSelvagem
*
***********************************************************************
* Software that is described here, is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
**********************************************************************/

#pragma once

#ifndef _PINICONTROLLER_ENVSENSORSTANDALONE_SPRILABS_H_
#define _PINICONTROLLER_ENVSENSORSTANDALONE_SPRILABS_H_

#include <pinicore.hpp>
#include "controller/ienvsensor.hpp"

using namespace pinicore;

#define ESA_MQTT_TOPIC_MAX_SIZE 64

class EnvSensorStandaloneSprigLabs : public IEnvSensor {
    public:
        /**
         * @brief   Initialize the Standalone using a specific network interface and MQTT.
         * @param   network Network interface to use for internet communication.
         */
        void init(INetwork* network);

        /**
         * @brief   The controller basic loop code.
         */
        void loop() override;


    private:
        /**
         * @brief   The controller specific loop code.
         */
        void loopController() override;

        /**
         * @brief   Build MQTT topics for later use.
         */
        void mqttBuildTopics();

        /**
         * @brief   Initialize the MQTT client.
         */
        void mqttInit();

        /**
         * @brief   Set all callbacks necessary for MQTT operation.
         */
        void mqttSetCallbacks();


        INetwork* m_network;    // Internet network interface.
        MQTT m_mqtt;            // MQTT client.
        //
        char m_mqttTopicPublish_online[ESA_MQTT_TOPIC_MAX_SIZE];      // PUBLISH topic: online
        char m_mqttTopicPublish_version[ESA_MQTT_TOPIC_MAX_SIZE];     // PUBLISH topic: firmware version
        char m_mqttTopicPublish_type[ESA_MQTT_TOPIC_MAX_SIZE];        // PUBLISH topic: standalone type
        char m_mqttTopicPublish_battery[ESA_MQTT_TOPIC_MAX_SIZE];     // PUBLISH topic: battery percentage
        char m_mqttTopicPublish_lux[ESA_MQTT_TOPIC_MAX_SIZE];         // PUBLISH topic: lux
        char m_mqttTopicPublish_temperature[ESA_MQTT_TOPIC_MAX_SIZE]; // PUBLISH topic: temperature
        char m_mqttTopicPublish_humidity[ESA_MQTT_TOPIC_MAX_SIZE];    // PUBLISH topic: humidity
        //
        uint64_t m_lastSensorUploadAt; // Last millis that sent sensor data

        SprigC3 m_sprigC;       // ESP32 from SprigLabs
        SprigRoot m_sprigRoot;  // ROOT, Plant sensor from SprigLabs
};

#endif // _PINICONTROLLER_ENVSENSORSTANDALONE_SPRILABS_H_