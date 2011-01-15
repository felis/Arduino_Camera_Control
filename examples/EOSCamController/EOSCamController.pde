/* Digital camera controller board test sketch */
//#include <Spi.h>
#include <Max3421e.h>
#include <Usb.h>
#include <Max_LCD.h>
#include <simpletimer.h>
#include <valuelist.h>
#include <canoneos.h>
#include <qep_port.h>

#include "camcontroller.h"
#include "controls.h"
#include "eoseventparser.h"
#include "dataitem.h"
#include "screenitem.h"
#include "screen.h"
#include "menu.h"
#include "hdrcapture.h"

#define DEV_ADDR        1

// Canon EOS 400D
#define DATA_IN_EP      1
#define DATA_OUT_EP     2
#define INTERRUPT_EP    3
#define CONFIG_NUM      1

#define EEP_APERTURE_LIST_OFFSET    0
#define EEP_APERTURE_LIST_SIZE      32

#define EEP_SHTSPEED_LIST_OFFSET    (EEP_APERTURE_LIST_OFFSET + EEP_APERTURE_LIST_SIZE)
#define EEP_SHTSPEED_LIST_SIZE      64

#define EEP_WBALANCE_LIST_OFFSET    (EEP_SHTSPEED_LIST_OFFSET + EEP_SHTSPEED_LIST_SIZE)
#define EEP_WBALANCE_LIST_SIZE      12

#define EEP_PICSTYLE_LIST_OFFSET    (EEP_WBALANCE_LIST_OFFSET + EEP_WBALANCE_LIST_SIZE)
#define EEP_PICSTYLE_LIST_SIZE      12

#define EEP_EXPOCOR_LIST_OFFSET     (EEP_PICSTYLE_LIST_OFFSET + EEP_PICSTYLE_LIST_SIZE)
#define EEP_EXPOCOR_LIST_SIZE       48

#define EEP_ISO_LIST_OFFSET         (EEP_EXPOCOR_LIST_OFFSET + EEP_EXPOCOR_LIST_SIZE)
#define EEP_ISO_LIST_SIZE           8

EEPROMByteList          vlAperture(EEP_APERTURE_LIST_OFFSET, EEP_APERTURE_LIST_SIZE);
EEPROMByteList          vlShutterSpeed(EEP_SHTSPEED_LIST_OFFSET, EEP_SHTSPEED_LIST_SIZE);
EEPROMByteList          vlWhiteBalance(EEP_WBALANCE_LIST_OFFSET, EEP_WBALANCE_LIST_SIZE);
EEPROMByteList          vlPictureStyle(EEP_PICSTYLE_LIST_OFFSET, EEP_PICSTYLE_LIST_SIZE);
EEPROMByteList          vlIso(EEP_ISO_LIST_OFFSET, EEP_ISO_LIST_SIZE);
EEPROMByteList          vlExpCompensation(EEP_EXPOCOR_LIST_OFFSET, EEP_EXPOCOR_LIST_SIZE);

class CamStateHandlers : public EOSStateHandlers
{
      bool stateConnected;
    
public:
      CamStateHandlers() : stateConnected(false) {};
      
      virtual void OnDeviceDisconnectedState(PTP *ptp);
      virtual void OnDeviceInitializedState(PTP *ptp);
};

class CamHDRCapture : public HDRCapture
{
public:  
    CamHDRCapture(CanonEOS &eos) : HDRCapture(eos) {};
protected:  
    virtual void OnFrameCaptured(uint16_t left); 
    virtual void OnSelfTimerProgress(uint32_t left);
    virtual void OnIntrTimerProgress(uint32_t left);
};

CamStateHandlers  CamStates;
Max_LCD               LCD;
SimpleTimer           ControlTimer, PTPPollTimer;

CanonEOS              Eos(DEV_ADDR, DATA_IN_EP, DATA_OUT_EP, INTERRUPT_EP, CONFIG_NUM, &CamStates);
CamHDRCapture         hdrCapture(Eos);
GPInRegister          ExtControls(Eos.GetMax());
QEvent                evtTick, evtAbort;

//--- (0) Message Screen ----------------------------------------------------------------------
PgmStringDataItem     diFirst(msgCamera);
PgmStringDataItem     diSecond(msgDisconnected);

ScreenItem            siFirst     (5, 0, 16, false, &diFirst);
ScreenItem            siSecond    (2, 1, 16, false, &diSecond);

