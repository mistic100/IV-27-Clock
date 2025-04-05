#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include "constants.hpp"
#include "model.hpp"

class Light
{
public:
    bool isOn = true;
    uint8_t brightness = 10;
    LightMode mode = LightMode::BREATH;

    void begin()
    {
        pinMode(LIGHT, OUTPUT);
    }

    void loop()
    {
        if (!isOn)
        {
            return;
        }

        switch (mode)
        {
        case LightMode::ON:
            analogWrite(LIGHT, brightness * 25.5);
            break;
        case LightMode::BREATH:
        {
            static const float gamma = 0.14; // affects the width of peak (more or less darkness)
            static const float range = 10000.0;
            static uint16_t k = 0;

            float pwm_val = 25.5 + (brightness - 1) * 25.5 * exp(-pow((k / range - 0.5) / gamma, 2.0) / 2.0);
            analogWrite(LIGHT, pwm_val);

            k++;
            if (k == range)
            {
                k = 0;
            }
            break;
        }
        }
    }

    void on()
    {
        isOn = true;
    }

    void off()
    {
        isOn = false;
        analogWrite(LIGHT, 0);
    }

    void nextMode()
    {
        ++mode;

        if (mode == LightMode::OFF)
        {
            off();
        }
        else
        {
            on();
        }
    }

    void prevMode()
    {
        --mode;

        if (mode == LightMode::OFF)
        {
            off();
        }
        else
        {
            on();
        }
    }

    void incBrightness()
    {
        if (brightness < 10)
        {
            brightness++;
        }
    }

    void decBrightness()
    {
        if (brightness > 0)
        {
            brightness--;
        }
    }
};
