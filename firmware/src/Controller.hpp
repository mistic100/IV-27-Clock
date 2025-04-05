#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include "Display.hpp"
#include "Light.hpp"
#include "DateTimeWrapper.hpp"
#include "model.hpp"

#ifdef USE_BME280_SENSOR
#include <Adafruit_BME280.h>
#endif
#ifdef USE_HA
#include "HaSensor.hpp"
#endif

class Controller
{
private:
    Display *display;
    Light *light;
#ifdef USE_BME280_SENSOR
    Adafruit_BME280 *bme280;
#endif
#ifdef USE_HA
    HaSensor *haSensor;
#endif

    DisplayMode mode = DisplayMode::TIME;
    MenuItem item = MenuItem::NONE;

    DateTimeWrapper dateTime;
    float temp;
    float humi;
    bool occupancy = true;
    String message;

    bool forceOff = false;
    bool forceOn = false;
    uint8_t messageTimeout = 0;

public:
    Controller(Display *display, Light *light) : display(display), light(light)
    {
    }

    void begin()
    {
        dateTime.init();

#ifdef USE_BME280_SENSOR
        bme280 = new Adafruit_BME280();

        while (!bme280->begin(0x76))
        {
            log_e("Could not find BME280");
            delay(1000);
        }

        bme280->setTemperatureCompensation(TEMP_OFFSET);
        getTemp();
#endif

#ifdef USE_HA
        haSensor = new HaSensor();
#endif
#ifdef USE_HA_MESSAGE
        getMessage();
#endif
#ifdef USE_HA_OCCUPANCY
        getOccupancy();
#endif
    }

    void loop()
    {
#ifdef USE_HA
        // update Home Assistant data
        EVERY_N_SECONDS(HA_UPDATE_INTERVAL_S)
        {
#ifdef USE_HA_MESSAGE
            getMessage();
#endif
#ifdef USE_HA_OCCUPANCY
            getOccupancy();
#endif
        }
#endif

#ifdef USE_BME280_SENSOR
        // update temperature
        EVERY_N_SECONDS(TEMP_UPDATE_INTERVAL_S)
        {
            getTemp();
        }
#endif

        EVERY_N_SECONDS(1)
        {
            // if not setting time, advance the clock
            if (mode != DisplayMode::SET_TIME)
            {
                // every hour, force an update from the RTC/NTP
                if (dateTime.tick())
                {
                    // ...unless we are setting the date
                    if (mode != DisplayMode::SET_DATE)
                    {
                        dateTime.update();
                    }
                }
            }

            // apply auto-off rules
            updateAutoOff();

#ifdef USE_HA_MESSAGE
            // force display the message
            if (isMainDisplayMode(mode))
            {
                if (messageTimeout > 0)
                {
                    messageTimeout--;
                }

                if (messageTimeout == 0 && !message.isEmpty())
                {
                    setMode(DisplayMode::MESSAGE);
                    show();
                }
            }

            // return to time if there is not more message
            if (mode == DisplayMode::MESSAGE && message.isEmpty())
            {
                setMode(DisplayMode::TIME);
            }
#endif

            // update display
            if (isMainDisplayMode(mode))
            {
                show();
            }
        }
    }

    void click()
    {
        switch (mode)
        {
        case DisplayMode::MENU:
            switch (item)
            {
#ifdef USE_RTC
            case MenuItem::SET_DATE:
                dateTime.update();
                setMode(DisplayMode::SET_DATE, MenuItem::YEAR);
                break;
            case MenuItem::SET_TIME:
                dateTime.update();
                setMode(DisplayMode::SET_TIME, MenuItem::HOURS);
                break;
#endif
            case MenuItem::SET_LIGHT:
                setMode(DisplayMode::SET_LIGHT, MenuItem::MODE);
                break;
            case MenuItem::BACK:
                setMode(DisplayMode::TIME);
                break;
            }
            break;
#ifdef USE_RTC
        case DisplayMode::SET_DATE:
            switch (item)
            {
            case MenuItem::YEAR:
                setMode(DisplayMode::SET_DATE, MenuItem::MONTH);
                break;
            case MenuItem::MONTH:
                setMode(DisplayMode::SET_DATE, MenuItem::DAY);
                break;
            case MenuItem::DAY:
                setMode(DisplayMode::SET_DATE, MenuItem::DONE);
                break;
            case MenuItem::DONE:
                dateTime.persistDate();
                setMode(DisplayMode::MENU, MenuItem::SET_DATE);
                break;
            }
            break;
        case DisplayMode::SET_TIME:
            switch (item)
            {
            case MenuItem::HOURS:
                setMode(DisplayMode::SET_TIME, MenuItem::MINUTES);
                break;
            case MenuItem::MINUTES:
                setMode(DisplayMode::SET_TIME, MenuItem::SECONDS);
                break;
            case MenuItem::SECONDS:
                setMode(DisplayMode::SET_TIME, MenuItem::DONE);
                break;
            case MenuItem::DONE:
                dateTime.persistTime();
                setMode(DisplayMode::MENU, MenuItem::SET_TIME);
                break;
            }
            break;
#endif
        case DisplayMode::SET_LIGHT:
            switch (item)
            {
            case MenuItem::MODE:
                setMode(DisplayMode::SET_LIGHT, MenuItem::BRIGHT);
                break;
            case MenuItem::BRIGHT:
                setMode(DisplayMode::SET_LIGHT, MenuItem::DONE);
                break;
            case MenuItem::DONE:
                setMode(DisplayMode::MENU, MenuItem::SET_LIGHT);
                break;
            }
            break;
        case DisplayMode::OFF:
            on(true);
            break;
        default:
            off(true);
            break;
        }

        show();
    }