ScreenItem            *messageScreenItems[]  PROGMEM = { &siFirst, &siSecond };
Screen                messageScreen(2, (ScreenItem*)messageScreenItems);

//--- (1) Main Menu Screen --------------------------------------------------------------------
ScreenItem            siIntervalometer (1,  0, 8, true,  (const char*)&msgIntervalometer);
ScreenItem            siSettings       (1,  1, 8, false, (const char*)&msgSettings);

ScreenItem            *mainMenuScreenItems[]  PROGMEM = { &siIntervalometer, &siSettings };
Screen                scrMainMenu(2, (ScreenItem*)mainMenuScreenItems);

//--- (2) Timer Menu Screen -------------------------------------------------------------------
ScreenItem            siSelf     (1,  0, 4, true,  (const char*)&msgSetSelf);
ScreenItem            siFrames   (1,  1, 4, false, (const char*)&msgSetFrames);
ScreenItem            siBkt      (6,  0, 3, false, (const char*)&msgSetBkt);
ScreenItem            siInterval (6,  1, 3, false, (const char*)&msgSetInterval);
ScreenItem            siRun      (10, 0, 4, false, (const char*)&msgSetRun);
ScreenItem            siExit     (10, 1, 4, false, (const char*)&msgExit);

ScreenItem            *timerSettingsScreenItems[]  PROGMEM = { &siSelf, &siFrames, &siBkt, &siInterval, &siRun, &siExit };
Screen                timerSettingsScreen(6, (ScreenItem*)timerSettingsScreenItems);

//--- (3) Self Timer Screen -------------------------------------------------------------------
DIT_TIMER_DIGIT_PAIR  diHourSelf(0),  diHourInt(0),
                      diMinSelf(0),   diMinInt(0),
                      diSecSelf(0),   diSecInt(0);

ScreenItem            siSelfTimer(0, 0, 16, false,  (const char*)&msgSelfTimer);
ScreenItem            siHourSelf(3, 1, 2, false, &diHourSelf);
ScreenItem            siMinSelf (6, 1, 2, false, &diMinSelf);
ScreenItem            siSecSelf (9, 1, 2, false, &diSecSelf);

ScreenItem            *scitmSelfTimerSet[]  PROGMEM = { &siSelfTimer, &siHourSelf, &siMinSelf, &siSecSelf };
Screen                scrSelfTimerSet(4, (ScreenItem*)scitmSelfTimerSet);

//--- (4) Number of Frames Screen -------------------------------------------------------------
IntDataItem<uint16_t, 5>         diFramesCount(0);
IntDataItem<uint16_t, 5>         diFramesLeft(0);

ScreenItem            siFramesText  (5, 0,  6, false, (const char*)&msgCntFrames);
ScreenItem            siFramesCount (6, 1,  4, false, &diFramesCount);

ScreenItem            *scitmFramesSet[]  PROGMEM = { &siFramesText, &siFramesCount };
Screen                scrFramesSet(2, (ScreenItem*)scitmFramesSet);

//--- (5) Bracketing Screen -------------------------------------------------------------------
KeyValuePairDataItem<uint8_t, 37, 7>      diBktEV(0, ExpCompTitles);
KeyValuePairDataItem<uint8_t, 37, 7>      diBktStep(0, ExpCompTitles);
uint8_t                                   nBktStep;
uint8_t                                   nBktNegativeIndex;
uint8_t                                   nBktPositiveIndex;

ScreenItem            siBracketing(0, 0, 15, false,  (const char*)&msgBracketing);
ScreenItem            siBktEV     (1, 1,  6, false,  &diBktEV);
ScreenItem            siBktStep   (9, 1,  6, false,  &diBktStep);

ScreenItem            *scitmBkt[]  PROGMEM = { &siBracketing, &siBktEV, &siBktStep };
Screen                scrBktSet(3, (ScreenItem*)scitmBkt);

//--- (6) Interval Timer Screen ---------------------------------------------------------------
ScreenItem            siIntTimer(0, 0, 16, false,  (const char*)&msgIntTimer);
ScreenItem            siHourInt(3, 1, 2, false, &diHourInt);
ScreenItem            siMinInt (6, 1, 2, false, &diMinInt);
ScreenItem            siSecInt (9, 1, 2, false, &diSecInt);

ScreenItem            *scitmIntTimerSet[]  PROGMEM = { &siIntTimer, &siHourInt, &siMinInt, &siSecInt };
Screen                scrIntTimerSet(4, (ScreenItem*)scitmIntTimerSet);

