#if defined(GATEWAY) || defined(TERMINAL)

#include "controller/lora/ienvsensorlora.hpp"
#include "global.hpp"


void IEnvSensorLora::loop() {
    p_lora.maintain();
    loopController();
}


void IEnvSensorLora::loraInit() {
    p_lora.init(
        PIN_LORA_MOSI, PIN_LORA_MISO, PIN_LORA_SCLK,
        PIN_LORA_CS, PIN_LORA_RST, PIN_LORA_DIO0,
        LORA_FREQUENCY_MHZ
    );
    p_lora.setRole(isTerminal(), myRadioId());
    p_lora.setCryptoPhrase(LORA_CRYPTO_PHRASE);
    p_lora.setSpreadingFactor(LORA_SF);
    p_lora.setTxPower(LORA_TX_POWER);
    p_lora.setBandwidth(LORA_BANDWIDTH);
}

#endif // GATEWAY || TERMINAL