/* Canon Powershot Capture Demo */
#include <inttypes.h>
#include <avr/pgmspace.h>

//#include <Spi.h>
#include <Max3421e.h>
#include <Max3421e_constants.h>
#include <Max_LCD.h>
#include <Usb.h>

#include <ptp.h>
#include <ptpdebug.h>
#include <canonps.h>
#include <simpletimer.h>
#include "pseventparser.h"
#include "ptpobjinfoparser.h"

#define DEV_ADDR        1

// Canon PowerShot S3 IS
#define DATA_IN_EP      1
#define DATA_OUT_EP     2
#define INTERRUPT_EP    3
#define CONFIG_NUM      1

void setup();
void loop();

class CamStateHandlers : public PSStateHandlers
{
      bool stateConnected;
    
public:
      CamStateHandlers() : stateConnected(false) {};
      
      virtual void OnDeviceDisconnectedState(PTP *ptp);
      virtual void OnDeviceInitializedState(PTP *ptp);
} CamStates;

CanonPS  Ps(DEV_ADDR, DATA_IN_EP, DATA_OUT_EP, INTERRUPT_EP, CONFIG_NUM, &CamStates);
SimpleTimer  eventTimer, captureTimer;

void CamStateHandlers::OnDeviceDisconnectedState(PTP *ptp)
{
    if (stateConnected)
    {
        eventTimer.Disable();
        captureTimer.Disable();
        stateConnected = false;
        
        Notify(PSTR("Camera disconnected\r\n"));
    }
}

void CamStateHandlers::OnDeviceInitializedState(PTP *ptp)
{
    if (!stateConnected)
    {
        Notify(PSTR("stateConnected\r\n"));
        
        stateConnected = true;
        eventTimer.Enable();
        captureTimer.Enable();
    }
}

void setup() 
{
  Serial.begin( 115200 );
  Serial.println("Start");
  Ps.Setup();
  eventTimer.Set(&OnEventTimer, 200);
  captureTimer.Set(&OnCaptureTimer, 5000);
  delay( 200 );
}

void loop() 
{
    eventTimer.Run();
    captureTimer.Run();
    Ps.Task();
}

void OnCaptureTimer()
{
    Ps.SetDevicePropValue(PS_DPC_CaptureTransferMode, (uint16_t)0x0D);
            
    uint16_t rc = Ps.Capture();
    
    if (rc != PTP_RC_OK)
        Message(PSTR("Error: "), rc);
}

void OnEventTimer()
{
    PSEventParser  prs;
    Ps.EventCheck(&prs);
    
    if (uint32_t handle = prs.GetObjHandle())
    {
                PTPObjInfoParser     inf;
                Ps.GetObjectInfo(handle, &inf);
    }
}


