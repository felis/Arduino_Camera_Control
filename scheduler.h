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
#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include <inttypes.h>
#include <avr/pgmspace.h>
#include <../Time/Time.h>
#include "WProgram.h"

#define DO_IT_FOREVER			0xFF
#define INVALID_TASK_ID			0xFF
#define IS_LEAP_YEAR(Y)     ( ((Y)>0) && !((Y)%4) && ( ((Y)%100) || !((Y)%400) ) )
#define DAYS_PER_YEAR(Y)    ( ((IS_LEAP_YEAR((Y))) ? 366 : 365) )

typedef void (*TaskCallback)();

uint8_t DaysPerMonth(time_t t);

class SimpleClock
{
    time_t   unixTime;
    
public:
    SimpleClock() : unixTime(0) {};
    SimpleClock(time_t time) : unixTime(time) {};
    bool SetTime(time_t time) { if (!time) return false; unixTime = time; return true; };
    time_t GetTime() { return unixTime; };
    void IncrementTime() { ++unixTime; };
};

class SchedulerTask
{
public:
    enum PeriodTypes { enOnce, enHourly, enDaily, enWeekly, enMonthly, enYearly };
    
    struct TaskAttributes
    {
        uint8_t    bmAllocated    : 1;
        uint8_t    bmTaskSet      : 1;
        uint8_t    bmEnabled      : 1;
        uint8_t    bmPeriodType   : 3;
    };
   
    TaskAttributes        taskAttribs;
    
    uint8_t        countDown;
    time_t         timeToFire;
    TaskCallback   pCallback;
    
    bool IsAllocated() { return (taskAttribs.bmAllocated == 1); };
    bool IsEnabled() { return (taskAttribs.bmEnabled == 1); };
    bool IsSet() { return (taskAttribs.bmTaskSet == 1); };

protected:
    virtual bool UpdateTime();    // updates time for the next extcution time

public:    
    SchedulerTask() :
                   countDown(0),
                   timeToFire(0),
                   pCallback(NULL)
    { *((uint8_t*)&taskAttribs) = 0; };
    
    void SetAllocated(bool bAllocated) { taskAttribs.bmAllocated = (bAllocated) ? 1 : 0; };
    void SetEnabled(bool bEnabled) { taskAttribs.bmEnabled = (bEnabled) ? 1 : 0; };
    bool Set(TaskCallback pfunc, uint8_t pt, time_t time, uint8_t num = DO_IT_FOREVER);
    bool Reset() { timeToFire = 0; *((uint8_t*)&taskAttribs) = 0; pCallback = NULL; countDown = 0; };
    virtual bool Run(time_t time);
};

template <class TASK_TYPE, const uint8_t MAX_TASKS>
class Scheduler : public SimpleClock
{
    TASK_TYPE taskList[MAX_TASKS];

    uint8_t AllocateTask();    
    void DeallocateTask(uint8_t task_id);
    uint8_t CreateTask(const TaskCallback task, uint8_t type, time_t time, uint8_t num);
    
public:
    uint8_t DailyTask(TaskCallback task, uint8_t hours = 0, uint8_t mins = 0, uint8_t secs = 0, uint8_t num = DO_IT_FOREVER);
    uint8_t WeeklyTask(TaskCallback task, uint8_t dow = 1, uint8_t hours = 0, uint8_t mins = 0, uint8_t secs = 0, uint8_t num = DO_IT_FOREVER);
    uint8_t MonthlyTask(TaskCallback task, uint8_t day = 0, uint8_t hours = 0, uint8_t mins = 0, uint8_t secs = 0, uint8_t num = DO_IT_FOREVER);
    uint8_t YearlyTask(TaskCallback task, uint8_t mon, uint8_t day = 1, uint8_t hours = 0, uint8_t mins = 0, uint8_t secs = 0, uint8_t num = DO_IT_FOREVER);
    void KillTask(uint8_t task_id) { DeallocateTask(task_id); };
    bool Run();
};

template <class TASK_TYPE, const uint8_t MAX_TASKS>
uint8_t Scheduler<TASK_TYPE, MAX_TASKS>::AllocateTask()
{
    for (uint8_t i=0; i<MAX_TASKS; i++)
        if (!taskList[i].IsAllocated())
        {
            taskList[i].SetAllocated(true);
            return i;
        }
    return INVALID_TASK_ID;
}

template <class TASK_TYPE, const uint8_t MAX_TASKS>
void Scheduler<TASK_TYPE, MAX_TASKS>::DeallocateTask(uint8_t id)
{
    taskList[id].Reset();
    taskList[id].SetAllocated(false);
}

