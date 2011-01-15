#include "hdrcapture.h"
#include "expcomp_macro.h"

/* fixes avr-gcc incompatibility with virtual destructors */
void operator delete( void *p ) {} 

QState HDRCapture::Initial(HDRCapture *me, QEvent const *e) 
{
    return Q_TRAN(&HDRCapture::Inactive);
}

QState HDRCapture::Inactive(HDRCapture *me, QEvent const *e) 
{
    switch(e->sig)
    {
    case Q_ENTRY_SIG:
        PTPTRACE("Inactive\r\n");
        return Q_HANDLED();
    case SET_FRAMES_SIG:
        me->frmCount = (uint16_t)((SetEvt*)e)->value;
        return Q_HANDLED();
    case SET_SELF_TIMEOUT_SIG:
        me->selfTimeout = ((SetEvt*)e)->value;
        return Q_HANDLED();
    case SET_FRAME_TIMEOUT_SIG:
        me->frameTimeout = ((SetEvt*)e)->value;
        return Q_HANDLED();
    case SET_BRACKETING_SIG:
        me->bktStep     = ((SetBktEvt*)e)->step;
        me->bktNegative = ((SetBktEvt*)e)->negative;
        me->bktPositive = ((SetBktEvt*)e)->positive;
        return Q_HANDLED();
    case RUN_SIG:
        me->frmCntdn = me->frmCount;
        me->bktCntdn = (me->bktStep) ? (me->bktPositive - me->bktNegative) / me->bktStep + 1 : 1;
        me->bktPos = me->bktNegative;
        return Q_TRAN(&HDRCapture::Active);
    }
    return Q_SUPER(&QHsm::top);
}

QState HDRCapture::Active(HDRCapture *me, QEvent const *e) 
{
    switch (e->sig) 
    {
    case Q_ENTRY_SIG:
        PTPTRACE("Active\r\n");
        return Q_HANDLED();
    case Q_EXIT_SIG:
        PTPTRACE("::Q_EXIT_SIG\r\n");
        return Q_HANDLED();
    case Q_INIT_SIG:
        PTPTRACE("Q_INIT_SIG\r\n");
        return Q_TRAN(&HDRCapture::SaveSettings);
    case RUN_SIG:
        PTPTRACE("::RUN_SIG\r\n");
        return Q_TRAN(&HDRCapture::SaveSettings);
    case ABORT_SIG:
        PTPTRACE("ABORT_SIG\r\n");
        return Q_TRAN(&HDRCapture::RestoreSettings);
    case PTP_RC_SIG:
        PTPTRACE("::PTP_RC_SIG\r\n");
        switch (((PTP_RC_Evt*)e)->rc)
        {
        case PTP_RC_OK:
            me->qpEvt.sig = PTP_RC_OK_SIG;
            me->theQueue.Push(&me->qpEvt);
            return Q_HANDLED();
        case PTP_RC_DeviceBusy:
            me->toEvt.timeout = PTP_TIMEOUT;
            me->toEvt.attribs = TA_PTP_TIMEOUT;
            me->theQueue.Push(&me->toEvt);
            return Q_TRAN(&HDRCapture::Timeout);
        default:
            return Q_TRAN(&HDRCapture::Inactive);
        }
    case SET_TIMEOUT_SIG:
        PTPTRACE("::SET_TIMEOUT_SIG\r\n");
        return Q_HANDLED();
    case TIMEOUT_SIG:
        PTPTRACE("::TIMEOUT_SIG\r\n");
        return Q_TRAN(me->activeHistory);
    case PTP_RC_OK_SIG: 
        PTPTRACE("::PTP_RC_OK_SIG\r\n");
        return Q_TRAN(me->activeHistory);
    case TICK_MILLIS_SIG: 
        PTPTRACE("::TICK_MILLIS_SIG\r\n");
        return Q_HANDLED();
    }
    return Q_SUPER(&QHsm::top);
}

