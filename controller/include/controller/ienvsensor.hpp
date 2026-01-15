/**
* @file     ienvsensor.hpp
* @brief    Interface for environment controller logic.
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

#ifndef _PINICONTROLLER_IENVSENSOR_H_
#define _PINICONTROLLER_IENVSENSOR_H_

#include <pinicore.hpp>

/**
 * @brief   LoRa message tagId.
 */
enum ELoRaTag : uint8_t {
    LT_STATUS = 1
};

/**
 * @brief   LoRa payload for tagId STATUS.
 * @note    Packed so there is no padding and do not waste any bytes during Tx/Rx.
 */
typedef struct __attribute__((packed)) {
    uint16_t firmware;
    uint8_t battery;
    uint8_t humidity;
    int8_t temperature;
} LoRaES_status_t;

class IEnvSensor {
    public:
        virtual ~IEnvSensor() = default;

        /**
         * @brief   The controller basic loop code.
         */
        void loop();

        
    protected:
        /**
         * @brief   Initialize the LoRa hardware.
         */
        void loraInit();

        LoRaComm p_lora;    // The LoRa communication hardware.


    private:
        /**
         * @brief   The controller specific loop code.
         */
        virtual void loopController() = 0;

        /**
         * @brief   Controller type, Terminal/Gateway.
         * @return  True if is terminal, false if Gateway.
         */
        virtual bool isTerminal() const = 0;

        /**
         * @brief   Controller radioId.
         * @return  RadioId that identifies this controller.
         */
        virtual radioid_t myRadioId() const = 0;
};

#endif // _PINICONTROLLER_IENVSENSOR_H_