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

#include <Adafruit_SSD1306.h>

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
         * @brief   Initialize and configure sleep function.
         */
        void sleepInit();

        /**
         * @brief   Initialize the OLED display.
         */
        void oledInit();

        /**
         * @brief   Read all available sensors and save them into internal variables.
         */
        void readSensors();

        /**
         * @brief   Renders to display the current sensors status.
         */
        void oledRender();

        /**
         * @brief   Send current device status and sensors via LoRa.
         */
        void loraSendStatus();

        /**
         * @brief   Sleep now.
         */
        void sleep();


        DHT m_sensorTH;     // Digital Humidity and Temperature sensor.
        Battery m_battery;  // Battery status

        uint64_t m_initEndAt = 0; // Millis since boot that the controller was ready at.
        bool m_manualWakeUp = false;

        bool m_uploaded = false;
        Adafruit_SSD1306 m_oled = Adafruit_SSD1306(128, 64); // TODO: Add support for SSD1306 in PiniCore.

        uint64_t m_sensorsReadAt;
        float m_humidity;
        float m_temperature;
        uint8_t m_batteryPercentage;
};

#endif // _PINICONTROLLER_ENVSENSORTERMINAL_H_