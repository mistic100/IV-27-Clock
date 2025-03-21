#pragma once

// enable Wifi OTA
#define WIFI_OTA
// enable BME280 ambient sensor
#define BME280_SENSOR
// enable message display from Home Assistant
#define HA_MESSAGE

#if defined(WIFI_OTA) || defined(HA_MESSAGE)
#define WIFI
#endif

#define DIN D10
#define CLK D9
#define LOAD D8
#define BLANK D0

#define HOSTNAME "iv27-clock"
#define HA_UPDATE_INTERVAL_MS 30000
#define BLINK_INTERVAL_MS 300
#define SCROLL_INTERVAL_MS 200
#define TEMP_OFFSET -2

#define NUM_GRIDS 12
#define NUM_OUTS 20

uint8_t GRID[NUM_GRIDS] = {
    11, // 12
    10, // 11
    9, // 10
    8, // 9
    7, // 8
    6, // 7
    5, // 6
    4, // 5
    3, // 4
    2, // 3
    1, // 2
    0, // 1
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
