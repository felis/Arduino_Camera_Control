/* Canon Powershot Device Property Parser */
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
#include <ptpdpparser.h>
#include <psvaluetitles.h>
#include "devpropparser.h"

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
CanonPS           Ps(DEV_ADDR, DATA_IN_EP, DATA_OUT_EP, INTERRUPT_EP, CONFIG_NUM, &CamStates);

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
    if (stateConnected == stDisconnected || stateConnected == stInitial)
    {
        stateConnected = stConnected;
        
        uint16_t    prefix = 0xD000;
               
        uint16_t  shoot_mode[] = {0x01, 0x02, 0x03, 0x04};
        
        for (uint8_t i=0; i<4; i++)
        {
            Ps.SetDevicePropValue(PS_DPC_ShootingMode, (uint16_t)shoot_mode[i]);
            delay(10);

            Notify(PSTR("Mode:"));
            PrintValueTitle<uint8_t, VT_MODE, VT_MODE_COUNT, VT_MODE_TEXT_LEN>((PTP*)ptp, PS_DPC_ShootingMode, ModeTitles);
            Notify(PSTR("\r\n"));
            
            for (uint8_t j=0; j<128; j++)
            {
                HexDump          dmp;
                
                if (Ps.GetDevicePropDesc((prefix | j), &dmp) == PTP_RC_OK)
                {
                    Notify(PSTR("\r\n"));
                    
                    DevPropParser    prs;
                    
            	    if (Ps.GetDevicePropDesc((prefix | j), &prs) == PTP_RC_OK)
                        Notify(PSTR("\r\n"));
                }
            }
    
            Notify(PSTR("\r\n"));

        } // for (uint8_t i=0; i<4; i++)

    }  // if (stateConnected == stDisconnected || stateConnected == stInitial)
}

void setup() 
{
  Serial.begin( 115200 );
  Serial.println("Start");
  Ps.Setup();
  delay( 200 );
}

void loop() 
{
    Ps.Task();
}

