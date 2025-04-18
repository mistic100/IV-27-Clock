#pragma once

#include "secrets.hpp"

// enable NTP (comment to use RTC)
#define USE_NTP
// enable Wifi OTA
#define USE_WIFI_OTA
// enable BME280 ambient sensor
#define USE_TEMP_SENSOR
// enable message display from Home Assistant
#define USE_HA_MESSAGE
// enable turn off from Home Assistant home occupancy
#define USE_HA_OCCUPANCY

#if defined(USE_HA_MESSAGE) || defined(USE_HA_OCCUPANCY)
#define USE_HA
#endif

#if defined(USE_NTP) || defined(USE_WIFI_OTA) || defined(USE_HA)
#define USE_WIFI
#endif

#ifndef USE_NTP
#define USE_RTC
#endif

#define DRIVER_DIN D10
#define DRIVER_CLK D8
#define DRIVER_LOAD D9

#define LIGHT_P D0

#define ENCODER_A D2
#define ENCODER_B D3
#define ENCODER_SW D1

#ifdef USE_NTP
// https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
#define TIMEZONE "CET-1CEST,M3.5.0,M10.5.0/3"
#endif
#ifdef USE_WIFI_OTA
#define HOSTNAME "iv27-clock"
#endif
#ifdef USE_HA
#define HA_UPDATE_INTERVAL_S 60
#endif
#ifdef USE_TEMP_SENSOR
#define TEMP_UPDATE_INTERVAL_S 10
#endif

#define BLINK_INTERVAL_MS 300
#define SCROLL_INTERVAL_MS 150

#define NUM_GRIDS 12
#define NUM_OUTS 20

uint8_t GRID[NUM_GRIDS] = {
    // 14 unused
    2,  // 13
    18, // 12
    1,  // 11
    17, // 10
    0,  // 9
    16, // 8
    5,  // 7
    7,  // 6
    6,  // 5
    4,  // 4
    19, // 3
    3,  // 2
    // 1 unused
};

uint8_t SEGMENTS[8] = {
    10, // A
    12, // B
    15, // C
    9,  // D
    14, // E
    11, // F
    13, // G
    8,  // Pt
};