QState HDRCapture::SaveSettings(HDRCapture *me, QEvent const *e) 
{
    static bool bSaved = false;
    switch (e->sig) 
    {
    case Q_ENTRY_SIG:
        PTPTRACE("SaveSettings\r\n");
        me->activeHistory = (QStateHandler)&SaveSettings;
//        me->rcEvt.rc = me->Eos.GetProperty(EOS_DPC_ExposureCompensation, me->bktOldVal);
        me->rcEvt.rc = PTP_RC_OK;
        me->theQueue.Push(&me->rcEvt);
        me->bktOldVal = (uint8_t)0;
        return Q_HANDLED();
    case PTP_RC_OK_SIG:
        PTPTRACE("PTP_RC_OK_SIG\r\n");
        if (me->selfTimeout)
        { 
            me->theQueue.Push((QEvent*)e);
            return Q_TRAN(&HDRCapture::SelfTimer);
        }
        return Q_TRAN(&HDRCapture::PreCapture);
    }  // switch
    return Q_SUPER(&HDRCapture::Active);
}

QState HDRCapture::SelfTimer(HDRCapture *me, QEvent const *e) 
{
    switch (e->sig) 
    {
    case Q_ENTRY_SIG:
        PTPTRACE("SelfTimer\r\n");
        me->activeHistory = (QStateHandler)&SelfTimer;
        return Q_HANDLED();
    case TICK_MILLIS_SIG: 
        PTPTRACE("TICK_MILLIS_SIG\r\n");
        return Q_TRAN(&HDRCapture::PreCapture);
    case PTP_RC_OK_SIG:
        PTPTRACE("PTP_RC_OK_SIG\r\n");
        me->toEvt.timeout = me->selfTimeout;
        me->toEvt.attribs = TA_SELF_TIMER;
        me->theQueue.Push(&me->toEvt);
        return Q_TRAN(&HDRCapture::Timeout);
    case TIMEOUT_SIG:
        PTPTRACE("TIMEOUT_SIG\r\n");
        return Q_TRAN(&HDRCapture::PreCapture);
    }  // switch
    return Q_SUPER(&HDRCapture::Active);
}

QState HDRCapture::RestoreSettings(HDRCapture *me, QEvent const *e) 
{
    switch (e->sig) 
    {
    case Q_ENTRY_SIG:
        PTPTRACE("RestoreSettings\r\n");
        me->activeHistory = (QStateHandler)&RestoreSettings;
        me->rcEvt.rc = me->Eos.SetProperty(EOS_DPC_ExposureCompensation, me->bktOldVal);
        me->theQueue.Push(&me->rcEvt);
        return Q_HANDLED();
    case PTP_RC_OK_SIG:
        return Q_TRAN(&HDRCapture::Inactive);
    }  // switch
    return Q_SUPER(&HDRCapture::Active);
}

QState HDRCapture::PreCapture(HDRCapture *me, QEvent const *e) 
{
    switch (e->sig) 
    {
    case Q_ENTRY_SIG:
        PTPTRACE("PreCapture\r\n");
        me->activeHistory = (QStateHandler)&PreCapture;
        return Q_HANDLED();
    case TICK_MILLIS_SIG: 
        PTPTRACE("TICK_MILLIS_SIG\r\n");
        
        if (!me->bktCntdn)
        {
            me->bktCntdn = (me->bktStep) ? (me->bktPositive - me->bktNegative) / me->bktStep + 1 : 1;     
            me->bktPos = me->bktNegative;
        }
        if (me->bktStep)
            return Q_TRAN(&HDRCapture::ExpCompSet);
        
        return Q_TRAN(&HDRCapture::Capture);
    } // switch
    return Q_SUPER(&HDRCapture::Active);
}

QState HDRCapture::Capture(HDRCapture *me, QEvent const *e) 
{
    switch (e->sig) 
    {
    case Q_ENTRY_SIG:
        PTPTRACE("Capture\r\n");
        me->activeHistory = (QStateHandler)&Capture;
        me->rcEvt.rc = me->Eos.Capture();
//        me->rcEvt.rc = PTP_RC_OK;
        me->theQueue.Push(&me->rcEvt);
        return Q_HANDLED();
    case PTP_RC_OK_SIG:
        PTPTRACE("PTP_RC_OK_SIG\r\n");
        return Q_TRAN(&HDRCapture::PostCapture);
    }  // switch
    return Q_SUPER(&HDRCapture::Active);
}

