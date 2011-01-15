#include "psconsole.h"
#include <psvaluetitles.h>
#include <valuelist.h>

extern CanonPS                 Ps;

/* fixes avr-gcc incompatibility with virtual destructors */
void operator delete( void *p ) {} 

const char* menuMain[] = {"Capture", "View Settings", "Change Settings", "Viewfinder On", "Viewfinder Off"};
const char* menuChangeSettings[] = {"Mode", "Aperture", "Shutter Speed", "WB", "ISO", "Exp Comp", "CamOutput", "Zoom"};
const char* menuUpDown[] = {"<<", ">>"};

#define MAIN_MENU_COUNT sizeof(menuMain) / sizeof(const char*)
#define CHSET_MENU_COUNT sizeof(menuChangeSettings) / sizeof(const char*)
#define UPDWN_MENU_COUNT sizeof(menuUpDown) / sizeof(const char*)

void PrintMode()
{
    Notify(PSTR("Mode:"));
    PrintValueTitle<uint8_t, VT_MODE, VT_MODE_COUNT, VT_MODE_TEXT_LEN>((PTP*)&Ps, PS_DPC_ShootingMode, ModeTitles);
    Notify(PSTR("\r\n"));
}

void PrintAperture()
{
    Notify(PSTR("F:"));
    PrintValueTitle<uint16_t, VT_APERTURE, VT_APT_COUNT, VT_APT_TEXT_LEN>((PTP*)&Ps, PS_DPC_Aperture, ApertureTitles);
    Notify(PSTR("\r\n"));
}

void PrintShutterSpeed()
{
    Notify(PSTR("T:"));
    PrintValueTitle<uint16_t, VT_SHSPEED, VT_SHSPEED_COUNT, VT_SHSPEED_TEXT_LEN>((PTP*)&Ps, PS_DPC_ShutterSpeed, ShutterSpeedTitles);
    Notify(PSTR("\r\n"));
}

void PrintWB()
{
    Notify(PSTR("WB:"));
    PrintValueTitle<uint8_t, VT_WB, VT_WB_COUNT, VT_WB_TEXT_LEN>((PTP*)&Ps, PS_DPC_WhiteBalance, WbTitles);
    Notify(PSTR("\r\n"));
}

void PrintIso()
{
    Notify(PSTR("ISO:"));
    PrintValueTitle<uint16_t, VT_ISO, VT_ISO_COUNT, VT_ISO_TEXT_LEN>((PTP*)&Ps, PS_DPC_ISOSpeed, IsoTitles);
    Notify(PSTR("\r\n"));
}

void PrintExpCompensation()
{
    Notify(PSTR("ExpComp:"));
    PrintValueTitle<uint8_t, VT_EXPCOMP, VT_EXPCOMP_COUNT, VT_EXPCOMP_TEXT_LEN>((PTP*)&Ps, PS_DPC_ExpCompensation, ExpCompTitles);
    Notify(PSTR("\r\n"));
}

void PrintCamOutput()
{
    Notify(PSTR("CamOutput:"));
    PrintValueTitle<uint8_t, VT_CAMOUTPUT, VT_CAMOUTPUT_COUNT, VT_CAMOUTPUT_TEXT_LEN>((PTP*)&Ps, PS_DPC_CameraOutput, CamOutputTitles);
    Notify(PSTR("\r\n"));
}

void PrintZoom()
{
    uint16_t    val = 0;
    Notify(PSTR("Zoom:"));
    
    if (Ps.GetDevicePropValue(PS_DPC_Zoom, (uint16_t&)val) == PTP_RC_OK)
        PrintHex<uint16_t>(val); 
        
    Notify(PSTR("\r\n"));
}

void PSConsole::ShowParams()
{
    PrintMode();
    PrintAperture();
    PrintShutterSpeed();
    PrintWB();
    PrintIso();
    PrintExpCompensation();
    PrintCamOutput();
    PrintZoom();
}

QState PSConsole::Initial(PSConsole *me, QEvent const *e) 
{
    return Q_TRAN(&PSConsole::Inactive);
}

QState PSConsole::Inactive(PSConsole *me, QEvent const *e) 
{
    switch (e->sig) 
    {
        case Q_ENTRY_SIG: 
            Notify(PSTR("Inactive\r\n"));
            return Q_HANDLED();
        case TICK_SIG: 
            return Q_TRAN(&PSConsole::Active);
    }
    return Q_SUPER(QHsm::top);
}

