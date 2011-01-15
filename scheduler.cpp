/* Copyright (C) 2010-2011 Circuits At Home, LTD. All rights reserved.

This software may be distributed and modified under the terms of the GNU
General Public License version 2 (GPL2) as published by the Free Software
Foundation and appearing in the file GPL2.TXT included in the packaging of
this file. Please note that GPL2 Section 2[b] requires that all works based
on this software must also be made publicly available under the terms of
the GPL2 ("Copyleft").

Contact information
-------------------

Circuits At Home, LTD
Web      :  http://www.circuitsathome.com
e-mail   :  support@circuitsathome.com
*/
#include "scheduler.h"


uint8_t DaysPerMonth(time_t t)
{
    uint8_t mon = month(t);
    switch (mon)
    {
    case 4:
    case 6:
    case 9:
    case 11:
        return 30;
    case 2:
        return (IS_LEAP_YEAR(year(t))) ? 29 : 28;
    default:
        return 31;
    }
}

bool SchedulerTask::Set(TaskCallback pfunc, uint8_t pt, time_t time, uint8_t num) 
{ 
    timeToFire = time; 
    taskAttribs.bmTaskSet = 1; 
    taskAttribs.bmEnabled = 0; 
    taskAttribs.bmPeriodType = pt; 
    pCallback = pfunc; 
    countDown = num; 
    return true; 
}

bool SchedulerTask::Run(time_t time)
{
    if (!IsSet() || !IsEnabled())
        return true;
        
    if (time >= timeToFire)
    {
        pCallback();
        
        if (countDown != DO_IT_FOREVER)
            countDown --;
            
        UpdateTime();
    }
    return true;
}

bool SchedulerTask::UpdateTime()
{
    if (!countDown)
        return true;
    switch (taskAttribs.bmPeriodType)
    {
    case enHourly:
        timeToFire += SECS_PER_HOUR;
        break;
    case enDaily:
        timeToFire += SECS_PER_DAY;
        break;
    case enWeekly:
        timeToFire += SECS_PER_WEEK;
        break;
    case enMonthly:
        timeToFire += DaysPerMonth(timeToFire) * SECS_PER_DAY;
        break;
    case enYearly:
        timeToFire += DAYS_PER_YEAR(year(timeToFire));
        break;
    }
    return true;
}