//--- (7) Run Screen ---------------------------------------------------------------------------
TimeSpanDataItem         diLeftTimer(0);
TimeSpanDataItem         diIntTimer(0);

ScreenItem            siRunLeftTime( 0, 0, 8, false,  &diLeftTimer);
ScreenItem            siRunIntTime( 0, 1, 8, false,  &diIntTimer);
ScreenItem            siRunFramesLeft ( 10, 0, 4, false,  &diFramesLeft);
ScreenItem            siAbort(10, 1, 5, false,  (const char*)&msgAbort);

ScreenItem            *scitmRun[]  PROGMEM = { &siRunLeftTime, &siRunIntTime, &siRunFramesLeft, &siAbort };
Screen                scrRun(4, (ScreenItem*)scitmRun);

//--- (8) Camera Settings Screen ---------------------------------------------------------------
DIT_MODE              diMode(0, ModeTitles);
DIT_APERTURE          diAperture(0, ApertureTitles);
DIT_WB                diWb(0, WbTitles);
DIT_SHUTTER_SPEED     diShutterSpeed(0, ShutterSpeedTitles);
DIT_PSTYLE            diPStyle(0, PStyleTitles);
DIT_ISO               diIso(0, IsoTitles);
DIT_EXPCOMP           diExpComp(0, ExpCompTitles);

ScreenItem            siMode        ( 0, 0, 3, false,  &diMode);
ScreenItem            siAperture    ( 0, 1, 3, false,  &diAperture);
ScreenItem            siWb          ( 4, 0, 3, false,  &diWb);
ScreenItem            siShutterSpeed( 4, 1, 4, false,  &diShutterSpeed);
ScreenItem            siPStyle      ( 8, 0, 3, false,  &diPStyle);
ScreenItem            siIso         (12, 0, 4, false,  &diIso);
ScreenItem            siExpComp     ( 9, 1, 6, false,  &diExpComp);

ScreenItem            *scitmSettings[]  PROGMEM = { &siMode, &siAperture, &siWb, &siShutterSpeed, &siPStyle, &siIso, &siExpComp };
Screen                scrSettings(7, (ScreenItem*)scitmSettings);

IntSpin<DIT_TIMER_DIGIT_PAIR, uint8_t>    hourSpinSelf(0, 99, 1, &diHourSelf, NULL);
IntSpin<DIT_TIMER_DIGIT_PAIR, uint8_t>    minSpinSelf(0, 99, 1, &diMinSelf, NULL);
IntSpin<DIT_TIMER_DIGIT_PAIR, uint8_t>    secSpinSelf(0, 99, 1, &diSecSelf, NULL);

IntSpin<DIT_TIMER_DIGIT_PAIR, uint8_t>    hourSpinInt(0, 99, 1, &diHourInt, NULL);
IntSpin<DIT_TIMER_DIGIT_PAIR, uint8_t>    minSpinInt(0, 99, 1, &diMinInt, NULL);
IntSpin<DIT_TIMER_DIGIT_PAIR, uint8_t>    secSpinInt(0, 99, 1, &diSecInt, NULL);


void SpinSetAperture(DataItemBase *data_item)
{
    Eos.SetProperty(EOS_DPC_Aperture, ((DIT_APERTURE*)data_item)->Get());
};

void SpinSetShutterSpeed(DataItemBase *data_item)
{
    Eos.SetProperty(EOS_DPC_ShutterSpeed, ((DIT_SHUTTER_SPEED*)data_item)->Get());
};

void SpinSetWb(DataItemBase *data_item)
{
    Eos.SetProperty(EOS_DPC_WhiteBalance, ((DIT_WB*)data_item)->Get());
};

void SpinSetPStyle(DataItemBase *data_item)
{
    Eos.SetProperty(EOS_DPC_PictureStyle, ((DIT_PSTYLE*)data_item)->Get());
};

void SpinSetIso(DataItemBase *data_item)
{
    Eos.SetProperty(EOS_DPC_Iso, ((DIT_ISO*)data_item)->Get());
};

void SpinSetExpComp(DataItemBase *data_item)
{
    Eos.SetProperty(EOS_DPC_ExposureCompensation, ((DIT_EXPCOMP*)data_item)->Get());
};

void SpinUpdateBktStepValues(DataItemBase *data_item);
void SpinUpdateBktStep(DataItemBase *data_item);