QState PSConsole::Active(PSConsole *me, QEvent const *e) 
{
    switch (e->sig) 
    {
        case Q_ENTRY_SIG: 
            Notify(PSTR("Active\r\n"));
            return Q_HANDLED();
        case Q_INIT_SIG: 
            return Q_TRAN(&PSConsole::MainMenu);
        case TICK_SIG:
            return Q_TRAN(&PSConsole::Inactive);
    }
    return Q_SUPER(QHsm::top);
}

void PSConsole::PrintMenuTitles(uint8_t count, const char **menu)
{
    Serial.println("");
    for (uint8_t i=0; i<=count; i++)
    {
        Serial.print(i, DEC);
        Serial.print(". ");
        
        if (i == 0)
            Serial.println("<..>");
        else
            Serial.println(menu[i-1]);
    }
    Serial.println("");
}

QState PSConsole::MainMenu(PSConsole *me, QEvent const *e) 
{
    switch (e->sig) 
    {
        case Q_ENTRY_SIG:
            PrintMenuTitles(MAIN_MENU_COUNT, menuMain);
            return Q_HANDLED();
        case MENU_SELECT_SIG: 
        {
            switch (((MenuSelectEvt*)e)->item_index)
            {
                case 0:
                    PrintMenuTitles(MAIN_MENU_COUNT, menuMain);
                    return Q_HANDLED();
                case 1:
                    Ps.SetDevicePropValue(PS_DPC_CaptureTransferMode, (uint16_t)0x0D);
                    Ps.Capture();
                    return Q_HANDLED();
                case 2:
                    ShowParams();
                    PrintMenuTitles(MAIN_MENU_COUNT, menuMain);
                    return Q_HANDLED();
                case 3:
                    return Q_TRAN(&PSConsole::ChangeSettingsMenu);
                case 4:
                    Ps.Operation(PS_OC_ViewfinderOn);
                    return Q_HANDLED();
                case 5:
                    Ps.Operation(PS_OC_ViewfinderOff);
                    return Q_HANDLED();
            }
        }
    }
    return Q_SUPER(&PSConsole::Active);
}

QState PSConsole::ChangeSettingsMenu(PSConsole *me, QEvent const *e) 
{
    switch (e->sig) 
    {
        case Q_ENTRY_SIG:
            PrintMenuTitles(CHSET_MENU_COUNT, menuChangeSettings);
            return Q_HANDLED();
        case MENU_SELECT_SIG: 
        {
            switch (((MenuSelectEvt*)e)->item_index)
            {
                case 0:
                    return Q_TRAN(&PSConsole::MainMenu);
                case 1:  // Aperture
                      return Q_TRAN(&PSConsole::ChangeModeMenu);
                case 2:  // Aperture
                      return Q_TRAN(&PSConsole::ChangeApertureMenu);
                case 3:  // Shutter Speed
                      return Q_TRAN(&PSConsole::ChangeShutterSpeedMenu);
                case 4:  // White Balance
                      return Q_TRAN(&PSConsole::ChangeWBMenu);
                case 5:  // ISO
                      return Q_TRAN(&PSConsole::ChangeIsoMenu);
                case 6:  // Exposure Compensation
                      return Q_TRAN(&PSConsole::ChangeExpCompMenu);
                case 7:  // Camera Output
                      return Q_TRAN(&PSConsole::ChangeCamOutputMenu);
                case 8:  // Zoom
                      return Q_TRAN(&PSConsole::ChangeZoomMenu);
            } // switch
        }
    }
    return Q_SUPER(&PSConsole::Active);
}

QState PSConsole::ChangeModeMenu(PSConsole *me, QEvent const *e) 
{
    switch (e->sig) 
    {
        case Q_ENTRY_SIG:
            PrintMenuTitles(UPDWN_MENU_COUNT, menuUpDown);
            PrintMode();
            return Q_HANDLED();
        case MENU_SELECT_SIG: 
        {
            uint8_t new_value;
            
            switch (((MenuSelectEvt*)e)->item_index)
            {
            case 0:
                return Q_TRAN(&PSConsole::ChangeSettingsMenu);
            case 2:
                StepUp<uint8_t>((PTP*)&Ps, PS_DPC_ShootingMode);
                PrintMode();
                return Q_HANDLED();
            case 1:
                StepDown<uint8_t>((PTP*)&Ps, PS_DPC_ShootingMode);
                PrintMode();
                return Q_HANDLED();
            } // switch (((MenuSelectEvt*)e)->item_index)
        } // case MENU_SELECT_SIG:
    }
    return Q_SUPER(&PSConsole::Active);
}

