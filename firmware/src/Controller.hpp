#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include "Display.hpp"
#include "MutableDateTime.hpp"
#include "model.hpp"

#ifdef USE_BME280_SENSOR
#include <Adafruit_BME280.h>
#endif
#ifdef USE_HA_MESSAGE
#include "HaSensor.hpp"
#endif

class Controller
{
private:
    Display *display;
#ifdef USE_BME280_SENSOR
    Adafruit_BME280 *bme280;
#endif
#ifdef USE_HA_MESSAGE
    HaSensor *haSensor;
#endif

    DisplayMode mode = DisplayMode::TIME;
    MenuItem item = MenuItem::NONE;

    uint8_t messageOverrideTime = 0;

    MutableDateTime dateTime;
    float temp;
    float humi;
    String message;

public:
    Controller(Display *display) : display(display)
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

#ifdef USE_HA_MESSAGE
        haSensor = new HaSensor();

        getMessage();
#endif
    }

    void loop()
    {

#ifdef USE_HA_MESSAGE
        EVERY_N_SECONDS(HA_UPDATE_INTERVAL_S)
        {
            getMessage();
        }
#endif

        EVERY_N_SECONDS(1)
        {
            if (mode != DisplayMode::SET_TIME)
            {
                if (dateTime.tick())
                {
                    if (mode != DisplayMode::SET_DATE)
                    {
                        dateTime.update();
                    }
                }
            }

#ifdef USE_HA_MESSAGE
            if (isMainDisplayMode(mode))
            {
                if (messageOverrideTime > 0)
                {
                    messageOverrideTime--;
                }

                if (messageOverrideTime == 0 && !message.isEmpty())
                {
                    setMode(DisplayMode::MESSAGE);
                    show();
                }
            }

            if (mode == DisplayMode::MESSAGE && message.isEmpty())
            {
                setMode(DisplayMode::TIME);
            }
#endif

            if (mode == DisplayMode::TIME)
            {
                show();
            }
        }

#ifdef USE_BME280_SENSOR
        EVERY_N_SECONDS(TEMP_UPDATE_INTERVAL_S)
        {

            if (mode == DisplayMode::TEMP)
            {
                getTemp();
                show();
            }
        }
#endif

        EVERY_N_SECONDS(60)
        {
            if (mode == DisplayMode::DATE)
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
        case DisplayMode::OFF:
            display->on();
#ifdef USE_HA_MESSAGE
            if (!message.isEmpty())
            {
                setMode(DisplayMode::MESSAGE);
                break;
            }
#endif
            setMode(DisplayMode::TIME);
            break;
        default:
            display->off();
            setMode(DisplayMode::OFF);
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

private:
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

        if (mode != DisplayMode::MESSAGE && mode != DisplayMode::OFF)
        {
            messageOverrideTime = 10;
        }
    }

    void show()
    {
        static char str[NUM_GRIDS];

        switch (mode)
        {
#ifdef USE_HA_MESSAGE
        case DisplayMode::MESSAGE:
            display->print(message);
            if (message.length() <= NUM_GRIDS)
            {
                display->blinkAll();
            }
            break;
#endif
        case DisplayMode::TIME:
            sprintf(str, "  %02d %02d %02d", dateTime.hour(), dateTime.minute(), dateTime.second());
            display->print(str, {5, 8});
            break;
        case DisplayMode::DATE:
            sprintf(str, " %04d-%02d-%02d", dateTime.year(), dateTime.month(), dateTime.day());
            display->print(str);
            break;
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
};