EEPROMListIntSpin<DIT_APERTURE, VT_APERTURE>         spinAperture(&vlAperture, &diAperture, &SpinSetAperture);
EEPROMListIntSpin<DIT_SHUTTER_SPEED, VT_SHSPEED>     spinShutterSpeed(&vlShutterSpeed, &diShutterSpeed, &SpinSetShutterSpeed);
EEPROMListIntSpin<DIT_WB, VT_WB>                     spinWb(&vlWhiteBalance, &diWb, &SpinSetWb);
EEPROMListIntSpin<DIT_PSTYLE, VT_PSTYLE>             spinPStyle(&vlPictureStyle, &diPStyle, &SpinSetPStyle);
EEPROMListIntSpin<DIT_ISO, VT_ISO>                   spinIso(&vlIso, &diIso, &SpinSetIso);

EEPROMListIntSpin<EXP_COMP_DATA_ITEM, VT_EXPCOMP>    spinExpComp(&vlExpCompensation, &diExpComp, &SpinSetExpComp);
EEPROMListIntSpin<EXP_COMP_DATA_ITEM, VT_EXPCOMP>    spinBktEV  (&vlExpCompensation, &diBktEV, &SpinUpdateBktStepValues);

BKT_STEP_VALUE_LIST  vlExpCompStep;

SRAMListIntSpin<EXP_COMP_DATA_ITEM, VT_EXPCOMP, BKT_STEP_VALUE_LIST>      spinBktStep(&vlExpCompStep, &diBktStep, &SpinUpdateBktStep);


void SpinUpdateBktStepValues(DataItemBase *data_item)
{
    uint8_t cur_value = ((EXP_COMP_DATA_ITEM*)data_item)->Get();
    
    vlExpCompStep.SetSize(0);
    
    // Check value for zerro. Exit on zerro.
    if (cur_value == 0)
        return;
    
    // Calculate negative and positive values of expo compensation
    uint8_t negative_value = (cur_value & 0x80) ? cur_value : ~(cur_value - 1);
    uint8_t positive_value = (cur_value & 0x80) ? ~(cur_value - 1) : cur_value;
    
    // Get indices of negative and positive expo compensation values
    uint16_t negative_index = vlExpCompensation.GetValueIndex(negative_value);
    uint16_t positive_index = vlExpCompensation.GetValueIndex(positive_value);

    nBktNegativeIndex = negative_index;
    nBktPositiveIndex = positive_index;
    
    // Calculate interval length
    uint16_t len = positive_index - negative_index;

    // Calculate zerro value index
    uint16_t zerro_index = vlExpCompensation.GetValueIndex(0);

    // Calculate positive index offset
    uint16_t zerro_based_offset = positive_index - zerro_index;
     
   // Calculate all possible interval indices
    for (uint16_t i = zerro_based_offset, j = positive_index; i>0; i--, j--)
    {
        // Insert values into the list
        if (len % i == 0)
            vlExpCompStep.Append(vlExpCompensation.Get(j));
    }
    diBktStep.Set(cur_value);
    diBktStep.SetUpdated(true);
};

void SpinUpdateBktStep(DataItemBase *data_item)
{
    uint8_t cur_value = ((EXP_COMP_DATA_ITEM*)data_item)->Get();
    
    nBktStep = vlExpCompensation.GetValueIndex(cur_value) - vlExpCompensation.GetValueIndex(0);
};

void MenuExit();
void MenuSetAperture();
void MenuSetShutterSpeed();
void MenuSetWb();
void MenuSetPStyle();
void MenuSetIso();
void MenuSetExpComp();

extern Menu mainMenu;

//--- Camera Settings Menu -----------------------------------------------------------------
MenuItem              settingsMenuItems[] = { {&siMode, &MenuExit}, {&siAperture, &MenuSetAperture}, {&siWb, &MenuSetWb}, {&siShutterSpeed, &MenuSetShutterSpeed}, {&siPStyle, &MenuSetPStyle}, {&siIso, &MenuSetIso}, {&siExpComp, &MenuSetExpComp} };
Menu                  settingsMenu(8, 7, settingsMenuItems, 0, &mainMenu);

void MenuSetAperture() { spinAperture.SetReturnState(&settingsMenu); StateMachine::SetState(&spinAperture); };
void MenuSetShutterSpeed() { spinShutterSpeed.SetReturnState(&settingsMenu); StateMachine::SetState(&spinShutterSpeed); };
void MenuSetWb(){ spinWb.SetReturnState(&settingsMenu); StateMachine::SetState(&spinWb); };
void MenuSetPStyle(){ spinPStyle.SetReturnState(&settingsMenu); StateMachine::SetState(&spinPStyle); };
void MenuSetIso(){ spinIso.SetReturnState(&settingsMenu); StateMachine::SetState(&spinIso); };
void MenuSetExpComp(){ spinExpComp.SetReturnState(&mainMenu); StateMachine::SetState(&spinExpComp); };

