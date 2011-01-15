/* Exposure compensation change demo */
#include <inttypes.h>
#include <avr/pgmspace.h>

//#include <Spi.h>
#include <Max3421e.h>
#include <Max3421e_constants.h>
#include <Max_LCD.h>
#include <Usb.h>
#include <ptp.h>
#include <canoneos.h>
#include <qep_port.h>

#include "hdrcapture.h"

#define DEV_ADDR        1

// Canon EOS 400D
#define DATA_IN_EP      1
#define DATA_OUT_EP     2
#define INTERRUPT_EP    3
#define CONFIG_NUM      1

class CamStateHandlers : public EOSStateHandlers
{
      enum CamStates { stInitial, stDisconnected, stConnected };
      CamStates stateConnected;
    
public:
      CamStateHandlers() : stateConnected(stInitial) 
      {
      };
      
      virtual void OnDeviceDisconnectedState(PTP *ptp);
      virtual void OnDeviceInitializedState(PTP *ptp);
};

CamStateHandlers  CamStates;
CanonEOS          Eos(DEV_ADDR, DATA_IN_EP, DATA_OUT_EP, INTERRUPT_EP, CONFIG_NUM, &CamStates);
HDRCapture        hdrCapture(Eos);
QEvent            msTick;
QEvent            secTick;

void CamStateHandlers::OnDeviceDisconnectedState(PTP *ptp)
{
    if (stateConnected == stConnected || stateConnected == stInitial)
    {
        stateConnected = stDisconnected;
        Notify(PSTR("Camera disconnected\r\n"));
    }
}

void CamStateHandlers::OnDeviceInitializedState(PTP *ptp)
{
    if (stateConnected == stDisconnected)
    {
        stateConnected = stConnected;
        
        Serial.println("Connected");

        SetEvt         setEvt;
        setEvt.sig     = RUN_SIG;
        
        hdrCapture.PostEvent(&setEvt);
    }
    hdrCapture.Run();
    hdrCapture.PostEvent(&msTick);                   // post TICK_MILLIS_SIG event
}

void setup()
{
    Serial.begin( 115200 );
    Serial.println("Start");

    Eos.Setup();
    delay( 200 );

    hdrCapture.init();
    
    SetEvt  setEvt;
    
    setEvt.sig     = SET_FRAMES_SIG;
    setEvt.value   = 3;
    
    hdrCapture.dispatch(&setEvt);
    
    setEvt.sig     = SET_FRAME_TIMEOUT_SIG;
    setEvt.value   = 5;
    
    hdrCapture.dispatch(&setEvt);
    
    setEvt.sig     = SET_SELF_TIMEOUT_SIG;
    setEvt.value   = 3;
    
    hdrCapture.dispatch(&setEvt);
    
    SetBktEvt          setBktEvt;
    setBktEvt.sig       = SET_BRACKETING_SIG;
    setBktEvt.step      = 2;
    setBktEvt.negative  = 3;
    setBktEvt.positive  = 9;
    
    hdrCapture.dispatch(&setBktEvt);
    
    
    msTick.sig = TICK_MILLIS_SIG;
}

void loop()
{
    Eos.Task();
    delay(1);
}

