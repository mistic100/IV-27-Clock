#pragma once

#include <map>
#include <Arduino.h>
#include <FastLED.h>
#include "constants.hpp"

std::array<byte, 7> SEGMENTS_BLANK = {0, 0, 0, 0, 0, 0, 0};

// ASCII 48-57
std::array<byte, 7> SEGMENTS_NUMS[] = {
    {1, 1, 1, 1, 1, 1, 0}, // 0
    {0, 1, 1, 0, 0, 0, 0}, // 1
    {1, 1, 0, 1, 1, 0, 1}, // 2
    {1, 1, 1, 1, 0, 0, 1}, // 3
    {0, 1, 1, 0, 0, 1, 1}, // 4
    {1, 0, 1, 1, 0, 1, 1}, // 5
    {1, 0, 1, 1, 1, 1, 1}, // 6
    {1, 1, 1, 0, 0, 0, 0}, // 7
    {1, 1, 1, 1, 1, 1, 1}, // 8
    {1, 1, 1, 0, 0, 1, 1}, // 9
};

// ASCII 65-90
std::array<byte, 7> SEGMENTS_ALPHA[] = {
    {1, 1, 1, 0, 1, 1, 1}, // A
    {0, 0, 1, 1, 1, 1, 1}, // b
    {1, 0, 0, 1, 1, 1, 0}, // C
    {0, 1, 1, 1, 1, 0, 1}, // d
    {1, 0, 0, 1, 1, 1, 1}, // E
    {1, 0, 0, 0, 1, 1, 1}, // F
    {1, 0, 1, 1, 1, 1, 0}, // G
    {0, 1, 1, 0, 1, 1, 1}, // H
    {0, 1, 1, 0, 0, 0, 0}, // I
    {0, 1, 1, 1, 1, 0, 0}, // J
    {0, 1, 0, 1, 1, 1, 1}, // K
    {0, 0, 0, 1, 1, 1, 0}, // L
    {1, 1, 0, 1, 0, 1, 0}, // M
    {1, 1, 1, 0, 1, 1, 0}, // N
    {1, 1, 1, 1, 1, 1, 0}, // O
    {1, 1, 0, 0, 1, 1, 1}, // P
    {1, 1, 0, 1, 0, 1, 1}, // Q
    {1, 1, 0, 0, 1, 1, 0}, // R
    {1, 0, 1, 1, 0, 1, 1}, // S
    {0, 0, 0, 1, 1, 1, 1}, // t
    {0, 1, 1, 1, 1, 1, 0}, // U
    {0, 1, 0, 0, 0, 1, 1}, // v
    {0, 1, 1, 1, 1, 1, 1}, // W
    {0, 0, 1, 0, 0, 1, 1}, // X
    {0, 1, 1, 1, 0, 1, 1}, // Y
    {1, 1, 0, 1, 1, 0, 1}, // Z
};

std::map<char, std::array<byte, 7>> SEGMENTS_SYM = {
    {'-', {{0, 0, 0, 0, 0, 0, 1}}},
    {'_', {{0, 0, 0, 1, 0, 0, 0}}},
    {'/', {{0, 1, 0, 0, 1, 0, 1}}},
    {'%', {{1, 1, 0, 0, 0, 1, 1}}}, // °
};

class Display
{
private:
    String str;

    char chars[NUM_GRIDS];
    byte dots[NUM_GRIDS];
    byte blinks[NUM_GRIDS];
    byte data[NUM_OUTS];

    bool blinkState = false;
    int scrollOffset = 0;

public:
    Display()
    {
        clear();
    }

    void begin()
    {
        pinMode(DIN, OUTPUT);
        pinMode(CLK, OUTPUT);
        pinMode(LOAD, OUTPUT);
        pinMode(BLANK, OUTPUT);

        digitalWrite(DIN, LOW);
        digitalWrite(CLK, LOW);
        digitalWrite(LOAD, LOW);
        digitalWrite(BLANK, LOW);
    }

    void on()
    {
        digitalWrite(BLANK, LOW);
    }

    void off()
    {
        digitalWrite(BLANK, HIGH);
    }

