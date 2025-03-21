#pragma once

#include <Arduino.h>
#include <DS3231.h>
#include <FastLED.h>
#include "Display.hpp"
#include "MutableDateTime.hpp"
#include "model.hpp"

#ifdef HA_MESSAGE
#include <HaSensor.hpp>
#endif
#ifdef BME280_SENSOR
#include <Adafruit_BME280.h>
#endif

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
#ifdef HA_MESSAGE
    HaSensor *haSensor;
#endif
#ifdef BME280_SENSOR
    Adafruit_BME280 *bme280;
#endif

    DisplayMode mode = DisplayMode::TIME;
    MenuItem item = MenuItem::NONE;

    uint8_t messageOverrideTime = 0;

    MutableDateTime dateTime;
    float temp;
    float humi;

public:
#ifdef HA_MESSAGE
    Controller(Display *display, HaSensor *haSensor) : display(display), haSensor(haSensor)
    {
    }
#else
    Controller(Display *display) : display(display)
    {
    }
#endif

    void begin()
    {
        this->ds3231 = new DS3231();

        while (!ds3231Begin())
        {
            log_e("Could not find DS3231");
            delay(1000);
        }

        ds3231->setClockMode(false);

        getDateTime();

#ifdef BME280_SENSOR
        this->bme280 = new Adafruit_BME280();

        while (!bme280->begin(0x76))
        {
            log_e("Could not find BME280");
            delay(1000);
        }

        bme280->setTemperatureCompensation(TEMP_OFFSET);

        getTemp();
#endif

        show();
    }

    void loop()
    {
        EVERY_N_SECONDS(1)
        {
            if (mode != DisplayMode::SET_TIME)
            {
                if (dateTime.tick())
                {
                    if (mode != DisplayMode::SET_DATE)
                    {
                        getDateTime();
                    }
                }
            }

#ifdef HA_MESSAGE
            if (isMainDisplayMode(mode))
            {
                if (messageOverrideTime > 0)
                {
                    messageOverrideTime--;
                }

                if (messageOverrideTime == 0 && !haSensor->getMessage().isEmpty())
                {
                    setMode(DisplayMode::MESSAGE);
                    show();
                }
            }

            if (mode == DisplayMode::MESSAGE && haSensor->getMessage().isEmpty())
            {
                setMode(DisplayMode::TIME);
            }
#endif

            if (mode == DisplayMode::TIME)
            {
                show();
            }
        }

#ifdef BME280_SENSOR
        EVERY_N_SECONDS(10)
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
            case MenuItem::SET_DATE:
                getDateTime();
                setMode(DisplayMode::SET_DATE, MenuItem::YEAR);
                break;
            case MenuItem::SET_TIME:
                getDateTime();
                setMode(DisplayMode::SET_TIME, MenuItem::HOURS);
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
            display->on();
#ifdef HA_MESSAGE
            if (!haSensor->getMessage().isEmpty())
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
        default:
            setMode(DisplayMode::MENU, MenuItem::SET_DATE);
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
        this->item = item;

        if (mode != DisplayMode::MESSAGE)
        {
            messageOverrideTime = 10;
        }
    }

    void show()
    {
        static char str[NUM_GRIDS];
        switch (mode)
        {
#ifdef HA_MESSAGE
        case DisplayMode::MESSAGE:
            display->print(haSensor->getMessage());
            if (haSensor->getMessage().length() <= NUM_GRIDS)
            {
                display->blinkAll();
            }
            break;
#endif
        case DisplayMode::TIME:
        {
            sprintf(str, "  %02d %02d %02d", dateTime.hour(), dateTime.minute(), dateTime.second());
            display->print(str, {5, 8});
            break;
        }
        case DisplayMode::DATE:
            sprintf(str, " %04d-%02d-%02d", dateTime.year(), dateTime.month(), dateTime.day());
            display->print(str);
            break;
#ifdef BME280_SENSOR
        case DisplayMode::TEMP:
        {
            auto tempFirstDecimal = (int)((temp - (long)temp) * 10);
            auto humiFirstDecimal = (int)((humi - (long)humi) * 10);
            sprintf(str, "%2.0f%d%%C   %2.0f%dP", temp, tempFirstDecimal, humi, humiFirstDecimal);
            display->print(str, {2, 10});
            break;
        }
#endif
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

#ifdef BME280_SENSOR
    void getTemp()
    {
        temp = bme280->readTemperature();
        humi = bme280->readHumidity();
    }
#endif

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
