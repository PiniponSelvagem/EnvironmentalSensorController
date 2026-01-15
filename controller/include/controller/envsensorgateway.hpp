/**
* @file     envsensorgateway.hpp
* @brief    Environment gateway controller logic.
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

#ifndef _PINICONTROLLER_ENVSENSORGATEWAY_H_
#define _PINICONTROLLER_ENVSENSORGATEWAY_H_

#include <pinicore.hpp>
#include "ienvsensor.hpp"

#define ESG_MQTT_TOPIC_MAX_SIZE 64

class EnvSensorGateway : public IEnvSensor {
    public:
        /**
         * @brief   Initialize the Gateway using a specific network interface, MQTT and LoRa communication.
         * @param   network Network interface to use for internet communication.
         */
        void init(INetwork* network);

    private:
        /**
         * @brief   The controller specific loop code.
         */
        void loopController() override;

        /**
         * @brief   Controller type, Terminal/Gateway.
         * @return  True if is terminal, false if Gateway.
         */
        bool isTerminal() const override; 

        /**
         * @brief   Controller radioId.
         * @return  RadioId that identifies this controller.
         */
        radioid_t myRadioId() const override;

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

        /**
         * @brief   Set all callbacks necessary for LoRa communication operation.
         */
        void loraSetCallbacks();


        INetwork* m_network;    // Internet network interface.
        MQTT m_mqtt;            // MQTT client.
        //
        char m_mqttTopicPublish_online[ESG_MQTT_TOPIC_MAX_SIZE];    // PUBLISH topic: online
        char m_mqttTopicPublish_version[ESG_MQTT_TOPIC_MAX_SIZE];   // PUBLISH topic: firmware version
        char m_mqttTopicPublish_status[ESG_MQTT_TOPIC_MAX_SIZE];    // PUBLISH topic: terminal status reporting
};

#endif // _PINICONTROLLER_ENVSENSORGATEWAY_H_