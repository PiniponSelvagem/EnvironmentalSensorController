#include "controller/ienvsensor.hpp"


void IEnvSensor::loop() {
    p_lora.maintain();
    loopController();
}


void IEnvSensor::loraInit() {
    p_lora.init(27,19,5,18,23,26, 864);
    p_lora.setRole(isTerminal(), myRadioId());
    p_lora.setCryptoPhrase(0xC4);
    p_lora.setSpreadingFactor(7);
    p_lora.setTxPower(20);
    p_lora.setBandwidth(ELoRaBandwidth::LR_BW_125_KHZ);
}

