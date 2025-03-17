#pragma once

#include <DS3231.h>

class MutableDateTime : public DateTime
{
public:
    MutableDateTime()
    {
    }

    void set(const DateTime &dt)
    {
        yOff = dt.year() - 2000;
        m = dt.month();
        d = dt.day();
        hh = dt.hour();
        mm = dt.minute();
        ss = dt.second();
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

    /**
     * @brief Advance one second, and update minute and hour if necessary
     * @return boolean true every hour
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
