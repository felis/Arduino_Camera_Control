#include <inttypes.h>
#include <avr/pgmspace.h>

//#include <Spi.h>
#include <Max3421e.h>
#include <Max3421e_constants.h>
#include <Max_LCD.h>
#include <Usb.h>

#include <ptp.h>
#include <ptpdebug.h>
#include <canoneos.h>
#include <simpletimer.h>

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

CamStateHandlers   CamStates;
SimpleTimer        PTPPollTimer;
CanonEOS           Eos(DEV_ADDR, DATA_IN_EP, DATA_OUT_EP, INTERRUPT_EP, CONFIG_NUM, &CamStates);


void CamStateHandlers::OnDeviceDisconnectedState(PTP *ptp)
{
    if (stateConnected == stConnected || stateConnected == stInitial)
    {
        stateConnected = stDisconnected;
        PTPPollTimer.Disable();
        Notify(PSTR("\r\nDevice disconnected.\r\n"));
    }
}

void CamStateHandlers::OnDeviceInitializedState(PTP *ptp)
{
    if (stateConnected == stDisconnected)
    {
        stateConnected = stConnected;
        PTPPollTimer.Enable();
    }
}

void OnPTPPollTimer()
{
    Serial.println("\r\n");
    
    HexDump  hex;
    Eos.EventCheck(&hex);
}

void setup() 
{
    Serial.begin( 115200 );
    Serial.println("Start");
    Eos.Setup();
    delay( 200 );
    PTPPollTimer.Set(OnPTPPollTimer, 1000);
}

void loop() 
{
    Eos.Task();
    PTPPollTimer.Run();
}