//--- Self Timer Menu ----------------------------------------------------------------------
void MenuSelfSetH();
void MenuSelfSetM();
void MenuSelfSetS();

extern Menu timerSettingsMenu;

MenuItem              selfSetMenuItems[] = { {&siHourSelf, &MenuSelfSetH}, {&siMinSelf, &MenuSelfSetM}, {&siSecSelf, &MenuSelfSetS} };
Menu                  selfSetMenu(3, 3, selfSetMenuItems, 0, &timerSettingsMenu);

void MenuSelfSetH() { hourSpinSelf.SetReturnState(&selfSetMenu); StateMachine::SetState(&hourSpinSelf); };
void MenuSelfSetM() { minSpinSelf.SetReturnState(&selfSetMenu); StateMachine::SetState(&minSpinSelf); };

//--- Timer Settings Menu ------------------------------------------------------------------
void MenuSelf();
void MenuBkt();
void MenuFrames();
void MenuInterval();
void MenuExit();
void MenuRun();

MenuItem              timerSettingsMenuItems[] = { {&siSelf, &MenuSelf}, {&siFrames, &MenuFrames}, {&siBkt, &MenuBkt}, {&siInterval, &MenuInterval}, {&siRun, &MenuRun}, {&siExit, &MenuExit} };
Menu                  timerSettingsMenu(2, 6, timerSettingsMenuItems, 0, &mainMenu);

void MenuSelfSetS() { secSpinSelf.SetReturnState(&timerSettingsMenu); StateMachine::SetState(&secSpinSelf); };

void MenuSelf()     { StateMachine::SetState(&selfSetMenu); };

void MenuBktSetStep();
void MenuBktStepExit();

MenuItem              bktSetMenuItems[] = { {&siBktEV, &MenuBktSetStep}, {&siBktStep, &MenuBktStepExit} }; 
Menu                  bktSetMenu(5, 2, bktSetMenuItems, 0, &timerSettingsMenu);

void MenuBktSetStep() 
{ 
    spinBktEV.SetReturnState(&bktSetMenu); 

    if (vlExpCompensation.GetSize())
        StateMachine::SetState(&spinBktEV); 
};

void MenuBktStepExit()
{ 
    spinBktStep.SetReturnState(&timerSettingsMenu); 

    if (vlExpCompensation.GetSize())
        StateMachine::SetState(&spinBktStep); 
};

void MenuBkt()      
{ 
    if (vlExpCompensation.GetSize())
        StateMachine::SetState(&bktSetMenu); 
};


IntSpin<IntDataItem<uint16_t, 5>, int16_t>  framesSpin(0, 9999, 1, &diFramesCount, NULL);

void MenuFrames()   { framesSpin.SetReturnState(&timerSettingsMenu); Screen::Set(4); StateMachine::SetState(&framesSpin); };

void MenuIntSetH();
void MenuIntSetM();
void MenuIntSetS();

MenuItem              intSetMenuItems[] = { {&siHourInt, &MenuIntSetH}, {&siMinInt, &MenuIntSetM}, {&siSecInt, &MenuIntSetS} };
Menu                  intSetMenu(6, 3, intSetMenuItems, 0, &timerSettingsMenu);

void MenuIntSetH() { hourSpinInt.SetReturnState(&intSetMenu); StateMachine::SetState(&hourSpinInt); };
void MenuIntSetM() { minSpinInt.SetReturnState(&intSetMenu); StateMachine::SetState(&minSpinInt); };
void MenuIntSetS() { secSpinInt.SetReturnState(&timerSettingsMenu); StateMachine::SetState(&secSpinInt); };

void MenuInterval() { StateMachine::SetState(&intSetMenu); };

void MenuRunAbort() 
{ 
    hdrCapture.PostEvent(&evtAbort);    
    diFramesLeft.Set(0);
    StateMachine::SetState(&timerSettingsMenu); 
};

MenuItem              runMenuItems[] = { {&siAbort, &MenuRunAbort} };
Menu                  runMenu(7, 1, runMenuItems, 0);

