#include <inttypes.h>
#include <avr/pgmspace.h>

#include <Max3421e.h>
#include <Max3421e_constants.h>
#include <Max_LCD.h>
#include <Usb.h>

#include <ptp.h>

#define DEV_ADDR        1

// Nikon CoolPix P100
#define DATA_IN_EP      2
#define DATA_OUT_EP     1
#define INTERRUPT_EP    3
#define CONFIG_NUM      1

// Canon 
//#define DATA_IN_EP      1
//#define DATA_OUT_EP     2
//#define INTERRUPT_EP    3
//#define CONFIG_NUM      1

#define MAX_USB_STRING_LEN 64

class CamStateHandlers : public PTPStateHandlers
{
      bool stateConnected;
    
public:
      CamStateHandlers() : stateConnected(false) {};
      
      virtual void OnDeviceDisconnectedState(PTP *ptp);
      virtual void OnDeviceInitializedState(PTP *ptp);
} CamStates;

PTP  Ptp(DEV_ADDR, DATA_IN_EP, DATA_OUT_EP, INTERRUPT_EP, CONFIG_NUM, &CamStates);

void CamStateHandlers::OnDeviceDisconnectedState(PTP *ptp)
{
    if (stateConnected)
    {
        stateConnected = false;
        Notify(PSTR("Camera disconnected\r\n"));
    }
}

void CamStateHandlers::OnDeviceInitializedState(PTP *ptp)
{
    if (!stateConnected)
    {
        stateConnected = true;
        
        Ptp.CaptureImage();
        delay(1000);
    }
}

void setup() {
  Serial.begin( 115200 );
  Serial.println("Start");
  Ptp.Setup();
  delay( 200 );
}

void loop() 
{
    Ptp.Task();
}

