#if !defined(__HDRCAPTURE_H__)
#define __HDRCAPTURE_H__

#include <inttypes.h>
#include <avr/pgmspace.h>
#include <qep_port.h>
#include <ptp.h>
#include <canoneos.h>
#include <simplefifo.h>
#include <valuelist.h>

#define PTP_TIMEOUT    250

#define TA_PTP_TIMEOUT   0x00
#define TA_SELF_TIMER    0x03
#define TA_INTR_TIMER    0x05
#define TA_BULB_TIMER    0x07

enum HDRCaptureSignals 
{
    TICK_MILLIS_SIG = Q_USER_SIG,
    SET_FRAMES_SIG,
    SET_SELF_TIMEOUT_SIG,
    SET_FRAME_TIMEOUT_SIG,
    SET_BRACKETING_SIG,
    RUN_SIG,
    ABORT_SIG,
    TIMEOUT_SIG,
    SET_TIMEOUT_SIG,
    PTP_RC_SIG,
    PTP_RC_OK_SIG
};

struct SetTimeoutEvt : public QEvent 
{
    uint32_t   timeout;
    uint8_t    attribs;
};

struct SetEvt : public QEvent 
{
    uint32_t    value;
};

struct SetBktEvt : public QEvent
{
    uint8_t  step;
    uint8_t  negative;
    uint8_t  positive;
};

struct PTP_RC_Evt : public QEvent
{
    uint16_t  rc;
};

class HDRCapture : public QHsm 
{
    uint16_t    frmCntdn;
    uint16_t    frmCount;
    uint32_t    selfTimeout;
    uint32_t    frameTimeout;
    uint8_t     bktStep;
    uint8_t     bktNegative;
    uint8_t     bktPositive;
    uint8_t     bktCntdn;
    uint8_t     bktPos;
    uint8_t     bktOldVal;
    
    CanonEOS    &Eos;
    
    SetTimeoutEvt  toEvt;
    PTP_RC_Evt     rcEvt;
    QEvent         qpEvt;
    
    QStateHandler  activeHistory;

    SimpleFIFO<QEvent*, 4>  theQueue;
    
public:
    HDRCapture(CanonEOS &eos) : 
        QHsm((QStateHandler)&HDRCapture::Initial),
        frmCount(0),
        frmCntdn(0),
        selfTimeout(0),
        frameTimeout(0),
        bktStep(0),
        bktNegative(0),
        bktPositive(0),
        bktCntdn(0),
        bktPos(0),
        bktOldVal(0),
        Eos(eos),
        activeHistory(NULL)
    {
        toEvt.sig = SET_TIMEOUT_SIG;
        rcEvt.sig = PTP_RC_SIG;
    };
    void PostEvent(QEvent *e)
    {
        theQueue.Push(e);
    };
    void Run()
    {
        QEvent *e = NULL;
        
        while ( (e = theQueue.Pop()) )
            dispatch(e);
    };

protected:
    static QState Initial(HDRCapture *me, QEvent const *e);
    static QState Inactive(HDRCapture *me, QEvent const *e);
    static QState Active(HDRCapture *me, QEvent const *e);
    static QState SelfTimer(HDRCapture *me, QEvent const *e);
    static QState PreCapture(HDRCapture *me, QEvent const *e);
    static QState Capture(HDRCapture *me, QEvent const *e);
    static QState PostCapture(HDRCapture *me, QEvent const *e);
    static QState ExpCompSet(HDRCapture *me, QEvent const *e);
    static QState SaveSettings(HDRCapture *me, QEvent const *e);
    static QState RestoreSettings(HDRCapture *me, QEvent const *e);
    static QState Timeout(HDRCapture *me, QEvent const *e);
    
    virtual void OnFrameCaptured(uint16_t left) {};
    virtual void OnBktFrameCaptured(uint16_t left) {};
    virtual void OnSelfTimerProgress(uint32_t left) {};
    virtual void OnIntrTimerProgress(uint32_t left) {};
};

#endif // __HDRCAPTURE_H__