QState PSConsole::ChangeApertureMenu(PSConsole *me, QEvent const *e) 
{
    switch (e->sig) 
    {
        case Q_ENTRY_SIG:
            PrintMenuTitles(UPDWN_MENU_COUNT, menuUpDown);
            PrintAperture();
            return Q_HANDLED();
        case MENU_SELECT_SIG: 
        {
            uint8_t new_value;
            
            switch (((MenuSelectEvt*)e)->item_index)
            {
            case 0:
                return Q_TRAN(&PSConsole::ChangeSettingsMenu);
            case 2:
                StepUp<uint16_t>((PTP*)&Ps, PS_DPC_Aperture);
                PrintAperture();
                return Q_HANDLED();
            case 1:
                StepDown<uint16_t>((PTP*)&Ps, PS_DPC_Aperture);
                PrintAperture();
                return Q_HANDLED();
            } // switch (((MenuSelectEvt*)e)->item_index)
        } // case MENU_SELECT_SIG:
    }
    return Q_SUPER(&PSConsole::Active);
}

QState PSConsole::ChangeShutterSpeedMenu(PSConsole *me, QEvent const *e) 
{
    switch (e->sig) 
    {
        case Q_ENTRY_SIG:
            PrintMenuTitles(UPDWN_MENU_COUNT, menuUpDown);
            PrintShutterSpeed();
            return Q_HANDLED();
        case MENU_SELECT_SIG: 
        {
            uint8_t new_value;
            
            switch (((MenuSelectEvt*)e)->item_index)
            {
            case 0:
                return Q_TRAN(&PSConsole::ChangeSettingsMenu);
            case 2:
                StepUp<uint16_t>((PTP*)&Ps, PS_DPC_ShutterSpeed);
                PrintShutterSpeed();
                return Q_HANDLED();
            case 1:
                StepDown<uint16_t>((PTP*)&Ps, PS_DPC_ShutterSpeed);
                PrintShutterSpeed();
                return Q_HANDLED();
            } // switch (((MenuSelectEvt*)e)->item_index)
        } // case MENU_SELECT_SIG:
    }
    return Q_SUPER(&PSConsole::Active);
}

QState PSConsole::ChangeWBMenu(PSConsole *me, QEvent const *e) 
{
    switch (e->sig) 
    {
        case Q_ENTRY_SIG:
            PrintMenuTitles(UPDWN_MENU_COUNT, menuUpDown);
            PrintWB();
            return Q_HANDLED();
        case MENU_SELECT_SIG: 
        {
            uint8_t new_value;
            
            switch (((MenuSelectEvt*)e)->item_index)
            {
            case 0:
                return Q_TRAN(&PSConsole::ChangeSettingsMenu);
            
            case 2:
                StepUp<uint8_t>((PTP*)&Ps, PS_DPC_WhiteBalance);
                PrintWB();
                return Q_HANDLED();
            case 1:
                StepDown<uint8_t>((PTP*)&Ps, PS_DPC_WhiteBalance);
                PrintWB();
                return Q_HANDLED();
            } // switch (((MenuSelectEvt*)e)->item_index)
        } // case MENU_SELECT_SIG:
    }
    return Q_SUPER(&PSConsole::Active);
}

QState PSConsole::ChangeIsoMenu(PSConsole *me, QEvent const *e) 
{
    switch (e->sig) 
    {
        case Q_ENTRY_SIG:
            PrintMenuTitles(UPDWN_MENU_COUNT, menuUpDown);
            PrintIso();
            return Q_HANDLED();
        case MENU_SELECT_SIG: 
        {
            uint8_t new_value;
            
            switch (((MenuSelectEvt*)e)->item_index)
            {
            case 0:
                return Q_TRAN(&PSConsole::ChangeSettingsMenu);
            case 2:
                StepUp<uint16_t>((PTP*)&Ps, PS_DPC_ISOSpeed);
                PrintIso();
                return Q_HANDLED();
            case 1:
                StepDown<uint16_t>((PTP*)&Ps, PS_DPC_ISOSpeed);
                PrintIso();
                return Q_HANDLED();
            } // switch (((MenuSelectEvt*)e)->item_index)
        } // case MENU_SELECT_SIG:
    }
    return Q_SUPER(&PSConsole::Active);
}

