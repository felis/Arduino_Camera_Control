#include <inttypes.h>
#include <avr/pgmspace.h>

#include <Spi.h>
#include <Max3421e.h>
#include <Max3421e_constants.h>
#include <Max_LCD.h>
#include <Usb.h>

#include <ptp.h>
#include <ptpdebug.h>
#include "devinfoparser.h"

#define DEV_ADDR        1

// Canon EOS 400D
#define DATA_IN_EP      1
#define DATA_OUT_EP     2
#define INTERRUPT_EP    3
#define CONFIG_NUM      1

#define MAX_USB_STRING_LEN 64

void setup();
void loop();
void ptpmain();

PTP  Ptp(DEV_ADDR, DATA_IN_EP, DATA_OUT_EP, INTERRUPT_EP, CONFIG_NUM, (PTPMAIN)&ptpmain);

void setup() {
  Serial.begin( 9600 );
  Serial.println("Start");
  Ptp.Setup();
  delay( 200 );
}

void loop() {
    Ptp.Task();
}

void ptpmain()
{
    if (Ptp.OpenSession() == PTP_RC_OK)
    {
        {
        	HexDump          dmp;
        	Ptp.GetDeviceInfo(&dmp);
                Notify(PSTR("\n"));
        }
        {
                DevInfoParser    prs;
        	Ptp.GetDeviceInfo(&prs);
        }
	Ptp.CloseSession();
    }
}

