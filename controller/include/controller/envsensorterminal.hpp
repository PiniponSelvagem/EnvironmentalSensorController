/**
* @file     envsensorterminal.hpp
* @brief    Environment terminal controller logic.
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

#ifndef _PINICONTROLLER_ENVSENSORTERMINAL_H_
#define _PINICONTROLLER_ENVSENSORTERMINAL_H_

#include <pinicore.hpp>
#include "ienvsensor.hpp"

class EnvSensorTerminal : public IEnvSensor {
    public:
        /**
         * @brief   Initialize the Terminal sensors and LoRa communication.
         */
        void init();

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
         * @brief   Initialize sensors.
         * @note    Temperature and humidity.
         */
        void sensorsInit();

        /**
         * @brief   Initialize battery status reader.
         */
        void batteryInit();

        /**
         * @brief   Send current device status and sensors via LoRa.
         */
        void loraSendStatus();


        DHT m_sensorTH;     // Digital Humidity and Temperature sensor.
        Battery m_battery;  // Battery status
};

#endif // _PINICONTROLLER_ENVSENSORTERMINAL_H_