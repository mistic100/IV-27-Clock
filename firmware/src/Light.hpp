#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include "Settings.hpp"
#include "constants.hpp"
#include "model.hpp"

float mapf(float x, float in_min, float in_max, float out_min, float out_max)
{
    const float run = in_max - in_min;
    if (run == 0)
    {
        log_e("map(): Invalid input range, min == max");
        return -1; // AVR returns -1, SAM returns 0
    }
    const float rise = out_max - out_min;
    const float delta = x - in_min;
    return (delta * rise) / run + out_min;
}

class Light
{
public:
    bool isOn = true;
    uint8_t brightness;
    LightMode mode;

    void begin()
    {
        pinMode(LIGHT_P, OUTPUT);

        brightness = SETTINGS.lightBrightness();
        mode = SETTINGS.lightMode();

        if (mode == LightMode::OFF)
        {
            off();
        }
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
            analogWrite(LIGHT_P, brightness * 25.5);
            break;
        case LightMode::BREATH:
        {
            // https://makersportal.com/blog/2020/3/27/simple-breathing-led-in-arduino
            static const float gamma = 0.14; // affects the width of peak (more or less darkness)
            static const float range = 10000.0;
            static uint16_t k = 0;

            auto val = exp(-pow((k / range - 0.5) / gamma, 2.0) / 2.0);
            auto pwm_val = mapf(val, 0, 1, 10, brightness * 25.5);
            analogWrite(LIGHT_P, pwm_val);

            k++;
            if (k == range)
            {
                k = 0;
            }
            break;
        }
        case LightMode::NOISE:
        {
            auto val = inoise8(millis());
            auto pwm_val = mapf(val, 0, 255, 0, brightness * 25.5);
            analogWrite(LIGHT_P, pwm_val);
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
        analogWrite(LIGHT_P, 0);
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

    void save()
    {
        SETTINGS.setLight(mode, brightness);
    }
};

Light LIGHT;
