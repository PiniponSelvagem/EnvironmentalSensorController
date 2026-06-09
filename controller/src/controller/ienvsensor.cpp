#include "controller/ienvsensor.hpp"
#include <pinicore.hpp>

using namespace pinicore;

#define PINI_TAG_IENVSENSOR "ienvsensor"

void IEnvSensor::restart() {
    LOG_I(PINI_TAG_IENVSENSOR, "Restaring...");
    ESP.restart();
}