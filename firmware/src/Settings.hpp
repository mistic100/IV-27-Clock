#pragma once

#include <ArduinoNvs.h>
#include "model.hpp"

static const char *KEY_DISPLAY_MODE = "dm";
static const char *KEY_LIGHT_MODE = "lm";
static const char *KEY_LIGHT_BRIGHTNESS = "lb";
static const char *KEY_DAYTIME_START = "dts";
static const char *KEY_DAYTIME_END = "dte";
static const char *KEY_MESSAGE_TIMEOUT = "mt";
static const char *KEY_TEMP_OFFSET = "to";

class Settings
{
public:
    void begin()
    {
        NVS.begin();
    }

    const DisplayMode displayMode() const
    {
        return static_cast<DisplayMode>(NVS.getInt(KEY_DISPLAY_MODE, static_cast<uint8_t>(DisplayMode::TIME)));
    }

    const LightMode lightMode() const
    {
        return static_cast<LightMode>(NVS.getInt(KEY_LIGHT_MODE, static_cast<uint8_t>(LightMode::ON)));
    }

    const uint8_t lightBrightness() const
    {
        return NVS.getInt(KEY_LIGHT_BRIGHTNESS, 12);
    }

    const uint16_t daytimeStart() const
    {
        return NVS.getInt(KEY_DAYTIME_START, 9 * 60 + 30);
    }

    const uint16_t daytimeEnd() const
    {
        return NVS.getInt(KEY_DAYTIME_END, 23 * 60 + 30);
    }

#ifdef USE_TEMP_SENSOR
    const float tempOffset() const
    {
        return NVS.getFloat(KEY_TEMP_OFFSET, 0);
    }
#endif

#ifdef USE_HA_MESSAGE
    const uint8_t messageTimeout() const
    {
        return NVS.getInt(KEY_MESSAGE_TIMEOUT, 30);
    }
#endif

    void setDisplayMode(const DisplayMode mode)
    {
        NVS.setInt(KEY_DISPLAY_MODE, static_cast<uint8_t>(mode), false);
        NVS.commit();
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

#ifdef USE_TEMP_SENSOR
    void setTempOffset(float offset)
    {
        NVS.setFloat(KEY_TEMP_OFFSET, offset, false);
        NVS.commit();
    }
#endif

#ifdef USE_HA_MESSAGE
    void setMessageTimeout(uint8_t timeout)
    {
        NVS.setInt(KEY_MESSAGE_TIMEOUT, timeout, false);
        NVS.commit();
    }
#endif
};

Settings SETTINGS;
