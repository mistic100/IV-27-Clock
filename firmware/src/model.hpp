#pragma once

#include "constants.hpp"

struct HaData {
    bool atHome = false;
    String message;
};

enum class DisplayMode
{
    OFF,

    // main display modes
    TIME,
    DATE,
#ifdef USE_TEMP_SENSOR
    TEMP,
#endif
#ifdef USE_HA_MESSAGE
    MESSAGE,
#endif
    FIRST_ITEM = TIME,
#ifdef USE_HA_MESSAGE
    LAST_ITEM = MESSAGE,
#elifdef USE_TEMP_SENSOR
    LAST_ITEM = TEMP,
#else
    LAST_ITEM = DATE,
#endif

    // other modes
    MENU,
    SET_DATE_FORMAT,
    SET_LIGHT,
#ifdef USE_RTC
    SET_DATE,
    SET_TIME,
#endif
    SET_DAYTIME,
#ifdef USE_TEMP_SENSOR
    SET_TEMP_OFFSET,
#endif
#ifdef USE_HA_MESSAGE
    SET_MESSAGE_TIMEOUT,
#endif
};

bool isMainDisplayMode(const DisplayMode &i)
{
    return static_cast<int>(i) >= static_cast<int>(DisplayMode::FIRST_ITEM) && static_cast<int>(i) <= static_cast<int>(DisplayMode::LAST_ITEM);
}

DisplayMode &operator++(DisplayMode &i)
{
    if (i == DisplayMode::LAST_ITEM)
    {
        i = DisplayMode::FIRST_ITEM;
    }
    else
    {
        i = static_cast<DisplayMode>(static_cast<uint8_t>(i) + 1);
    }
    return i;
}

DisplayMode &operator--(DisplayMode &i)
{
    if (i == DisplayMode::FIRST_ITEM)
    {
        i = DisplayMode::LAST_ITEM;
    }
    else
    {
        i = static_cast<DisplayMode>(static_cast<uint8_t>(i) - 1);
    }
    return i;
}

enum class MenuItem
{
    NONE,

    // main menu items
    SET_DATE_FORMAT,
    SET_LIGHT,
#ifdef USE_RTC
    SET_DATE,
    SET_TIME,
#endif
    SET_DAYTIME,
#ifdef USE_TEMP_SENSOR
    SET_TEMP_OFFSET,
#endif
#ifdef USE_HA_MESSAGE
    SET_MESSAGE_TIMEOUT,
#endif
    BACK,
    FIRST_ITEM = SET_DATE_FORMAT,
    LAST_ITEM = BACK,

// other items
#ifdef USE_RTC
    YEAR,
    MONTH,
    DAY,
    HOURS,
    MINUTES,
    SECONDS,
#endif
    MODE,
    BRIGHT,
    START,
    END,
};

MenuItem &operator++(MenuItem &i)
{
    if (i == MenuItem::LAST_ITEM)
    {
        i = MenuItem::FIRST_ITEM;
    }
    else
    {
        i = static_cast<MenuItem>(static_cast<uint8_t>(i) + 1);
    }
    return i;
}

MenuItem &operator--(MenuItem &i)
{
    if (i == MenuItem::FIRST_ITEM)
    {
        i = MenuItem::LAST_ITEM;
    }
    else
    {
        i = static_cast<MenuItem>(static_cast<uint8_t>(i) - 1);
    }
    return i;
}

enum class LightMode
{
    OFF,
    ON,
    BREATH,
    NOISE,

    FIRST_ITEM = OFF,
    LAST_ITEM = NOISE
};

LightMode &operator++(LightMode &i)
{
    if (i == LightMode::LAST_ITEM)
    {
        i = LightMode::FIRST_ITEM;
    }
    else
    {
        i = static_cast<LightMode>(static_cast<uint8_t>(i) + 1);
    }
    return i;
}

LightMode &operator--(LightMode &i)
{
    if (i == LightMode::FIRST_ITEM)
    {
        i = LightMode::LAST_ITEM;
    }
    else
    {
        i = static_cast<LightMode>(static_cast<uint8_t>(i) - 1);
    }
    return i;
}

const char *OFF_STR = "OFF";
const char *ON_STR = "STATIC";
const char *BREATH_STR = "BREATH";
const char *NOISE_STR = "NOISE";
const char *EMPTY_STR = "";

const char *lightModeStr(const LightMode &mode)
{
    switch (mode)
    {
    case LightMode::OFF:
        return OFF_STR;
    case LightMode::ON:
        return ON_STR;
    case LightMode::BREATH:
        return BREATH_STR;
    case LightMode::NOISE:
        return NOISE_STR;
    default:
        return EMPTY_STR;
    }
}

enum class DateFormat
{
    FULL,
    DAY_MONTH,

    FIRST_ITEM = FULL,
    LAST_ITEM = DAY_MONTH
};

DateFormat &operator++(DateFormat &i)
{
    if (i == DateFormat::LAST_ITEM)
    {
        i = DateFormat::FIRST_ITEM;
    }
    else
    {
        i = static_cast<DateFormat>(static_cast<uint8_t>(i) + 1);
    }
    return i;
}

DateFormat &operator--(DateFormat &i)
{
    if (i == DateFormat::FIRST_ITEM)
    {
        i = DateFormat::LAST_ITEM;
    }
    else
    {
        i = static_cast<DateFormat>(static_cast<uint8_t>(i) - 1);
    }
    return i;
}

const char *FULL_STR = "YYYY-MM-DD";
const char *DAY_MONTH_STR = "DAY MONTH";

const char *dateFormatStr(const DateFormat &mode)
{
    switch (mode)
    {
    case DateFormat::FULL:
        return FULL_STR;
    case DateFormat::DAY_MONTH:
        return DAY_MONTH_STR;
    default:
        return EMPTY_STR;
    }
}