void MenuRun()      
{ 
    if (!diFramesCount.Get())
        return;
        
    uint32_t  intr_timeout = ((uint32_t)diHourInt.Get() * 3600 + (uint32_t)diMinInt.Get() * 60 + (uint32_t)diSecInt.Get());
    uint32_t  self_timeout = ((uint32_t)diHourSelf.Get() * 3600 + (uint32_t)diMinSelf.Get() * 60 + (uint32_t)diSecSelf.Get());

    diFramesLeft.Set(diFramesCount.Get());
    diLeftTimer.Set(self_timeout);
    diIntTimer.Set(intr_timeout);

    SetEvt  setEvt;
    
    setEvt.sig     = SET_FRAMES_SIG;
    setEvt.value   = diFramesCount.Get();
    
    hdrCapture.dispatch(&setEvt);
    
    setEvt.sig     = SET_FRAME_TIMEOUT_SIG;
    setEvt.value   = intr_timeout;
    
    hdrCapture.dispatch(&setEvt);
    
    setEvt.sig     = SET_SELF_TIMEOUT_SIG;
    setEvt.value   = self_timeout;
    
    hdrCapture.dispatch(&setEvt);
    
    SetBktEvt          setBktEvt;
    setBktEvt.sig       = SET_BRACKETING_SIG;
    setBktEvt.step      = nBktStep;
    setBktEvt.negative  = nBktNegativeIndex;
    setBktEvt.positive  = nBktPositiveIndex;
    
    hdrCapture.dispatch(&setBktEvt);
    
    StateMachine::SetState(&runMenu); 
    
    setEvt.sig     = RUN_SIG;
    hdrCapture.dispatch(&setEvt);
};

//--- Main Menu ----------------------------------------------------------------------------
void MenuIntervalometer() { StateMachine::SetState(&timerSettingsMenu); };

void MenuSettings() 
{ 
    StateMachine::SetState(&settingsMenu); 
};

MenuItem              mainMenuItems[] = { {&siIntervalometer, &MenuIntervalometer}, {&siSettings, &MenuSettings} };
Menu                  mainMenu(1, 2, mainMenuItems, 0);

void MenuExit()     { StateMachine::SetState(&mainMenu); };

class DummyMenu : public StateMachine
{
public:    
    virtual bool OnInitialState() 
    { 
        Screen::Set(0);
        return true; 
    };
} DisconnectedState;

void CamStateHandlers::OnDeviceDisconnectedState(PTP *ptp)
{
    if (stateConnected)
    {
        stateConnected = false;
        PTPPollTimer.Disable();
        StateMachine::SetState(&DisconnectedState); 
    }
}

void CamStateHandlers::OnDeviceInitializedState(PTP *ptp)
{
    if (!stateConnected)
    {
        stateConnected = true;
        PTPPollTimer.Enable();
        StateMachine::SetState(&mainMenu); 
    }
    hdrCapture.Run();
}

void CamHDRCapture::OnFrameCaptured(uint16_t left)
{
    diFramesLeft.Set(left);

    if (!left)
        StateMachine::SetState(&timerSettingsMenu); 
}

void CamHDRCapture::OnSelfTimerProgress(uint32_t left)
{
    diLeftTimer.Set(left);
}

void CamHDRCapture::OnIntrTimerProgress(uint32_t left)
{
    diIntTimer.Set(left);
}

void OnControlTimer()
{
    ExtControls.CheckControls();
    hdrCapture.PostEvent(&evtTick);
    Screen::Run(&LCD);
}

void OnPTPPollTimer()
{
    EOSEventParser    prs;
    Eos.EventCheck(&prs);
    diLeftTimer.SetUpdated(true);
    diIntTimer.SetUpdated(true);
}

void setup()
{
    StateMachine::SetState(&DisconnectedState);
 
#ifdef PTPDEBUG 
    Serial.begin(115200);
#endif

    Eos.Setup();
    delay( 200 );
  
    // set up the LCD's number of rows and columns: 
    LCD.begin(16, 2);
    LCD.clear();
    LCD.home();
    LCD.setCursor(0,0);
    
    PTPPollTimer.Set(OnPTPPollTimer, 300);
    
    // 1ms is the perfect interval for encoder polling
    ControlTimer.Set(OnControlTimer, 1);
    ControlTimer.Enable();
  
    evtTick.sig = TICK_MILLIS_SIG;
    evtAbort.sig = ABORT_SIG;
    hdrCapture.init();

#ifdef PTPDEBUG 
    Serial.println("Start");
#endif    
}

void loop()
{
    Eos.Task();
    PTPPollTimer.Run();
    ControlTimer.Run();
}
 