template <class TASK_TYPE, const uint8_t MAX_TASKS>
uint8_t Scheduler<TASK_TYPE, MAX_TASKS>::CreateTask(const TaskCallback task, uint8_t type, time_t time, uint8_t num)
{
    uint8_t id = AllocateTask();
    
    if (id == INVALID_TASK_ID)
        return INVALID_TASK_ID;
        
    if (taskList[id].Set(task, type, time, num))
    {
        taskList[id].SetEnabled(true);
        return id;
    }
    else
        DeallocateTask(id);
    
    return INVALID_TASK_ID;
}

template <class TASK_TYPE, const uint8_t MAX_TASKS>
uint8_t Scheduler<TASK_TYPE, MAX_TASKS>::DailyTask(TaskCallback task, uint8_t hours, uint8_t mins, uint8_t secs, uint8_t num)
{
    if (hours > 23 || mins > 59 || secs > 59)
        return INVALID_TASK_ID;
    
    time_t  time = GetTime();
    
    tmElements_t    tm;
    tm.Hour   = hours;
    tm.Minute = mins;
    tm.Second = secs;
    tm.Day    = day(time);
    tm.Month  = month(time);
    tm.Year   = year(time);
    time_t  t = makeTime(tm);
    
    if (t < time)
        t += SECS_PER_DAY;
        
    return CreateTask(task, SchedulerTask::enDaily, t, num);
}

template <class TASK_TYPE, const uint8_t MAX_TASKS>
uint8_t Scheduler<TASK_TYPE, MAX_TASKS>::WeeklyTask(TaskCallback task, uint8_t dow, uint8_t hours, uint8_t mins, uint8_t secs, uint8_t num)
{
    if (hours > 23 || mins > 59 || secs > 59 || dow < 1 || dow > 7)
        return INVALID_TASK_ID;
    
    time_t  time = GetTime();
    
    tmElements_t    tm;
    tm.Hour   = hours;
    tm.Minute = mins;
    tm.Second = secs;
    tm.Day    = day(time);
    tm.Month  = month(time);
    tm.Year   = year(time);
    time_t  t = makeTime(tm);
    
    uint8_t  wd = weekday(time);
    uint8_t  days_to_add = (wd < dow) ? dow - wd : 7 - wd + dow;
    
    if (days_to_add)
        t += days_to_add * SECS_PER_DAY;
        
    if (t < time)
        t+= SECS_PER_WEEK;
        
    return CreateTask(task, SchedulerTask::enWeekly, t, num);
}

template <class TASK_TYPE, const uint8_t MAX_TASKS>
uint8_t Scheduler<TASK_TYPE, MAX_TASKS>::MonthlyTask(TaskCallback task, uint8_t day, uint8_t hours, uint8_t mins, uint8_t secs, uint8_t num)
{
    if (hours > 23 || mins > 59 || secs > 59 || day > 31)
        return INVALID_TASK_ID;
    
    time_t  time = GetTime();
    
    tmElements_t    tm;
    tm.Hour   = hours;
    tm.Minute = mins;
    tm.Second = secs;
    tm.Day    = day;
    tm.Month  = month(time);
    tm.Year   = year(time);
    time_t  t = makeTime(tm);
    
    if (t < time)
        t += DaysPerMonth(t) * SECS_PER_DAY;
        
    return CreateTask((const TaskCallback)task, SchedulerTask::enMonthly, t, (uint8_t)num);
}

template <class TASK_TYPE, const uint8_t MAX_TASKS>
uint8_t Scheduler<TASK_TYPE, MAX_TASKS>::YearlyTask(TaskCallback task, uint8_t mon, uint8_t day, uint8_t hours, uint8_t mins, uint8_t secs, uint8_t num)
{
    if (hours > 23 || mins > 59 || secs > 59 || day > 31 || mon > 12)
        return INVALID_TASK_ID;
    
    time_t  time = GetTime();
    uint16_t y = year(time);
    
    tmElements_t    tm;
    tm.Hour   = hours;
    tm.Minute = mins;
    tm.Second = secs;
    tm.Day    = day;
    tm.Month  = mon;
    tm.Year   = y;
    time_t  t = makeTime(tm);
    
    if (t < time)
        t += DAYS_PER_YEAR(y) * SECS_PER_DAY;
        
    return CreateTask(task, SchedulerTask::enYearly, t, num);
}

template <class TASK_TYPE, const uint8_t MAX_TASKS>
bool Scheduler<TASK_TYPE, MAX_TASKS>::Run()
{
    IncrementTime();
    time_t time = GetTime();
    
    for (uint8_t i=0; i<MAX_TASKS; i++)
        if (!taskList[i].Run(time))
            return false;
    return true;
}

#endif // __SCHEDULER_H__