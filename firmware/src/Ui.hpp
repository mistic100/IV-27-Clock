#pragma once

#include "constants.hpp"
#include "model.hpp"
#include "Controller.hpp"

class Ui
{
public:
    void click()
    {
        switch (CTRL.mode)
        {
        case DisplayMode::MENU:
            switch (CTRL.item)
            {
#ifdef USE_RTC
            case MenuItem::SET_DATE:
                CTRL.dateTime.update();
                CTRL.setMode(DisplayMode::SET_DATE, MenuItem::YEAR);
                break;
            case MenuItem::SET_TIME:
                CTRL.dateTime.update();
                CTRL.setMode(DisplayMode::SET_TIME, MenuItem::HOURS);
                break;
#endif
            case MenuItem::SET_LIGHT:
                CTRL.setMode(DisplayMode::SET_LIGHT, MenuItem::MODE);
                break;
            case MenuItem::SET_DAYTIME:
                CTRL.setMode(DisplayMode::SET_DAYTIME, MenuItem::START);
                break;
#ifdef USE_TEMP_SENSOR
            case MenuItem::SET_TEMP_OFFSET:
                CTRL.setMode(DisplayMode::SET_TEMP_OFFSET);
                break;
#endif
#ifdef USE_HA_MESSAGE
            case MenuItem::SET_MESSAGE_TIMEOUT:
                CTRL.setMode(DisplayMode::SET_MESSAGE_TIMEOUT);
                break;
#endif
            case MenuItem::BACK:
                CTRL.setMode(DisplayMode::TIME);
                break;
            }
            break;
#ifdef USE_RTC
        case DisplayMode::SET_DATE:
            switch (CTRL.item)
            {
            case MenuItem::YEAR:
                CTRL.setMode(DisplayMode::SET_DATE, MenuItem::MONTH);
                break;
            case MenuItem::MONTH:
                CTRL.setMode(DisplayMode::SET_DATE, MenuItem::DAY);
                break;
            case MenuItem::DAY:
                CTRL.dateTime.persistDate();
                CTRL.setMode(DisplayMode::MENU, MenuItem::SET_DATE);
                break;
            }
            break;
        case DisplayMode::SET_TIME:
            switch (CTRL.item)
            {
            case MenuItem::HOURS:
                CTRL.setMode(DisplayMode::SET_TIME, MenuItem::MINUTES);
                break;
            case MenuItem::MINUTES:
                CTRL.setMode(DisplayMode::SET_TIME, MenuItem::SECONDS);
                break;
            case MenuItem::SECONDS:
                CTRL.dateTime.persistDate();
                CTRL.setMode(DisplayMode::MENU, MenuItem::SET_TIME);
                break;
            }
            break;
#endif
        case DisplayMode::SET_LIGHT:
            switch (CTRL.item)
            {
            case MenuItem::MODE:
                CTRL.setMode(DisplayMode::SET_LIGHT, MenuItem::BRIGHT);
                break;
            case MenuItem::BRIGHT:
                LIGHT.save();
                CTRL.setMode(DisplayMode::MENU, MenuItem::SET_LIGHT);
                break;
            }
            break;
        case DisplayMode::SET_DAYTIME:
            switch (CTRL.item)
            {
            case MenuItem::START:
                CTRL.setMode(DisplayMode::SET_DAYTIME, MenuItem::END);
                break;
            case MenuItem::END:
                CTRL.saveDaytime();
                CTRL.setMode(DisplayMode::MENU, MenuItem::SET_DAYTIME);
                break;
            }
            break;
#ifdef USE_TEMP_SENSOR
        case DisplayMode::SET_TEMP_OFFSET:
            CTRL.saveTempOffset();
            CTRL.setMode(DisplayMode::MENU, MenuItem::SET_TEMP_OFFSET);
            break;
#endif
#ifdef USE_HA_MESSAGE
        case DisplayMode::SET_MESSAGE_TIMEOUT:
            CTRL.saveMessageTimeout();
            CTRL.setMode(DisplayMode::MENU, MenuItem::SET_MESSAGE_TIMEOUT);
            break;
#endif
        case DisplayMode::OFF:
            CTRL.on(true);
            break;
        default:
            CTRL.off(true);
            break;
        }

        CTRL.show();
    }

