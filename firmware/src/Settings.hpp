#pragma once

#include <ArduinoNvs.h>
#include "model.hpp"

static const char *KEY_LIGHT_MODE = "lm";
static const char *KEY_LIGHT_BRIGHTNESS = "lb";
static const char *KEY_DAYTIME_START = "dts";
static const char *KEY_DAYTIME_END = "dte";

class Settings
{
public:
    void begin()
    {
        NVS.begin();
    }

    const LightMode lightMode() const
    {
        return static_cast<LightMode>(NVS.getInt(KEY_LIGHT_MODE, static_cast<uint8_t>(LightMode::ON)));
    }

    const uint8_t lightBrightness() const
    {
        return NVS.getInt(KEY_LIGHT_BRIGHTNESS, 5);
    }

    const uint16_t daytimeStart() const
    {
        return NVS.getInt(KEY_DAYTIME_START, 9 * 60 + 30);
    }

    const uint16_t daytimeEnd() const
    {
        return NVS.getInt(KEY_DAYTIME_END, 23 * 60 + 30);
    }

    void setLight(const LightMode mode, uint8_t brightness)
    {
        NVS.setInt(KEY_LIGHT_MODE, static_cast<uint8_t>(mode), false);
        NVS.setInt(KEY_LIGHT_BRIGHTNESS, brightness, false);
        NVS.commit();
    }

    void setDaytime(uint16_t start, uint16_t end)
    {
        NVS.setInt(KEY_DAYTIME_START, start, false);
        NVS.setInt(KEY_DAYTIME_END, end, false);
        NVS.commit();
    }
};

Settings SETTINGS;