    void clear()
    {
        scrollOffset = 0;

        str.clear();

        for (byte i = 0; i < NUM_GRIDS; i++)
        {
            chars[i] = ' ';
            dots[i] = LOW;
            blinks[i] = LOW;
        }
        for (byte i = 0; i < NUM_OUTS; i++)
        {
            data[i] = LOW;
        }
    }

    /**
     * Changes the displayed text, optionnally adding dots at specific positions (1-based)
     * If "str" is longer than NUM_GRIDS it will perform a scroll animation and ignore "dots"
     */
    void print(const String &str, const std::vector<byte> &dots = {})
    {
        clear();

        if (str.length() > NUM_GRIDS)
        {
            this->str = str;
            scrollOffset = -NUM_GRIDS;
        }
        else
        {
            for (byte i = 0; i < NUM_GRIDS; i++)
            {
                this->chars[i] = str[i] ? str[i] : ' ';
            }

            for (const auto &dot : dots)
            {
                if (dot >= 1 && dot <= NUM_GRIDS)
                {
                    this->dots[dot - 1] = HIGH;
                }
            }
        }
    }

    /**
     * Set blinking positions (1-based)
     */
    void blink(const std::vector<byte> &positions)
    {
        for (const auto &pos : positions)
        {
            if (pos >= 1 && pos <= NUM_GRIDS)
            {
                blinks[pos - 1] = HIGH;
            }
        }
    }

    void blinkAll()
    {
        for (byte i = 0; i < NUM_GRIDS; i++)
        {
            blinks[i] = HIGH;
        }
    }

    void loop()
    {
        EVERY_N_MILLIS(BLINK_INTERVAL_MS)
        {
            blink();
        }

        EVERY_N_MILLIS(SCROLL_INTERVAL_MS)
        {
            scroll();
        }

        // for each character
        for (byte i = 0; i < NUM_GRIDS; i++)
        {
            // set the active grid
            data[GRID[i]] = 1;
            // set the segments
            setChar(i);

            // send the data
            for (int k = NUM_OUTS - 1; k >= 0; k--)
            {
                digitalWrite(DIN, data[k]);
                clock();
            }
            load();

            // reset the active grid
            data[GRID[i]] = 0;
        }
    }

private:
    void clock()
    {
        digitalWrite(CLK, HIGH);
        digitalWrite(CLK, LOW);
    }

    void load()
    {
        digitalWrite(LOAD, HIGH);
        digitalWrite(LOAD, LOW);
    }

    void blink()
    {
        blinkState = !blinkState;
    }

    void scroll()
    {
        if (!str.isEmpty())
        {
            for (byte i = 0; i < NUM_GRIDS; i++)
            {
                auto idx = scrollOffset + i;
                chars[i] = idx < 0 || idx >= str.length() ? ' ' : str[idx];
            }

            scrollOffset++;
            if (scrollOffset >= (int)str.length())
            {
                scrollOffset = -NUM_GRIDS;
            }
        }
    }

    /**
     * Updates `data` 8 segments for current character
     */
    void setChar(byte index)
    {
        if (isBlinked(index))
        {
            setChar(SEGMENTS_BLANK);
            data[SEGMENTS[7]] = LOW;
            return;
        }

        const char s0 = chars[index];

        if (s0 >= 48 && s0 <= 57)
        {
            setChar(SEGMENTS_NUMS[s0 - 48]);
        }
        else if (s0 >= 65 && s0 <= 90)
        {
            setChar(SEGMENTS_ALPHA[s0 - 65]);
        }
        else if (s0 >= 97 && s0 <= 122)
        {
            setChar(SEGMENTS_ALPHA[s0 - 97]);
        }
        else if (SEGMENTS_SYM.contains(s0))
        {
            setChar(SEGMENTS_SYM[s0]);
        }
        else
        {
            setChar(SEGMENTS_BLANK);
        }

        data[SEGMENTS[7]] = (s0 == '.' || dots[index] == HIGH) ? HIGH : LOW;
    }

    void setChar(const std::array<byte, 7> &s)
    {
        for (byte i = 0; i < 7; i++)
        {
            data[SEGMENTS[i]] = s[i];
        }
    }

    boolean isBlinked(byte index)
    {
        return !blinkState && blinks[index] == HIGH;
    }
};
