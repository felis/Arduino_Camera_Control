#include "eosconsole.h"
#include <eosvaluetitles.h>
#include <valuelist.h>

extern CanonEOS                Eos;
extern EEPROMByteList          vlAperture;
extern EEPROMByteList          vlShutterSpeed;
extern EEPROMByteList          vlWhiteBalance;
extern EEPROMByteList          vlPictureStyle;
extern EEPROMByteList          vlIso;
extern EEPROMByteList          vlExpCompensation;

extern uint8_t                 dpMode;
extern uint8_t                 dpAperture;
extern uint8_t                 dpShutterSpeed;
extern uint8_t                 dpWb;
extern uint8_t                 dpPStyle;
extern uint8_t                 dpIso;
extern uint8_t                 dpExpComp;

/* fixes avr-gcc incompatibility with virtual destructors */
void operator delete( void *p ) {} 

const char* menuMain[] = {"Capture", "View Settings", "Change Settings"};
const char* menuChangeSettings[] = {"Aperture", "Shutter Speed", "WB", "Pict Style", "ISO", "Exp Comp"};
const char* menuUpDown[] = {"<<", ">>"};

void EOSConsole::ShowParams()
{
    Notify(PSTR("\r\nMode:"));
    Notify((char*)FindTitle<VT_MODE, VT_MODE_TEXT_LEN>(VT_MODE_COUNT, ModeTitles, dpMode));
    Notify(PSTR("\r\nF:"));
    Notify((char*)FindTitle<VT_APERTURE, VT_APT_TEXT_LEN>(VT_APT_COUNT, ApertureTitles, dpAperture));
    Notify(PSTR("\r\nT:"));
    Notify((char*)FindTitle<VT_SHSPEED, VT_SHSPEED_TEXT_LEN>(VT_SHSPEED_COUNT, ShutterSpeedTitles, dpShutterSpeed));
    Notify(PSTR("\r\nWB:"));
    Notify((char*)FindTitle<VT_WB, VT_WB_TEXT_LEN>(VT_WB_COUNT, WbTitles, dpWb));
    Notify(PSTR("\r\nPict Style:"));
    Notify((char*)FindTitle<VT_PSTYLE, VT_PSTYLE_TEXT_LEN>(VT_PSTYLE_COUNT, PStyleTitles, dpPStyle));
    Notify(PSTR("\r\nISO:"));
    Notify((char*)FindTitle<VT_ISO, VT_ISO_TEXT_LEN>(VT_ISO_COUNT, IsoTitles, dpIso));
    Notify(PSTR("\r\nExp Comp:"));
    Notify((char*)FindTitle<VT_EXPCOMP, VT_EXPCOMP_TEXT_LEN>(VT_EXPCOMP_COUNT, ExpCompTitles, dpExpComp));
    Notify(PSTR("\r\n"));
}

QState EOSConsole::Initial(EOSConsole *me, QEvent const *e) 
{
    return Q_TRAN(&EOSConsole::Inactive);
}

QState EOSConsole::Inactive(EOSConsole *me, QEvent const *e) 
{
    switch (e->sig) 
    {
        case TICK_SIG: 
            return Q_TRAN(&EOSConsole::Active);
    }
    return Q_SUPER(QHsm::top);
}

QState EOSConsole::Active(EOSConsole *me, QEvent const *e) 
{
    switch (e->sig) 
    {
        case Q_INIT_SIG: 
            return Q_TRAN(&EOSConsole::MainMenu);
        case TICK_SIG:
            return Q_TRAN(&EOSConsole::Inactive);
    }
    return Q_SUPER(QHsm::top);
}

void EOSConsole::PrintMenuTitles(uint8_t count, const char **menu)
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

QState EOSConsole::MainMenu(EOSConsole *me, QEvent const *e) 
{
    switch (e->sig) 
    {
        case Q_ENTRY_SIG:
            PrintMenuTitles(3, menuMain);
            return Q_HANDLED();
        case MENU_SELECT_SIG: 
        {
            switch (((MenuSelectEvt*)e)->item_index)
            {
                case 0:
                    PrintMenuTitles(3, menuMain);
                    return Q_HANDLED();
                case 1:
                    Eos.Capture();
                    return Q_HANDLED();
                case 2:
                    ShowParams();
                    PrintMenuTitles(3, menuMain);
                    return Q_HANDLED();
                case 3:
                    return Q_TRAN(&EOSConsole::ChangeSettingsMenu);
            }
        }
    }
    return Q_SUPER(&EOSConsole::Active);
}