    void longClick()
    {
        switch (mode)
        {
#ifdef USE_RTC
        case DisplayMode::SET_DATE:
            switch (item)
            {
            case MenuItem::MONTH:
                setMode(DisplayMode::SET_DATE, MenuItem::YEAR);
                break;
            case MenuItem::DAY:
                setMode(DisplayMode::SET_DATE, MenuItem::MONTH);
                break;
            case MenuItem::DONE:
                setMode(DisplayMode::SET_DATE, MenuItem::DAY);
                break;
            }
            break;
        case DisplayMode::SET_TIME:
            switch (item)
            {
            case MenuItem::MINUTES:
                setMode(DisplayMode::SET_TIME, MenuItem::HOURS);
                break;
            case MenuItem::SECONDS:
                setMode(DisplayMode::SET_TIME, MenuItem::MINUTES);
                break;
            case MenuItem::DONE:
                setMode(DisplayMode::SET_TIME, MenuItem::SECONDS);
                break;
            }
            break;
#endif
        case DisplayMode::SET_LIGHT:
            switch (item)
            {
            case MenuItem::BRIGHT:
                setMode(DisplayMode::SET_LIGHT, MenuItem::MODE);
                break;
            case MenuItem::DONE:
                setMode(DisplayMode::SET_LIGHT, MenuItem::BRIGHT);
                break;
            }
        case DisplayMode::OFF:
            on(true);
            break;
        default:
            setMode(DisplayMode::MENU);
            break;
        }

        show();
    }

    void down()
    {
        switch (mode)
        {
        case DisplayMode::MENU:
            setMode(DisplayMode::MENU, ++item);
            break;
#ifdef USE_RTC
        case DisplayMode::SET_DATE:
            switch (item)
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
            switch (item)
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
            switch (item)
            {
            case MenuItem::MODE:
                light->nextMode();
                break;
            case MenuItem::BRIGHT:
                light->incBrightness();
                break;
            }
            break;
        case DisplayMode::MESSAGE:
            setMode(DisplayMode::TIME);
            break;
        default:
            if (isMainDisplayMode(mode))
            {
                setMode(++mode);
            }
            break;
        }

        show();
    }

    void up()
    {
        switch (mode)
        {
        case DisplayMode::MENU:
            setMode(DisplayMode::MENU, --item);
            break;
#ifdef USE_RTC
        case DisplayMode::SET_DATE:
            switch (item)
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
            switch (item)
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
            switch (item)
            {
            case MenuItem::MODE:
                light->prevMode();
                break;
            case MenuItem::BRIGHT:
                light->decBrightness();
                break;
            }
            break;
        case DisplayMode::MESSAGE:
            setMode(DisplayMode::TIME);
            break;
        default:
            if (isMainDisplayMode(mode))
            {
                setMode(--mode);
            }
            break;
        }

        show();
    }

    // turn off if not already off
    void off(bool force = false)
    {
        if (mode != DisplayMode::OFF && (!forceOn || force))
        {
            display->off();
            light->off();
            setMode(DisplayMode::OFF);
        }

        forceOff = force;
        forceOn = false;
    }

    // turn on if off
    void on(bool force = false)
    {
        if (mode == DisplayMode::OFF && (!forceOff || force))
        {
            display->on();
            light->on();

#ifdef USE_HA_MESSAGE
            if (!message.isEmpty())
            {
                setMode(DisplayMode::MESSAGE);
            }
            else
            {
                setMode(DisplayMode::TIME);
            }
#else
            setMode(DisplayMode::TIME);
#endif
        }

        forceOn = force;
        forceOff = false;
    }

private:
    // change the display mode
    void setMode(const DisplayMode mode, const MenuItem item = MenuItem::NONE)
    {
        this->mode = mode;

        if (mode == DisplayMode::MENU)
        {
            this->item = item != MenuItem::NONE ? item : MenuItem::FIRST_ITEM;
        }
        else
        {
            this->item = item;
        }

#ifdef USE_HA_MESSAGE
        // reset the message timeout
        if (mode != DisplayMode::MESSAGE && mode != DisplayMode::OFF)
        {
            messageTimeout = 10;
        }
#endif
    }

