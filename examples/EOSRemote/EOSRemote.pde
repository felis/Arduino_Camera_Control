/* EOS control terminal */
//#include <Spi.h>
#include <Max3421e.h>
#include <Usb.h>
#include <simpletimer.h>
#include <ptp.h>
#include <canoneos.h>
#include <qep_port.h>
#include <valuelist.h>
#include <eosvaluetitles.h>

#include "eoseventparser.h"
#include "eosconsole.h"

#define DEV_ADDR        1

// Canon EOS 400D
#define DATA_IN_EP      1
#define DATA_OUT_EP     2
#define INTERRUPT_EP    3
#define CONFIG_NUM      1

#define EEP_APERTURE_LIST_OFFSET    0
#define EEP_APERTURE_LIST_SIZE      32

#define EEP_SHTSPEED_LIST_OFFSET    (EEP_APERTURE_LIST_OFFSET + EEP_APERTURE_LIST_SIZE)
#define EEP_SHTSPEED_LIST_SIZE      64

#define EEP_WBALANCE_LIST_OFFSET    (EEP_SHTSPEED_LIST_OFFSET + EEP_SHTSPEED_LIST_SIZE)
#define EEP_WBALANCE_LIST_SIZE      12

#define EEP_PICSTYLE_LIST_OFFSET    (EEP_WBALANCE_LIST_OFFSET + EEP_WBALANCE_LIST_SIZE)
#define EEP_PICSTYLE_LIST_SIZE      12

#define EEP_EXPOCOR_LIST_OFFSET     (EEP_PICSTYLE_LIST_OFFSET + EEP_PICSTYLE_LIST_SIZE)
#define EEP_EXPOCOR_LIST_SIZE       48

#define EEP_ISO_LIST_OFFSET         (EEP_EXPOCOR_LIST_OFFSET + EEP_EXPOCOR_LIST_SIZE)
#define EEP_ISO_LIST_SIZE           8

EEPROMByteList          vlAperture(EEP_APERTURE_LIST_OFFSET, EEP_APERTURE_LIST_SIZE);
EEPROMByteList          vlShutterSpeed(EEP_SHTSPEED_LIST_OFFSET, EEP_SHTSPEED_LIST_SIZE);
EEPROMByteList          vlWhiteBalance(EEP_WBALANCE_LIST_OFFSET, EEP_WBALANCE_LIST_SIZE);
EEPROMByteList          vlPictureStyle(EEP_PICSTYLE_LIST_OFFSET, EEP_PICSTYLE_LIST_SIZE);
EEPROMByteList          vlIso(EEP_EXPOCOR_LIST_OFFSET, EEP_EXPOCOR_LIST_SIZE);
EEPROMByteList          vlExpCompensation(EEP_EXPOCOR_LIST_OFFSET, EEP_EXPOCOR_LIST_SIZE);

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
SimpleTimer       ControlTimer, PTPPollTimer;

CanonEOS          Eos(DEV_ADDR, DATA_IN_EP, DATA_OUT_EP, INTERRUPT_EP, CONFIG_NUM, &CamStates);
QEvent            evtTick, evtAbort;
EOSConsole        eosConsole;

uint8_t  dpMode         = 0;
uint8_t  dpAperture     = 0;
uint8_t  dpShutterSpeed = 0;
uint8_t  dpWb           = 0;
uint8_t  dpPStyle       = 0;
uint8_t  dpIso          = 0;
uint8_t  dpExpComp      = 0;

void CamStateHandlers::OnDeviceDisconnectedState(PTP *ptp)
{
    if (stateConnected == stConnected || stateConnected == stInitial)
    {
        stateConnected = stDisconnected;
        PTPPollTimer.Disable();
        Notify(PSTR("Camera disconnected.\r\n"));
        
        if (stateConnected == stConnected)
            eosConsole.dispatch(&evtTick);
    }
}

void CamStateHandlers::OnDeviceInitializedState(PTP *ptp)
{
    if (stateConnected == stDisconnected)
    {
        stateConnected = stConnected;
        PTPPollTimer.Enable();
        eosConsole.dispatch(&evtTick);
    }
    int8_t  index = eosConsole.MenuSelect();
    
    if (index >= 0)
    {
        MenuSelectEvt     menu_sel_evt;
        menu_sel_evt.sig         = MENU_SELECT_SIG;
        menu_sel_evt.item_index  = index;
        eosConsole.dispatch(&menu_sel_evt);      // dispatch the event
    }
}

void OnControlTimer()
{
//    ExtControls.CheckControls();
//    hdrCapture.PostEvent(&evtTick);
//    Screen::Run(&LCD);
}

void OnPTPPollTimer()
{
    EOSEventParser    prs;
    Eos.EventCheck(&prs);
}

void setup()
{
    Serial.begin(115200);
    Eos.Setup();
    delay( 200 );
  
    PTPPollTimer.Set(OnPTPPollTimer, 300);
    
    // 1ms is the perfect interval for encoder polling
    ControlTimer.Set(OnControlTimer, 1);
    ControlTimer.Enable();
  
    evtTick.sig = TICK_SIG;
//    evtAbort.sig = ABORT_SIG;
    eosConsole.init();

    Serial.println("Start");
}

void loop()
{
    Eos.Task();
    PTPPollTimer.Run();
    ControlTimer.Run();
}
 
