#pragma once

#define DIN D10
#define CLK D9
#define LOAD D8
#define BLANK D0

#define BLINK_TIME 300
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
