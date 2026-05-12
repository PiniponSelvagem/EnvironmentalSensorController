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

class IEnvSensor {
    public:
        virtual ~IEnvSensor() = default;

        /**
         * @brief   The controller basic loop code.
         */
        virtual void loop() = 0;


    private:
        /**
         * @brief   The controller specific loop code.
         */
        virtual void loopController() = 0;
};

#endif // _PINICONTROLLER_IENVSENSOR_H_