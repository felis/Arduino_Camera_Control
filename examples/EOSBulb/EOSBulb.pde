/* Bulb mode demo */
/* Works with cameras which have bulb mode in shutter speed list (in other words, ones that don't have a letter 'B' on mode dial */
/* Tested with EOS Rebel XSI (450D) */
/* Camera has to be switched to manual mode */

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

// Canon EOS 450D
#define DATA_IN_EP      1
#define DATA_OUT_EP     2
#define INTERRUPT_EP    3
#define CONFIG_NUM      1

#define SHUTTER_SPEED_BULB       0x0c

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
} CamStates;

CanonEOS  Eos(DEV_ADDR, DATA_IN_EP, DATA_OUT_EP, INTERRUPT_EP, CONFIG_NUM, &CamStates);

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
        
         uint16_t rc = Eos.SetProperty(EOS_DPC_ShutterSpeed,SHUTTER_SPEED_BULB);
    
        if (rc != PTP_RC_OK)
            Message(PSTR("Error: "), rc);
    }

    Eos.StartBulb();
    delay(6000);
    Eos.StopBulb();
}

void setup() 
{
  Serial.begin( 115200 );
  Serial.println("Start");
  Eos.Setup();
  delay( 200 );
}

void loop() {
    Eos.Task();
}
