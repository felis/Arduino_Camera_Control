/* EOS Capture Example. Works with the following cameras: */
/* 400D, 450D(XSi), 40D, 5D Mark II, 7D                   */
#include <inttypes.h>
#include <avr/pgmspace.h>

#include <Spi.h>
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

#define MAX_USB_STRING_LEN 64

void setup();
void loop();
void ptpmain();

CanonEOS  Eos(DEV_ADDR, DATA_IN_EP, DATA_OUT_EP, INTERRUPT_EP, CONFIG_NUM, (PTPMAIN)&ptpmain);

void setup() {
  Serial.begin( 9600 );
  Serial.println("Start");
  Eos.Setup();
  delay( 200 );
}

void loop() {
    Eos.Task();
}

void ptpmain()
{
    if (Eos.OpenSession() == PTP_RC_OK)
    {
                Eos.Initialize(true);
                Eos.Capture();
                delay(2000);
                Eos.Initialize(false);               
		Eos.CloseSession();
    }
}