QState EOSConsole::ChangeSettingsMenu(EOSConsole *me, QEvent const *e) 
{
    switch (e->sig) 
    {
        case Q_ENTRY_SIG:
            PrintMenuTitles(6, menuChangeSettings);
            return Q_HANDLED();
        case MENU_SELECT_SIG: 
        {
            switch (((MenuSelectEvt*)e)->item_index)
            {
                case 0:
                    return Q_TRAN(&EOSConsole::MainMenu);
                
                case 1:  // Aperture
                      return Q_TRAN(&EOSConsole::ChangeApertureMenu);
                case 2:  // Shutter Speed
                      return Q_TRAN(&EOSConsole::ChangeShutterSpeedMenu);
                case 3:  // White Balance
                      return Q_TRAN(&EOSConsole::ChangeWBMenu);
                case 4:  // Picture Style
                      return Q_TRAN(&EOSConsole::ChangePStyleMenu);
                case 5:  // ISO
                      return Q_TRAN(&EOSConsole::ChangeIsoMenu);
                case 6:  // Exposure Compensation
                      return Q_TRAN(&EOSConsole::ChangeExpCompMenu);
            } // switch
        }
    }
    return Q_SUPER(&EOSConsole::Active);
}

QState EOSConsole::ChangeApertureMenu(EOSConsole *me, QEvent const *e) 
{
    switch (e->sig) 
    {
        case Q_ENTRY_SIG:
            PrintMenuTitles(2, menuUpDown);
            return Q_HANDLED();
        case MENU_SELECT_SIG: 
        {
            uint8_t new_value;
            
            switch (((MenuSelectEvt*)e)->item_index)
            {
            case 0:
                return Q_TRAN(&EOSConsole::ChangeSettingsMenu);
            
            case 2:
                if (vlAperture.GetSize() > 0)
                {
                    new_value = vlAperture.GetNext(dpAperture, 1); 
                    Eos.SetProperty(EOS_DPC_Aperture, new_value);
                }
                return Q_HANDLED();
            case 1:
                if (vlAperture.GetSize() > 0)
                {
                    new_value = vlAperture.GetPrev(dpAperture, 1); 
                    Eos.SetProperty(EOS_DPC_Aperture, new_value);
                }
                return Q_HANDLED();
            } // switch (((MenuSelectEvt*)e)->item_index)
        } // case MENU_SELECT_SIG:
    }
    return Q_SUPER(&EOSConsole::Active);
}

QState EOSConsole::ChangeShutterSpeedMenu(EOSConsole *me, QEvent const *e) 
{
    switch (e->sig) 
    {
        case Q_ENTRY_SIG:
            PrintMenuTitles(2, menuUpDown);
            return Q_HANDLED();
        case MENU_SELECT_SIG: 
        {
            uint8_t new_value;
            
            switch (((MenuSelectEvt*)e)->item_index)
            {
            case 0:
                return Q_TRAN(&EOSConsole::ChangeSettingsMenu);
            
            case 2:
                if (vlShutterSpeed.GetSize() > 0)
                {
                    new_value = vlShutterSpeed.GetNext(dpShutterSpeed, 1); 
                    Eos.SetProperty(EOS_DPC_ShutterSpeed, new_value);
                }
                return Q_HANDLED();
            case 1:
                if (vlShutterSpeed.GetSize() > 0)
                {
                    new_value = vlShutterSpeed.GetPrev(dpShutterSpeed, 1); 
                    Eos.SetProperty(EOS_DPC_ShutterSpeed, new_value);
                }
                return Q_HANDLED();
            } // switch (((MenuSelectEvt*)e)->item_index)
        } // case MENU_SELECT_SIG:
    }
    return Q_SUPER(&EOSConsole::Active);
}

QState EOSConsole::ChangeWBMenu(EOSConsole *me, QEvent const *e) 
{
    switch (e->sig) 
    {
        case Q_ENTRY_SIG:
            PrintMenuTitles(2, menuUpDown);
            return Q_HANDLED();
        case MENU_SELECT_SIG: 
        {
            uint8_t new_value;
            
            switch (((MenuSelectEvt*)e)->item_index)
            {
            case 0:
                return Q_TRAN(&EOSConsole::ChangeSettingsMenu);
            
            case 2:
                if (vlWhiteBalance.GetSize() > 0)
                {
                    new_value = vlWhiteBalance.GetNext(dpWb, 1); 
                    Eos.SetProperty(EOS_DPC_WhiteBalance, new_value);
                }
                return Q_HANDLED();
            case 1:
                if (vlAperture.GetSize() > 0)
                {
                    new_value = vlWhiteBalance.GetPrev(dpWb, 1); 
                    Eos.SetProperty(EOS_DPC_WhiteBalance, new_value);
                }
                return Q_HANDLED();
            } // switch (((MenuSelectEvt*)e)->item_index)
        } // case MENU_SELECT_SIG:
    }
    return Q_SUPER(&EOSConsole::Active);
}

