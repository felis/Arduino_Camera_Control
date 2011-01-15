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
#ifndef __SIMPLETIMER_H__
#define __SIMPLETIMER_H__

#include <inttypes.h>
#include <avr/pgmspace.h>
#include "WProgram.h"

#define MSB_MASK	0x80000000

typedef void (*TimerCallback)();

class SimpleTimer
{
      struct TimerAttributes
      {
          uint8_t    bmAllocated : 1;    // 1 if allocated for some routine
          uint8_t    bmTimerSet  : 1;    // 1 if all data for the timer is set
          uint8_t    bmEnabled   : 1;    // 1 if enabled and running
          uint8_t    bmOneTime   : 1;    // 1 if the task should be executed onece
          uint8_t    bmSign      : 1;    // necessary to handle millis() rollovers
      };
      
      TimerAttributes  timerAttribs;
      uint32_t         timeOut;
      uint32_t         timeToFire;
      TimerCallback    pCallback;
      
      bool TimeoutEllapsed();
      
public:
      SimpleTimer() : timeOut(0), timeToFire(0), pCallback(NULL) { *((uint8_t*)&timerAttribs) = 0; };
      
      void SetAllocated(bool yes) { timerAttribs.bmAllocated = (yes) ? 1 : 0; };
      bool IsAllocated() { return timerAttribs.bmAllocated == 1; };
      bool IsEnabled()   { return timerAttribs.bmEnabled == 1; };
      
      bool Set(TimerCallback task, uint32_t timeout, bool once = false);
      void Reset();
      bool Enable();
      void Disable();
      void Run();

	uint32_t TimeLeft() 
	{ 
		if (timerAttribs.bmEnabled != 1)
			return 0;

		int32_t time_left = timeToFire - millis();
		return (time_left > 0) ? time_left : 0; 
	};
};

#define INVALID_TIMER_ID      0

template <class TIMER_TYPE, const uint8_t POOL_SIZE>
class TimerPool
{
    TIMER_TYPE    thePool[POOL_SIZE];

public:
    TimerPool() {};
    
    uint8_t SetTimer(TimerCallback pfunc, uint32_t msec, bool once);
    bool EnableTimer(uint8_t timer_id)   { if (!timer_id) return false; thePool[timer_id-1].Enable(); return true; };
    bool DisableTimer(uint8_t timer_id) { if (!timer_id) return false; thePool[timer_id-1].Disable(); return true; };
    bool ReleaseTimer(uint8_t &timer_id){ if (!timer_id) return false; thePool[timer_id-1].SetAllocated(false); thePool[timer_id-1].Reset(); timer_id = 0; return true; };
    void Run();    
};

template <class TIMER_TYPE, const uint8_t POOL_SIZE>
uint8_t TimerPool<TIMER_TYPE, POOL_SIZE>::SetTimer(TimerCallback pfunc, uint32_t msec, bool once)
{
    for (uint8_t i=0; i<POOL_SIZE; i++)
    {
        if (!thePool[i].IsAllocated())
        {
            thePool[i].SetAllocated(true);
            thePool[i].SetTimer(pfunc, msec, once);
            return i + 1;
        }
    }    
    return INVALID_TIMER_ID;
}

template <class TIMER_TYPE, const uint8_t POOL_SIZE>
void TimerPool<TIMER_TYPE, POOL_SIZE>::Run()
{
    for (uint8_t i=0; i<POOL_SIZE; i++)
        thePool[i].Run();
}

#endif // __SIMPLETIMER_H__