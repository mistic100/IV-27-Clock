#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include "Settings.hpp"
#include "Display.hpp"
#include "Light.hpp"
#include "DateTimeWrapper.hpp"
#include "model.hpp"

#ifdef USE_TEMP_SENSOR
#include <Adafruit_BME280.h>
#endif
#ifdef USE_HA
#include "HaSensor.hpp"
#endif

class Controller
{
private:
#ifdef USE_TEMP_SENSOR
    Adafruit_BME280 *bme280;
#endif
#ifdef USE_HA
    HaSensor *haSensor;
#endif

    uint16_t daytimeStart;
    uint16_t daytimeEnd;
    uint8_t messageTimeout;
    float tempOffset;

    float temp;
    float humi;
    bool occupancy = true;
    String message;

    bool forceOff = false;
    bool forceOn = false;
    uint8_t messageTimer = 0;

public:
    DateTimeWrapper dateTime;
    DisplayMode mode = DisplayMode::TIME;
    MenuItem item = MenuItem::NONE;

    void begin()
    {
        daytimeStart = SETTINGS.daytimeStart();
        daytimeEnd = SETTINGS.daytimeEnd();

        dateTime.init();

#ifdef USE_TEMP_SENSOR
        bme280 = new Adafruit_BME280();
        while (!bme280->begin(0x76))
        {
            log_e("Could not find BME280");
            delay(1000);
        }

        tempOffset = SETTINGS.tempOffset();
        bme280->setTemperatureCompensation(tempOffset);
        getTemp();
#endif

#ifdef USE_HA
        haSensor = new HaSensor();
#endif
#ifdef USE_HA_MESSAGE
        messageTimeout = SETTINGS.messageTimeout();
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

#ifdef USE_TEMP_SENSOR
        // update temperature
        EVERY_N_SECONDS(TEMP_UPDATE_INTERVAL_S)
        {
            getTemp();
        }
#endif

        EVERY_N_SECONDS(1)
        {
#ifdef USE_RTC
            // if not setting time, advance the clock
            if (mode != DisplayMode::SET_TIME)
            {
                // every hour, force an update from the RTC
                if (dateTime.tick())
                {
                    // ...unless we are setting the date
                    if (mode != DisplayMode::SET_DATE)
                    {
                        dateTime.update();
                    }
                }
            }
#else
            // every hour, force an update from the NTP
            if (dateTime.tick())
            {
                dateTime.update();
            }
#endif

            // apply auto-off rules
            updateAutoOff();

#ifdef USE_HA_MESSAGE
            // force display the message
            if (isMainDisplayMode(mode) && mode != DisplayMode::MESSAGE)
            {
                if (messageTimer > 0)
                {
                    messageTimer--;
                }

                if (messageTimer == 0 && !message.isEmpty())
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
            if (isMainDisplayMode(mode) && mode != DisplayMode::MESSAGE)
            {
                show();
            }
        }
    }

    // turn off if not already off
    void off(bool force = false)
    {
        if (!forceOn || force)
        {
            if (mode != DisplayMode::OFF)
            {
                DISP.off();
                LIGHT.off();
                setMode(DisplayMode::OFF);
            }

            forceOff = force && !forceOn;
            forceOn = false;
        }
    }

    // turn on if off
    void on(bool force = false)
    {
        if (!forceOff || force)
        {
            if (mode == DisplayMode::OFF)
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
            }

            forceOn = force && !forceOff;
            forceOff = false;
        }
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
            messageTimer = messageTimeout;
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

#ifdef USE_TEMP_SENSOR
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
                DISP.print("SET LIGHT");
                break;
            case MenuItem::SET_DAYTIME:
                DISP.print("SET DAYTIME");
                break;
#ifdef USE_TEMP_SENSOR
            case MenuItem::SET_TEMP_OFFSET:
                DISP.print("TEMP OFFSET", {4});
                break;
#endif
#ifdef USE_HA_MESSAGE
            case MenuItem::SET_MESSAGE_TIMEOUT:
                DISP.print("MESS TIMEOUT", {4});
                break;
#endif
            case MenuItem::BACK:
                DISP.print("BACK");
                break;
            }
            break;

#ifdef USE_RTC
        case DisplayMode::SET_DATE:
            sprintf(str, "  %02d-%02d-%02d", dateTime.year() - 2000, dateTime.month(), dateTime.day());
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
            }
            break;

        case DisplayMode::SET_TIME:
            sprintf(str, " 20%02d.%02d.%02d", dateTime.hour(), dateTime.minute(), dateTime.second());
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
            }
            break;
#endif

        case DisplayMode::SET_LIGHT:
            sprintf(str, "% 6s %5d", lightModeStr(LIGHT.mode), LIGHT.brightness);
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
            break;
        }

#ifdef USE_TEMP_SENSOR
        case DisplayMode::SET_TEMP_OFFSET:
            sprintf(str, "%10.1f*C", tempOffset);
            DISP.print(str);
            DISP.blink({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
            break;
#endif

#ifdef USE_HA_MESSAGE
        case DisplayMode::SET_MESSAGE_TIMEOUT:
            sprintf(str, "%8d sec", messageTimeout);
            DISP.print(str);
            DISP.blink({1, 2, 3, 4, 5, 6, 7, 8, 9});
            break;
#endif
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

#ifdef USE_TEMP_SENSOR
    void incTempOffset()
    {
        tempOffset += 0.1;
    }

    void decTempOffset()
    {
        tempOffset -= 0.1;
    }

    void saveTempOffset()
    {
        SETTINGS.setTempOffset(tempOffset);
        bme280->setTemperatureCompensation(tempOffset);
    }
#endif

#ifdef USE_HA_MESSAGE
    void incMessageTimeout()
    {
        if (messageTimeout < 250)
        {
            messageTimeout += 10;
        }
    }

    void decMessageTimeout()
    {
        if (messageTimeout > 10)
        {
            messageTimeout -= 10;
        }
    }

    void saveMessageTimeout()
    {
        SETTINGS.setMessageTimeout(messageTimeout);
    }
#endif

private:
#ifdef USE_TEMP_SENSOR
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
        if (mode != DisplayMode::OFF && !isMainDisplayMode(mode))
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