QState EOSConsole::ChangeIsoMenu(EOSConsole *me, QEvent const *e) 
{
    switch (e->sig) 
    {
        case Q_ENTRY_SIG:
            PrintMenuTitles(2, menuUpDown);
            return Q_HANDLED();
        case MENU_SELECT_SIG: 
        {
            uint8_t new_value;
            
            switch (((MenuSelectEvt*)e)->item_index)
            {
            case 0:
                return Q_TRAN(&EOSConsole::ChangeSettingsMenu);
            
            case 2:
                if (vlIso.GetSize() > 0)
                {
                    new_value = vlIso.GetNext(dpIso, 1); 
                    Eos.SetProperty(EOS_DPC_Iso, new_value);
                }
                return Q_HANDLED();
            case 1:
                if (vlAperture.GetSize() > 0)
                {
                    new_value = vlIso.GetPrev(dpIso, 1); 
                    Eos.SetProperty(EOS_DPC_Iso, new_value);
                }
                return Q_HANDLED();
            } // switch (((MenuSelectEvt*)e)->item_index)
        } // case MENU_SELECT_SIG:
    }
    return Q_SUPER(&EOSConsole::Active);
}

QState EOSConsole::ChangePStyleMenu(EOSConsole *me, QEvent const *e) 
{
    switch (e->sig) 
    {
        case Q_ENTRY_SIG:
            PrintMenuTitles(2, menuUpDown);
            return Q_HANDLED();
        case MENU_SELECT_SIG: 
        {
            uint8_t new_value;
            
            switch (((MenuSelectEvt*)e)->item_index)
            {
            case 0:
                return Q_TRAN(&EOSConsole::ChangeSettingsMenu);
            
            case 2:
                if (vlPictureStyle.GetSize() > 0)
                {
                    new_value = vlPictureStyle.GetNext(dpPStyle, 1); 
                    Eos.SetProperty(EOS_DPC_PictureStyle, new_value);
                }
                return Q_HANDLED();
            case 1:
                if (vlPictureStyle.GetSize() > 0)
                {
                    new_value = vlPictureStyle.GetPrev(dpPStyle, 1); 
                    Eos.SetProperty(EOS_DPC_PictureStyle, new_value);
                }
                return Q_HANDLED();
            } // switch (((MenuSelectEvt*)e)->item_index)
        } // case MENU_SELECT_SIG:
    }
    return Q_SUPER(&EOSConsole::Active);
}

QState EOSConsole::ChangeExpCompMenu(EOSConsole *me, QEvent const *e) 
{
    switch (e->sig) 
    {
        case Q_ENTRY_SIG:
            PrintMenuTitles(2, menuUpDown);
            return Q_HANDLED();
        case MENU_SELECT_SIG: 
        {
            uint8_t new_value;
            
            switch (((MenuSelectEvt*)e)->item_index)
            {
            case 0:
                return Q_TRAN(&EOSConsole::ChangeSettingsMenu);
            
            case 2:
                if (vlExpCompensation.GetSize() > 0)
                {
                    new_value = vlExpCompensation.GetNext(dpExpComp, 1); 
                    Eos.SetProperty(EOS_DPC_ExposureCompensation, new_value);
                }
                return Q_HANDLED();
            case 1:
                if (vlExpCompensation.GetSize() > 0)
                {
                    new_value = vlExpCompensation.GetPrev(dpExpComp, 1); 
                    Eos.SetProperty(EOS_DPC_ExposureCompensation, new_value);
                }
                return Q_HANDLED();
            } // switch (((MenuSelectEvt*)e)->item_index)
        } // case MENU_SELECT_SIG:
    }
    return Q_SUPER(&EOSConsole::Active);
}

//static EOSConsole          menu;
static TickEvt           tick_evt;
//static MenuSelectEvt     menu_sel_evt;

//............................................................................

//void setup()
//{
//    Serial.begin( 115200 );
//
//    menu.init();                                // take the initial transition
//    
//    tick_evt.sig = TICK_SIG;
//    tick_evt.fine_time = 0;
//}

int8_t EOSConsole::MenuSelect()
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

//void loop()
//{
//    delay(100);                                            // 100 ms delay
//
//    if (++tick_evt.fine_time == 10) 
//        tick_evt.fine_time = 0;
//
//    menu.dispatch(&tick_evt);                       // dispatch TICK event
//
//    int8_t  index = MenuSelect();
//    
//    if (index >= 0)
//    {
//        menu_sel_evt.sig         = MENU_SELECT_SIG;
//        menu_sel_evt.item_index  = index;
//        menu.dispatch(&menu_sel_evt);      // dispatch the event
//    }
//}

