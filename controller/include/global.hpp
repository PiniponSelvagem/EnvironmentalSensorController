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
#endif
//#define WDTG_INTERNAL_TIMER_IN_SECONDS   90
/***************/
