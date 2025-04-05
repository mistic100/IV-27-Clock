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
                dateTime.update();
                CTRL.setMode(DisplayMode::SET_DATE, MenuItem::YEAR);
                break;
            case MenuItem::SET_TIME:
                dateTime.update();
                CTRL.setMode(DisplayMode::SET_TIME, MenuItem::HOURS);
                break;
#endif
            case MenuItem::SET_LIGHT:
                CTRL.setMode(DisplayMode::SET_LIGHT, MenuItem::MODE);
                break;
            case MenuItem::SET_DAYTIME:
                CTRL.setMode(DisplayMode::SET_DAYTIME, MenuItem::START);
                break;
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
                CTRL.setMode(DisplayMode::SET_DATE, MenuItem::DONE);
                break;
            case MenuItem::DONE:
                dateTime.persistDate();
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
                CTRL.setMode(DisplayMode::SET_TIME, MenuItem::DONE);
                break;
            case MenuItem::DONE:
                dateTime.persistDate();
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
            case MenuItem::DONE:
                CTRL.setMode(DisplayMode::SET_DATE, MenuItem::DAY);
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
            case MenuItem::DONE:
                CTRL.setMode(DisplayMode::SET_TIME, MenuItem::SECONDS);
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
                dateTime.incYear();
                break;
            case MenuItem::MONTH:
                dateTime.incMonth();
                break;
            case MenuItem::DAY:
                dateTime.incDay();
                break;
            }
            break;
        case DisplayMode::SET_TIME:
            switch (CTRL.item)
            {
            case MenuItem::HOURS:
                dateTime.incHours();
                break;
            case MenuItem::MINUTES:
                dateTime.incMinutes();
                break;
            case MenuItem::SECONDS:
                dateTime.incSeconds();
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
        case DisplayMode::MESSAGE:
            CTRL.setMode(DisplayMode::TIME);
            break;
        default:
            if (isMainDisplayMode(CTRL.mode))
            {
                CTRL.setMode(++CTRL.mode);
            }
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
                dateTime.decYear();
                break;
            case MenuItem::MONTH:
                dateTime.decMonth();
                break;
            case MenuItem::DAY:
                dateTime.decDay();
                break;
            }
            break;
        case DisplayMode::SET_TIME:
            switch (CTRL.item)
            {
            case MenuItem::HOURS:
                dateTime.decHours();
                break;
            case MenuItem::MINUTES:
                dateTime.decMinutes();
                break;
            case MenuItem::SECONDS:
                dateTime.decSeconds();
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
        case DisplayMode::MESSAGE:
            CTRL.setMode(DisplayMode::TIME);
            break;
        default:
            if (isMainDisplayMode(CTRL.mode))
            {
                CTRL.setMode(--CTRL.mode);
            }
            break;
        }

        CTRL.show();
    }
};

Ui UI;
