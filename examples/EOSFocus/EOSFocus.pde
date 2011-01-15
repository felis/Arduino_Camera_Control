/* EOS Focus control demo */
/* Tested with 7D */
#include <inttypes.h>
#include <avr/pgmspace.h>

//#include <Spi.h>
#include <Max3421e.h>
#include <Max3421e_constants.h>
#include <Max_LCD.h>
#include <Usb.h>

#include <ptp.h>
#include <canoneos.h>

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

void setup() {
  Serial.begin( 115200 );
  Serial.println("Start");
  Eos.Setup();
  delay( 200 );
}

void loop() 
{
    Eos.Task();
}

void CamStateHandlers::OnDeviceDisconnectedState(PTP *ptp)
{
    if (stateConnected == stConnected || stateConnected == stInitial)
    {
        stateConnected = stDisconnected;
        Notify(PSTR("Camera disconnected.\r\n"));
    }
}

void CamStateHandlers::OnDeviceInitializedState(PTP *ptp)
{
    if (stateConnected == stDisconnected)
    {
        stateConnected = stConnected;

        // Switch LiveView on
        Eos.SwitchLiveView(true);
        delay(50);
        
        for (uint8_t i=0; i<10; i++)
        {
            if (i > 4)
                Eos.MoveFocus(3);
            else
                Eos.MoveFocus(0x8003);

            delay(100);
            Eos.Capture();
            delay(1500);
        }

        // Switch LiveView off
        Eos.SwitchLiveView(false);
    }
}

