#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include "Settings.hpp"
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
#ifdef USE_BME280_SENSOR
    Adafruit_BME280 *bme280;
#endif
#ifdef USE_HA
    HaSensor *haSensor;
#endif

    uint16_t daytimeStart;
    uint16_t daytimeEnd;

    DateTimeWrapper dateTime;
    float temp;
    float humi;
    bool occupancy = true;
    String message;

    bool forceOff = false;
    bool forceOn = false;
    uint8_t messageTimeout = 0;

public:
    DisplayMode mode = DisplayMode::TIME;
    MenuItem item = MenuItem::NONE;

    void begin()
    {
        daytimeStart = SETTINGS.daytimeStart();
        daytimeEnd = SETTINGS.daytimeEnd();

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

    // turn off if not already off
    void off(bool force = false)
    {
        if (mode != DisplayMode::OFF && (!forceOn || force))
        {
            DISP.off();
            LIGHT.off();
            setMode(DisplayMode::OFF);

            forceOn = false;
        }

        forceOff = force;
    }

    // turn on if off
    void on(bool force = false)
    {
        if (mode == DisplayMode::OFF && (!forceOff || force))
        {
            DISP.on();
            LIGHT.on();

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

            forceOff = false;
        }

        forceOn = force;
    }

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
            DISP.print(str, {5, 8});
            break;

        case DisplayMode::DATE:
            sprintf(str, " %04d-%02d-%02d", dateTime.year(), dateTime.month(), dateTime.day());
            DISP.print(str);
            break;

#ifdef USE_HA_MESSAGE
        case DisplayMode::MESSAGE:
            DISP.print(message);
            if (message.length() <= NUM_GRIDS)
            {
                DISP.blinkAll();
            }
            break;
#endif

#ifdef USE_BME280_SENSOR
        case DisplayMode::TEMP:
        {
            auto tempFirstDecimal = (int)((temp - (long)temp) * 10);
            auto humiFirstDecimal = (int)((humi - (long)humi) * 10);
            sprintf(str, "%2.0f%d*C   %2.0f%dP", temp, tempFirstDecimal, humi, humiFirstDecimal);
            DISP.print(str, {2, 10});
            break;
        }
#endif

        case DisplayMode::MENU:
            switch (item)
            {
#ifdef USE_RTC
            case MenuItem::SET_DATE:
                DISP.print("SET DATE");
                break;
            case MenuItem::SET_TIME:
                DISP.print("SET TIME");
                break;
#endif
            case MenuItem::SET_LIGHT:
                DISP.print("LIGHT");
                break;
            case MenuItem::SET_DAYTIME:
                DISP.print("DAYTIME");
                break;
            case MenuItem::BACK:
                DISP.print("BACK");
                break;
            }
            break;

#ifdef USE_RTC
        case DisplayMode::SET_DATE:
            sprintf(str, "%02d-%02d-%02d  OK", dateTime.year() - 2000, dateTime.month(), dateTime.day());
            DISP.print(str);
            switch (item)
            {
            case MenuItem::YEAR:
                DISP.blink({1, 2});
                break;
            case MenuItem::MONTH:
                DISP.blink({4, 5});
                break;
            case MenuItem::DAY:
                DISP.blink({7, 8});
                break;
            case MenuItem::DONE:
                DISP.blink({11, 12});
                break;
            }
            break;

        case DisplayMode::SET_TIME:
            sprintf(str, "%02d.%02d.%02d  OK", dateTime.hour(), dateTime.minute(), dateTime.second());
            DISP.print(str);
            switch (item)
            {
            case MenuItem::HOURS:
                DISP.blink({1, 2});
                break;
            case MenuItem::MINUTES:
                DISP.blink({4, 5});
                break;
            case MenuItem::SECONDS:
                DISP.blink({7, 8});
                break;
            case MenuItem::DONE:
                DISP.blink({11, 12});
                break;
            }
            break;
#endif

        case DisplayMode::SET_LIGHT:
            sprintf(str, "% 6s    %02d", lightModeStr(LIGHT.mode), LIGHT.brightness);
            DISP.print(str);
            switch (item)
            {
            case MenuItem::MODE:
                DISP.blink({1, 2, 3, 4, 5, 6});
                break;
            case MenuItem::BRIGHT:
                DISP.blink({11, 12});
                break;
                break;
            }
            break;

        case DisplayMode::SET_DAYTIME:
        {
            if (item == MenuItem::START)
            {
                auto h = daytimeStart / 60;
                auto m = daytimeStart - h * 60;
                sprintf(str, "START %02d.%02d", h, m);
            }
            else
            {
                auto h = daytimeEnd / 60;
                auto m = daytimeEnd - h * 60;
                sprintf(str, "  END %02d.%02d", h, m);
            }
            DISP.print(str);
            DISP.blink({7, 8, 9, 10, 11});
        }
        }
    }

    void incDaytimeStart()
    {
        if (daytimeStart < 23 * 60 + 30)
        {
            daytimeStart += 30;
        }
    }

    void incDaytimeEnd()
    {
        if (daytimeEnd < 23 * 60 + 30)
        {
            daytimeEnd += 30;
        }
    }

    void decDaytimeStart()
    {
        if (daytimeStart > 0)
        {
            daytimeStart -= 30;
        }
    }

    void decDaytimeEnd()
    {
        if (daytimeEnd > 0)
        {
            daytimeEnd -= 30;
        }
    }

    void saveDaytime()
    {
        SETTINGS.setDaytime(daytimeStart, daytimeEnd);
    }

private:
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

        uint16_t timeInMinutes = dateTime.hour() * 60 + dateTime.minute();
        if (timeInMinutes < daytimeStart || timeInMinutes >= daytimeEnd)
        {
            off();
            return;
        }

        on();
    }
};

Controller CTRL;
