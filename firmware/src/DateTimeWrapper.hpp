#pragma once

#include <DS3231.h>
#include "constants.hpp"

#ifdef USE_NTP
#include <time.h>
#endif

#ifdef USE_RTC
boolean ds3231Begin()
{
    Wire.beginTransmission(0x68);
    return Wire.endTransmission() == ESP_OK;
}
#endif

static const char *TAG_DT = "DATETIME";

class DateTimeWrapper : public DateTime
{
private:
#ifdef USE_RTC
    DS3231 *ds3231;
#endif

public:
    DateTimeWrapper()
    {
    }

    void init()
    {
#ifdef USE_NTP
        ESP_LOGI(TAG_DT, "Init NTP");
        configTzTime(TIMEZONE, "pool.ntp.org");
#endif
#ifdef USE_RTC
        ESP_LOGI(TAG_DT, "Init RTC");
        ds3231 = new DS3231();

        while (!ds3231Begin())
        {
            ESP_LOGE(TAG_DT, "Could not find DS3231");
            delay(1000);
        }

        ds3231->setClockMode(false);
#endif

        update();
    }

    void update()
    {
        ESP_LOGI(TAG_DT, "Update");

#ifdef USE_NTP
        static struct tm timeinfo;
        if (!getLocalTime(&timeinfo))
        {
            ESP_LOGE(TAG_DT, "Failed to obtain time");
            return;
        }

        yOff = timeinfo.tm_year - 100;
        m = timeinfo.tm_mon + 1;
        d = timeinfo.tm_mday;
        hh = timeinfo.tm_hour;
        mm = timeinfo.tm_min;
        ss = timeinfo.tm_sec;
#endif
#ifdef USE_RTC
        DateTime dt = RTClib::now();
        yOff = dt.year() - 2000;
        m = dt.month();
        d = dt.day();
        hh = dt.hour();
        mm = dt.minute();
        ss = dt.second();
#endif

        ESP_LOGI(TAG_DT, "Date: %04d-%02d-%02d", year(), m, d);
        ESP_LOGI(TAG_DT, "Time: %02d:%02d:%02d", hh, mm, ss);
    }

#ifdef USE_RTC
    void persistDate()
    {
        ds3231->setYear(yOff);
        ds3231->setMonth(m);
        ds3231->setDate(d);
    }

    void persistTime()
    {
        ds3231->setHour(hh);
        ds3231->setMinute(mm);
        ds3231->setSecond(ss);
    }

    void incYear()
    {
        yOff++;
    }

    void decYear()
    {
        if (yOff > 0)
        {
            yOff--;
        }
    }

    void incMonth()
    {
        m = m == 12 ? 1 : m + 1;
    }

    void decMonth()
    {
        m = m == 1 ? 12 : m - 1;
    }

    void incDay()
    {
        d = d == 31 ? 1 : d + 1;
    }

    void decDay()
    {
        d = d == 1 ? 31 : d - 1;
    }

    void incHours()
    {
        hh = hh == 23 ? 0 : hh + 1;
    }

    void decHours()
    {
        hh = hh == 0 ? 23 : hh - 1;
    }

    void incMinutes()
    {
        mm = mm == 59 ? 0 : mm + 1;
    }

    void decMinutes()
    {
        mm = mm == 0 ? 59 : mm - 1;
    }

    void incSeconds()
    {
        ss = ss == 59 ? 0 : ss + 1;
    }

    void decSeconds()
    {
        ss = ss == 0 ? 59 : ss - 1;
    }
#endif

    /**
     * @brief Advance one second, and update minute and hour if necessary
     * @return true every hour
     */
    boolean tick()
    {
        ss++;
        if (ss == 60)
        {
            ss = 0;
            mm++;
            if (mm == 60)
            {
                mm = 0;
                hh++;
                if (hh == 24)
                {
                    hh = 0;
                }
                return true;
            }
        }
        return false;
    }
};