    void show()
    {
        static char str[NUM_GRIDS];

        switch (mode)
        {
        case DisplayMode::TIME:
            sprintf(str, "  %02d %02d %02d", dateTime.hour(), dateTime.minute(), dateTime.second());
            display->print(str, {5, 8});
            break;
        case DisplayMode::DATE:
            sprintf(str, " %04d-%02d-%02d", dateTime.year(), dateTime.month(), dateTime.day());
            display->print(str);
            break;
#ifdef USE_HA_MESSAGE
        case DisplayMode::MESSAGE:
            display->print(message);
            if (message.length() <= NUM_GRIDS)
            {
                display->blinkAll();
            }
            break;
#endif
#ifdef USE_BME280_SENSOR
        case DisplayMode::TEMP:
        {
            auto tempFirstDecimal = (int)((temp - (long)temp) * 10);
            auto humiFirstDecimal = (int)((humi - (long)humi) * 10);
            sprintf(str, "%2.0f%d*C   %2.0f%dP", temp, tempFirstDecimal, humi, humiFirstDecimal);
            display->print(str, {2, 10});
            break;
        }
#endif
        case DisplayMode::MENU:
            switch (item)
            {
#ifdef USE_RTC
            case MenuItem::SET_DATE:
                display->print("SET DATE");
                break;
            case MenuItem::SET_TIME:
                display->print("SET TIME");
                break;
#endif
            case MenuItem::SET_LIGHT:
                display->print("LIGHT");
                break;
            case MenuItem::BACK:
                display->print("BACK");
                break;
            }
            break;
#ifdef USE_RTC
        case DisplayMode::SET_DATE:
            sprintf(str, "%02d-%02d-%02d  OK", dateTime.year() - 2000, dateTime.month(), dateTime.day());
            display->print(str);
            switch (item)
            {
            case MenuItem::YEAR:
                display->blink({1, 2});
                break;
            case MenuItem::MONTH:
                display->blink({4, 5});
                break;
            case MenuItem::DAY:
                display->blink({7, 8});
                break;
            case MenuItem::DONE:
                display->blink({11, 12});
                break;
            }
            break;
        case DisplayMode::SET_TIME:
            sprintf(str, "%02d.%02d.%02d  OK", dateTime.hour(), dateTime.minute(), dateTime.second());
            display->print(str);
            switch (item)
            {
            case MenuItem::HOURS:
                display->blink({1, 2});
                break;
            case MenuItem::MINUTES:
                display->blink({4, 5});
                break;
            case MenuItem::SECONDS:
                display->blink({7, 8});
                break;
            case MenuItem::DONE:
                display->blink({11, 12});
                break;
            }
            break;
#endif
        case DisplayMode::SET_LIGHT:
            sprintf(str, "%s %02d OK", lightModeStr(light->mode), light->brightness);
            display->print(str);
            switch (item)
            {
            case MenuItem::MODE:
                display->blink({1, 2, 3, 4, 5, 6});
                break;
            case MenuItem::BRIGHT:
                display->blink({8, 9});
                break;
            case MenuItem::DONE:
                display->blink({11, 12});
                break;
            }
            break;
        }
    }

#ifdef USE_BME280_SENSOR
    void getTemp()
    {
        temp = bme280->readTemperature();
        humi = bme280->readHumidity();
    }
#endif

#ifdef USE_HA_MESSAGE
    void getMessage()
    {
        message = haSensor->getMessage();
    }
#endif

#ifdef USE_HA_OCCUPANCY
    void getOccupancy()
    {
        occupancy = haSensor->getOccupancy();
    }
#endif

#if defined(USE_HA_OCCUPANCY) || defined(USE_AUTO_OFF)
    // if in menus : do nothing
    // if occupancy=false : turn off
    // if not daytime : turn off
    // else : turn on
    void updateAutoOff()
    {
        if (mode != DisplayMode::OFF && mode != DisplayMode::MESSAGE && !isMainDisplayMode(mode))
        {
            return;
        }

#ifdef USE_HA_OCCUPANCY
        if (!occupancy)
        {
            off();
            return;
        }
#endif
#ifdef USE_AUTO_OFF
        if (!isDaytime(dateTime.hour(), dateTime.minute()))
        {
            off();
            return;
        }
#endif
        on();
    }
#endif
};