QState PSConsole::ChangeExpCompMenu(PSConsole *me, QEvent const *e) 
{
    switch (e->sig) 
    {
        case Q_ENTRY_SIG:
            PrintMenuTitles(UPDWN_MENU_COUNT, menuUpDown);
            PrintExpCompensation();
            return Q_HANDLED();
        case MENU_SELECT_SIG: 
        {
            uint8_t new_value;
            
            switch (((MenuSelectEvt*)e)->item_index)
            {
            case 0:
                return Q_TRAN(&PSConsole::ChangeSettingsMenu);
            case 1:
                StepUp<uint8_t>((PTP*)&Ps, PS_DPC_ExpCompensation);
                PrintExpCompensation();
                return Q_HANDLED();
            case 2:
                StepDown<uint8_t>((PTP*)&Ps, PS_DPC_ExpCompensation);
                PrintExpCompensation();
                return Q_HANDLED();
            } // switch (((MenuSelectEvt*)e)->item_index)
        } // case MENU_SELECT_SIG:
    }
    return Q_SUPER(&PSConsole::Active);
}

QState PSConsole::ChangeCamOutputMenu(PSConsole *me, QEvent const *e) 
{
    switch (e->sig) 
    {
        case Q_ENTRY_SIG:
            PrintMenuTitles(UPDWN_MENU_COUNT, menuUpDown);
            PrintCamOutput();
            return Q_HANDLED();
        case MENU_SELECT_SIG: 
        {
            switch (((MenuSelectEvt*)e)->item_index)
            {
            case 0:
                return Q_TRAN(&PSConsole::ChangeSettingsMenu);
            case 2:
                StepUp<uint8_t>((PTP*)&Ps, PS_DPC_CameraOutput);
                PrintCamOutput();
                return Q_HANDLED();
            case 1:
                StepDown<uint8_t>((PTP*)&Ps, PS_DPC_CameraOutput);
                PrintCamOutput();
                return Q_HANDLED();
            } // switch (((MenuSelectEvt*)e)->item_index)
        } // case MENU_SELECT_SIG:
    }
    return Q_SUPER(&PSConsole::Active);
}

QState PSConsole::ChangeZoomMenu(PSConsole *me, QEvent const *e) 
{
    switch (e->sig) 
    {
        case Q_ENTRY_SIG:
            PrintMenuTitles(UPDWN_MENU_COUNT, menuUpDown);
            PrintZoom();
            return Q_HANDLED();
        case MENU_SELECT_SIG: 
        {
            switch (((MenuSelectEvt*)e)->item_index)
            {
            case 0:
                return Q_TRAN(&PSConsole::ChangeSettingsMenu);
            case 2:
                StepUp<uint16_t>((PTP*)&Ps, PS_DPC_Zoom);
                PrintZoom();
                return Q_HANDLED();
            case 1:
                StepDown<uint16_t>((PTP*)&Ps, PS_DPC_Zoom);
                PrintZoom();
                return Q_HANDLED();
            } // switch (((MenuSelectEvt*)e)->item_index)
        } // case MENU_SELECT_SIG:
    }
    return Q_SUPER(&PSConsole::Active);
}

static TickEvt     tick_evt;

int8_t PSConsole::MenuSelect()
{
    if( !Serial.available()) 
        return -1;
      
    uint8_t  char_count = 0;
    uint8_t  index = 0;
          
    while (Serial.available() > 0 && char_count < 2)
    {
        uint8_t key = Serial.read();
        key -= '0';
                  
        if (index)
        {
	    uint8_t tmp = index;
	    // index *= 10;
	    index <<= 3;
	    index += tmp;
	    index += tmp;
        }
        index += key;
        char_count ++;
    }
    return (char_count) ? (int8_t)index : (int8_t)-1;
}

