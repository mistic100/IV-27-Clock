#pragma once

#include <Arduino.h>
#include <DS3231.h>
#include <Adafruit_BME280.h>
#include <FastLED.h>
#include "Display.hpp"
#include "MutableDateTime.hpp"
#include "model.hpp"

boolean ds3231Begin()
{
    Wire.beginTransmission(0x68);
    return Wire.endTransmission() == ESP_OK;
}

class Controller
{
private:
    Display *display;

    DS3231 *ds3231;
    Adafruit_BME280 *bme280;

    DisplayMode mode = DisplayMode::TIME;
    MenuItem item = MenuItem::NONE;

    MutableDateTime dateTime;
    float temp;

public:
    Controller(Display *display) : display(display)
    {
    }

    void begin()
    {
        this->ds3231 = new DS3231();
        this->bme280 = new Adafruit_BME280();

        while (!bme280->begin(0x76))
        {
            Serial.println("Could not find BME280");
            delay(1000);
        }

        while (!ds3231Begin())
        {
            Serial.println("Could not find DS3231");
            delay(1000);
        }

        ds3231->setClockMode(false);

        getDateTime();
        getTemp();
    }

    void loop()
    {
        EVERY_N_SECONDS(1)
        {
            if (mode != DisplayMode::SET_DATE && mode != DisplayMode::SET_TIME)
            {
                if (dateTime.tick())
                {
                    getDateTime();
                }
            }
            if (mode == DisplayMode::TIME)
            {
                show();
            }
        }

        EVERY_N_SECONDS(60)
        {
            getTemp();

            if (mode == DisplayMode::DATE)
            {
                show();
            }
        }
    }

    void longClick()
    {
        switch (mode)
        {
        case DisplayMode::OFF:
        case DisplayMode::TIME:
        case DisplayMode::DATE:
            setMode(DisplayMode::MENU, MenuItem::SET_DATE);
            break;
        }

        show();
    }

    void click()
    {
        switch (mode)
        {
        case DisplayMode::MENU:
            switch (item)
            {
            case MenuItem::SET_DATE:
                getDateTime();
                setMode(DisplayMode::SET_DATE, MenuItem::YEAR);
                break;
            case MenuItem::SET_TIME:
                getDateTime();
                setMode(DisplayMode::SET_DATE, MenuItem::HOURS);
                break;
            case MenuItem::BACK:
                setMode(DisplayMode::TIME);
                break;
            }
            break;
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
                setDate();
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
                setTime();
                setMode(DisplayMode::MENU, MenuItem::SET_TIME);
                break;
            }
            break;
        case DisplayMode::OFF:
            setMode(DisplayMode::TIME);
            display->on();
            break;
        default:
            setMode(DisplayMode::OFF);
            display->off();
            break;
        }

        show();
    }

    void down()
    {
        switch (mode)
        {
        case DisplayMode::MENU:
            item++;
            break;
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
        default:
            mode++;
            break;
        }

        show();
    }

    void up()
    {
        switch (mode)
        {
        case DisplayMode::MENU:
            item--;
            break;
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
        default:
            mode--;
            break;
        }

        show();
    }

private:
    void setMode(const DisplayMode mode, const MenuItem item = MenuItem::NONE)
    {
        this->mode = mode;
        this->item = item;
    }

    void show()
    {
        static char str[NUM_GRIDS];
        switch (mode)
        {
        case DisplayMode::TIME:
        {
            auto tempFirstDecimal = (int)((temp - (long)temp) * 10 + 0.5);
            sprintf(str, "%02d%02d%02d %2.0f%doC", dateTime.hour(), dateTime.minute(), dateTime.second(), temp, tempFirstDecimal);
            display->print(str, {2, 4, 6, 9});
            break;
        }
        case DisplayMode::DATE:
            sprintf(str, " %04d-%02d-%02d ", dateTime.year(), dateTime.month(), dateTime.day());
            display->print(str);
            break;
        case DisplayMode::MENU:
            switch (item)
            {
            case MenuItem::SET_DATE:
                display->print("SET DATE");
                break;
            case MenuItem::SET_TIME:
                display->print("SET TIME");
                break;
            case MenuItem::BACK:
                display->print("BACK");
                break;
            }
            break;
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
        }
    }

    void getDateTime()
    {
        dateTime.set(RTClib::now());
    }

    void getTemp()
    {
        temp = bme280->readTemperature() + TEMP_OFFSET;
    }

    void setDate()
    {
        ds3231->setYear(dateTime.year() - 2000);
        ds3231->setMonth(dateTime.month());
        ds3231->setDate(dateTime.day());
    }

    void setTime()
    {
        ds3231->setHour(dateTime.hour());
        ds3231->setMinute(dateTime.minute());
        ds3231->setSecond(dateTime.second());
    }
};