    void longClick()
    {
        switch (CTRL.mode)
        {
#ifdef USE_RTC
        case DisplayMode::SET_DATE:
            switch (CTRL.item)
            {
            case MenuItem::MONTH:
                CTRL.setMode(DisplayMode::SET_DATE, MenuItem::YEAR);
                break;
            case MenuItem::DAY:
                CTRL.setMode(DisplayMode::SET_DATE, MenuItem::MONTH);
                break;
            }
            break;
        case DisplayMode::SET_TIME:
            switch (CTRL.item)
            {
            case MenuItem::MINUTES:
                CTRL.setMode(DisplayMode::SET_TIME, MenuItem::HOURS);
                break;
            case MenuItem::SECONDS:
                CTRL.setMode(DisplayMode::SET_TIME, MenuItem::MINUTES);
                break;
            }
            break;
#endif
        case DisplayMode::SET_LIGHT:
            switch (CTRL.item)
            {
            case MenuItem::BRIGHT:
                CTRL.setMode(DisplayMode::SET_LIGHT, MenuItem::MODE);
                break;
            }
            break;
        case DisplayMode::SET_DAYTIME:
            switch (CTRL.item)
            {
            case MenuItem::END:
                CTRL.setMode(DisplayMode::SET_DAYTIME, MenuItem::START);
                break;
            }
            break;
        case DisplayMode::OFF:
            CTRL.on(true);
            break;
        default:
            CTRL.setMode(DisplayMode::MENU);
            break;
        }

        CTRL.show();
    }

    void up()
    {
        switch (CTRL.mode)
        {
        case DisplayMode::MENU:
            CTRL.setMode(DisplayMode::MENU, ++CTRL.item);
            break;
#ifdef USE_RTC
        case DisplayMode::SET_DATE:
            switch (CTRL.item)
            {
            case MenuItem::YEAR:
                CTRL.dateTime.incYear();
                break;
            case MenuItem::MONTH:
                CTRL.dateTime.incMonth();
                break;
            case MenuItem::DAY:
                CTRL.dateTime.incDay();
                break;
            }
            break;
        case DisplayMode::SET_TIME:
            switch (CTRL.item)
            {
            case MenuItem::HOURS:
                CTRL.dateTime.incHours();
                break;
            case MenuItem::MINUTES:
                CTRL.dateTime.incMinutes();
                break;
            case MenuItem::SECONDS:
                CTRL.dateTime.incSeconds();
                break;
            }
            break;
#endif
        case DisplayMode::SET_LIGHT:
            switch (CTRL.item)
            {
            case MenuItem::MODE:
                LIGHT.nextMode();
                break;
            case MenuItem::BRIGHT:
                LIGHT.incBrightness();
                break;
            }
            break;
        case DisplayMode::SET_DAYTIME:
            if (CTRL.item == MenuItem::START)
            {
                CTRL.incDaytimeStart();
            }
            else
            {
                CTRL.incDaytimeEnd();
            }
            break;
#ifdef USE_TEMP_SENSOR
        case DisplayMode::SET_TEMP_OFFSET:
            CTRL.incTempOffset();
            break;
#endif
#ifdef USE_HA_MESSAGE
        case DisplayMode::SET_MESSAGE_TIMEOUT:
            CTRL.incMessageTimeout();
            break;
#endif
        default:
            CTRL.nextMode();
            break;
        }

        CTRL.show();
    }

    void down()
    {
        switch (CTRL.mode)
        {
        case DisplayMode::MENU:
            CTRL.setMode(DisplayMode::MENU, --CTRL.item);
            break;
#ifdef USE_RTC
        case DisplayMode::SET_DATE:
            switch (CTRL.item)
            {
            case MenuItem::YEAR:
                CTRL.dateTime.decYear();
                break;
            case MenuItem::MONTH:
                CTRL.dateTime.decMonth();
                break;
            case MenuItem::DAY:
                CTRL.dateTime.decDay();
                break;
            }
            break;
        case DisplayMode::SET_TIME:
            switch (CTRL.item)
            {
            case MenuItem::HOURS:
                CTRL.dateTime.decHours();
                break;
            case MenuItem::MINUTES:
                CTRL.dateTime.decMinutes();
                break;
            case MenuItem::SECONDS:
                CTRL.dateTime.decSeconds();
                break;
            }
            break;
#endif
        case DisplayMode::SET_LIGHT:
            switch (CTRL.item)
            {
            case MenuItem::MODE:
                LIGHT.prevMode();
                break;
            case MenuItem::BRIGHT:
                LIGHT.decBrightness();
                break;
            }
            break;
        case DisplayMode::SET_DAYTIME:
            if (CTRL.item == MenuItem::START)
            {
                CTRL.decDaytimeStart();
            }
            else
            {
                CTRL.decDaytimeEnd();
            }
            break;
#ifdef USE_TEMP_SENSOR
        case DisplayMode::SET_TEMP_OFFSET:
            CTRL.decTempOffset();
            break;
#endif
#ifdef USE_HA_MESSAGE
        case DisplayMode::SET_MESSAGE_TIMEOUT:
            CTRL.decMessageTimeout();
            break;
#endif
        default:
            CTRL.prevMode();
            break;
        }

        CTRL.show();
    }
};

Ui UI;
