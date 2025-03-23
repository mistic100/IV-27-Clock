#pragma once

#include "secrets.hpp"

// enable NTP (comment to use RTC)
#define USE_NTP
// enable Wifi OTA
#define USE_WIFI_OTA
// enable BME280 ambient sensor
#define USE_BME280_SENSOR
// enable message display from Home Assistant
#define USE_HA_MESSAGE

#if defined(USE_NTP) || defined(USE_WIFI_OTA) || defined(USE_HA_MESSAGE)
#define WIFI
#endif

#ifndef USE_NTP
#define USE_RTC
#endif

#define DIN D10
#define CLK D9
#define LOAD D8
#define BLANK D0

#define ENCODER_A D1
#define ENCODER_B D2
#define ENCODER_SW D3

#ifdef USE_NTP
// https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
#define TIMEZONE "CET-1CEST,M3.5.0,M10.5.0/3"
#endif
#ifdef USE_WIFI_OTA
#define HOSTNAME "iv27-clock"
#endif
#ifdef USE_HA_MESSAGE
#define HA_UPDATE_INTERVAL_S 60
#endif
#ifdef USE_BME280_SENSOR
#define TEMP_OFFSET -2
#define TEMP_UPDATE_INTERVAL_S 10
#endif

#define BLINK_INTERVAL_MS 300
#define SCROLL_INTERVAL_MS 200

#define NUM_GRIDS 12
#define NUM_OUTS 20

uint8_t GRID[NUM_GRIDS] = {
    11, // 12
    10, // 11
    9,  // 10
    8,  // 9
    7,  // 8
    6,  // 7
    5,  // 6
    4,  // 5
    3,  // 4
    2,  // 3
    1,  // 2
    0,  // 1
};

uint8_t SEGMENTS[8] = {
    13, // A
    14, // B
    15, // C
    16, // D
    17, // E
    18, // F
    19, // G
    12, // Pt
};
