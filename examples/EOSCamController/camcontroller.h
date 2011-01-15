/* Camera controller header */
#ifndef __CAMCONTROLLER_H__
#define __CAMCONTROLLER_H__

#include <inttypes.h>
#include <avr/pgmspace.h>

#include <canoneos.h>
#include <eosvaluetitles.h>
#include "menu.h"

const char msgSetSelf[]     PROGMEM = "SELF";
const char msgSetFrames[]   PROGMEM = "FRMS";
const char msgSetBkt[]      PROGMEM = "BKT";
const char msgSetInterval[] PROGMEM = "INT";
const char msgExit[]        PROGMEM = "EXIT";
const char msgSetRun[]      PROGMEM = "RUN ";
const char msgAbort[]       PROGMEM = "ABORT";
const char msgLeft[]        PROGMEM = "LEFT";

const char msgError[]       PROGMEM = "ERROR:";
const char msgCamera[]      PROGMEM = "CAMERA";
const char msgDisconnected[] PROGMEM = "DISCONNECTED";

const char msgSelfTimer[]   PROGMEM = "SELF TIMER h/m/s";
const char msgBracketing[]  PROGMEM = "BKT(RANGE/STEP)";
const char msgSteps[]       PROGMEM = "steps";
const char msgCntFrames[]   PROGMEM = "FRAMES";
const char msgIntTimer[]    PROGMEM = "INT TIMER h/m/s";
const char msgIntervalometer[] PROGMEM = "INTERVAL";
const char msgSettings[]    PROGMEM = "SETTINGS";
const char msgClock[]       PROGMEM = "CLOCK";
const char msgEV[]          PROGMEM = "EV";

typedef KeyValuePairDataItem<VT_EXPCOMP, VT_EXPCOMP_COUNT, VT_EXPCOMP_TEXT_LEN>    EXP_COMP_DATA_ITEM;            // Exposure compensation data item type
typedef SRAMValueList<VT_EXPCOMP, 16>                                              BKT_STEP_VALUE_LIST;           // Bracketing step value list type

//--- Data Item Types ---------------------------------------------------------
typedef KeyValuePairDataItem<VT_MODE, VT_MODE_COUNT, VT_MODE_TEXT_LEN>             DIT_MODE;
typedef KeyValuePairDataItem<VT_APERTURE, VT_APT_COUNT, VT_APT_TEXT_LEN>           DIT_APERTURE;
typedef KeyValuePairDataItem<VT_WB, VT_WB_COUNT, VT_WB_TEXT_LEN>                   DIT_WB;
typedef KeyValuePairDataItem<VT_SHSPEED, VT_SHSPEED_COUNT, VT_SHSPEED_TEXT_LEN>    DIT_SHUTTER_SPEED;
typedef KeyValuePairDataItem<VT_PSTYLE, VT_PSTYLE_COUNT, VT_PSTYLE_TEXT_LEN>       DIT_PSTYLE;
typedef KeyValuePairDataItem<VT_ISO, VT_ISO_COUNT, VT_ISO_TEXT_LEN>                DIT_ISO;
typedef KeyValuePairDataItem<VT_EXPCOMP, VT_EXPCOMP_COUNT, VT_EXPCOMP_TEXT_LEN>    DIT_EXPCOMP;
typedef IntDataItem<uint8_t, 3>                                                    DIT_TIMER_DIGIT_PAIR;


#endif //__CAMCONTROLLER_H__
