/* Canon Powershot control terminal   */
//#include <Spi.h>
#include <Max3421e.h>
#include <Usb.h>
#include <simpletimer.h>
#include <ptp.h>
#include <canonps.h>
#include <qep_port.h>
#include <valuelist.h>
#include <psvaluetitles.h>

#include "ptpdpparser.h"
#include "ptpobjinfoparser.h"
#include "pseventparser.h"
#include "psconsole.h"

#define DEV_ADDR        1

// Canon PowerShot S3 IS
#define DATA_IN_EP      1
#define DATA_OUT_EP     2
#define INTERRUPT_EP    3
#define CONFIG_NUM      1

class CamStateHandlers : public PSStateHandlers
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
SimpleTimer       ControlTimer, PTPPollTimer;

CanonPS           Ps(DEV_ADDR, DATA_IN_EP, DATA_OUT_EP, INTERRUPT_EP, CONFIG_NUM, &CamStates);
QEvent            evtTick, evtAbort;
PSConsole         psConsole;

void CamStateHandlers::OnDeviceDisconnectedState(PTP *ptp)
{
    if (stateConnected == stConnected || stateConnected == stInitial)
    {
        stateConnected = stDisconnected;
        PTPPollTimer.Disable();
        Notify(PSTR("Camera disconnected.\r\n"));
        
        if (stateConnected == stConnected)
            psConsole.dispatch(&evtTick);
    }
}

void CamStateHandlers::OnDeviceInitializedState(PTP *ptp)
{
    if (stateConnected == stDisconnected || stateConnected == stInitial)
    {
        stateConnected = stConnected;
        PTPPollTimer.Enable();
        psConsole.dispatch(&evtTick);
    }
    int8_t  index = psConsole.MenuSelect();
    
    if (index >= 0)
    {
        MenuSelectEvt     menu_sel_evt;
        menu_sel_evt.sig         = MENU_SELECT_SIG;
        menu_sel_evt.item_index  = index;
        psConsole.dispatch(&menu_sel_evt);      // dispatch the event
    }
}

void OnPTPPollTimer()
{
    PSEventParser  prs;
    Ps.EventCheck(&prs);
    
    if (uint32_t handle = prs.GetObjHandle())
    {
                PTPObjInfoParser     inf;
                Ps.GetObjectInfo(handle, &inf);
    }
}

void setup()
{
    Serial.begin(115200);
    Ps.Setup();
    delay( 200 );
  
    PTPPollTimer.Set(OnPTPPollTimer, 300);
    
    evtTick.sig = TICK_SIG;
//    evtAbort.sig = ABORT_SIG;
    psConsole.init();

    Serial.println("Start");
}

void loop()
{
    Ps.Task();
    PTPPollTimer.Run();
}
 
