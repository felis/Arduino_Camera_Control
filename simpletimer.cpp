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
#include "simpletimer.h"

bool SimpleTimer::Set(TimerCallback task, uint32_t timeout, bool once)
{
    if (!task)
        return false;
    
    timeOut     = timeout;
    pCallback   = task;
    timerAttribs.bmOneTime   = (once) ? 1 : 0;
    timerAttribs.bmTimerSet  = 1;
    timerAttribs.bmEnabled   = 0;
    return true;
}

void SimpleTimer::Reset()
{
    timerAttribs.bmTimerSet  = 0;
    timerAttribs.bmEnabled   = 0;
    timerAttribs.bmOneTime   = 0;
    timerAttribs.bmSign      = 0;
    
    pCallback = NULL;
}

bool SimpleTimer::Enable()
{
    if (!pCallback || !timeOut)
        return false;
    
    uint32_t time = millis();
    
    timeToFire = time + timeOut;
    
    timerAttribs.bmEnabled   = 1;
    timerAttribs.bmSign = time & MSB_MASK;
    return true;
}

void SimpleTimer::Disable()
{
    timerAttribs.bmEnabled   = 0;
}

void SimpleTimer::Run()
{
    if (timerAttribs.bmEnabled == 0)
        return;
        
    if (TimeoutEllapsed())
    {
        if (pCallback)
            pCallback();
            
        if (timerAttribs.bmOneTime == 1)
            timerAttribs.bmEnabled = 0;
    }
}

bool SimpleTimer::TimeoutEllapsed()
{
    bool      ret   = false;
    uint32_t  time  = millis();
    
    if (time >= timeToFire || (time < timeToFire && timerAttribs.bmSign == 1) )
    {
        timeToFire = time + timeOut;
        ret = true;
    }
    timerAttribs.bmSign = (time & MSB_MASK) ? 1 : 0;
    return ret;
}
