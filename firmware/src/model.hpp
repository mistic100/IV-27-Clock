#pragma once

enum class DisplayMode
{
    OFF,

    // main display modes
    TIME,
    DATE,
    FIRST_ITEM = TIME,
    LAST_ITEM = DATE,

    // other modes
    MENU,
    SET_DATE,
    SET_TIME,
};

DisplayMode &operator++(DisplayMode &i, int)
{
    if (i == DisplayMode::LAST_ITEM)
    {
        i = DisplayMode::FIRST_ITEM;
    }
    else
    {
        i = static_cast<DisplayMode>(static_cast<int>(i) + 1);
    }
    return i;
}

DisplayMode &operator--(DisplayMode &i, int)
{
    if (i == DisplayMode::FIRST_ITEM)
    {
        i = DisplayMode::LAST_ITEM;
    }
    else
    {
        i = static_cast<DisplayMode>(static_cast<int>(i) - 1);
    }
    return i;
}

enum class MenuItem
{
    NONE,

    // main menu items
    SET_DATE,
    SET_TIME,
    BACK,
    FIRST_ITEM = SET_DATE,
    LAST_ITEM = BACK,

    // other items
    YEAR,
    MONTH,
    DAY,
    HOURS,
    MINUTES,
    SECONDS,
    DONE
};

MenuItem &operator++(MenuItem &i, int)
{
    if (i == MenuItem::LAST_ITEM)
    {
        i = MenuItem::FIRST_ITEM;
    }
    else
    {
        i = static_cast<MenuItem>(static_cast<int>(i) + 1);
    }
    return i;
}

MenuItem &operator--(MenuItem &i, int)
{
    if (i == MenuItem::FIRST_ITEM)
    {
        i = MenuItem::LAST_ITEM;
    }
    else
    {
        i = static_cast<MenuItem>(static_cast<int>(i) - 1);
    }
    return i;
}