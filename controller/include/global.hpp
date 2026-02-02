#pragma once

/* Localization used in WiFi AP page */
//#define LANGUAGE_SELECTION__PT_PT
//#define LANGUAGE_SELECTION__EN_US

/* Device specific configuration */
#define FIRMWARE_VERSION 1
#ifdef GATEWAY
    #define STORAGE_ID "PINI_GATEWAY_ENVSENSOR"
#else
    #define STORAGE_ID "PINI_TERMINAL_ENVSENSOR"
    /* PIN defines */
        #define PIN_BATTERY 35  // Battery read
        #define PIN_DHT     22  // Digital Humidity Temperature sensor
        #define PIN_WAKEUP  14  // Wake-up on button press
    /***************/
    #define WAKEUP_TIME_MS  10000    // Milliseconds that should stay awake after button press
    #define SLEEP_TIME_SECS (15) // Amount of seconds the controller should sleep between sensors reporting
#endif
#define WDTG_INTERNAL_TIMER_IN_SECONDS   120
//#define USE_GSM_NETWORK   // Use GSM network instead of WiFi. Currently not supported.

/* Common PIN defines */
    #define PIN_LED         15
    #define PIN_I2C_SDA     33 
    #define PIN_I2C_SCL     32
    /** LoRa **/
        #define PIN_LORA_MOSI   27
        #define PIN_LORA_MISO   19
        #define PIN_LORA_SCLK   5
        #define PIN_LORA_CS     18
        #define PIN_LORA_RST    23 
        #define PIN_LORA_DIO0   26
    /**********/
/**********************/

/** LoRa configuration **/
#define LORA_FREQUENCY_MHZ  868
#define LORA_BANDWIDTH      ELoRaBandwidth::LR_BW_125_KHZ
#define LORA_SF             7
#define LORA_TX_POWER       20
#define LORA_CRYPTO_PHRASE  0xC4
/************************/
/*********************************/