QState HDRCapture::PostCapture(HDRCapture *me, QEvent const *e) 
{
    switch (e->sig) 
    {
    case Q_ENTRY_SIG:
        PTPTRACE("PostCapture\r\n");
        me->activeHistory = (QStateHandler)&PostCapture;
        return Q_HANDLED();
    case TICK_MILLIS_SIG: 
        PTPTRACE("TICK_MILLIS_SIG\r\n");

        if (!me->bktCntdn)
            return Q_TRAN(&HDRCapture::PreCapture);

        me->bktPos += me->bktStep;                
        
        if (--me->bktCntdn == 0)
        {
            --me->frmCntdn;            
            me->OnFrameCaptured(me->frmCntdn);
        }
        me->OnBktFrameCaptured(me->bktCntdn);

        // if it was the last frame in total
        if (!me->frmCntdn)
        {
            me->qpEvt.sig = ABORT_SIG;
            me->theQueue.Push(&me->qpEvt);
            return Q_HANDLED();
        }
        if (me->bktCntdn)
            return Q_TRAN(&HDRCapture::PreCapture);

        // if it was the last frame in bracketing            
        me->toEvt.timeout = me->frameTimeout;
        me->toEvt.attribs = TA_INTR_TIMER;
        me->theQueue.Push(&me->toEvt);
        return Q_TRAN(&HDRCapture::Timeout);
    case TIMEOUT_SIG:
        PTPTRACE("TIMEOUT_SIG\r\n");
        return Q_TRAN(&HDRCapture::PreCapture);
    }
    return Q_SUPER(&HDRCapture::Active);
}

QState HDRCapture::ExpCompSet(HDRCapture *me, QEvent const *e) 
{
    switch (e->sig) 
    {
    case Q_ENTRY_SIG:
        PTPTRACE("ExpCompSet\r\n");
        me->activeHistory = (QStateHandler)&ExpCompSet; 
        me->rcEvt.rc = me->Eos.SetProperty(EOS_DPC_ExposureCompensation, EXP_COMP_VALUE(me->bktPos));
//        me->rcEvt.rc = PTP_RC_OK;
        me->theQueue.Push(&me->rcEvt);
        return Q_HANDLED();
    case PTP_RC_OK_SIG:
        PTPTRACE("PTP_RC_OK_SIG\r\n");
        return Q_TRAN(&HDRCapture::Capture);
    }  // switch
    return Q_SUPER(&HDRCapture::Active);
}

QState HDRCapture::Timeout(HDRCapture *me, QEvent const *e) 
{
    static uint32_t timeout = 0;
    static uint32_t local_time = 0;
    static uint32_t seconds_left = 0;
    static uint8_t  attribs = 0;
    static uint16_t sec_cntdn = 1000;
    
    switch (e->sig) 
    {
    case Q_ENTRY_SIG:
        PTPTRACE("Timeout\r\n");
        return Q_HANDLED();
    case SET_TIMEOUT_SIG:
        attribs = ((SetTimeoutEvt*)e)->attribs;
        timeout = (attribs & 0x01) ? ((SetTimeoutEvt*)e)->timeout * 1000 : ((SetTimeoutEvt*)e)->timeout;
        
        if (attribs & 0x01)
        {
            seconds_left = ((SetTimeoutEvt*)e)->timeout;
            sec_cntdn = 1000;
            
            switch (attribs)
            {
            case TA_SELF_TIMER:
                me->OnSelfTimerProgress(seconds_left);
                break;
            case TA_INTR_TIMER:
                me->OnIntrTimerProgress(seconds_left);
                break;
//                case TA_BULB_TIMER:
//                    me->OnBulbTimerProgress(seconds_left);
//                    break;
            }  // switch
        }    
        local_time = millis();
        return Q_HANDLED();
    case TICK_MILLIS_SIG: 
        uint32_t dt = local_time;
        local_time = millis();
        dt = local_time - dt;
        timeout = (timeout < dt) ? 0 : timeout - dt;

        if (attribs & 1) 
        {
            bool bTick = (sec_cntdn <= dt);
            sec_cntdn = (sec_cntdn > dt) ? sec_cntdn - dt : 1000 - dt;
            
            if (bTick)
            {
                seconds_left --;
                
                switch (attribs)
                {
                case TA_SELF_TIMER:
                    me->OnSelfTimerProgress(seconds_left);
                    break;
                case TA_INTR_TIMER:
                    me->OnIntrTimerProgress(seconds_left);
                    break;
                }  // switch
            } // if (--sec_cntdn == 0)
        } // if (attribs & 1)
        
        if (!timeout)
        {
            me->qpEvt.sig = TIMEOUT_SIG;
            me->theQueue.Push(&me->qpEvt);
        }
        return Q_HANDLED();
    } // switch
    return Q_SUPER(&HDRCapture::Active);
}

