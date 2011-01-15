#if !defined(__PSCONSOLE_H__)
#define __PSCONSOLE_H__

#include <inttypes.h>
#include <avr/pgmspace.h>
#include <qep_port.h>
#include <WProgram.h>
#include <canonps.h>

#include "ptpdpparser.h"

enum TextMenuSignals 
{
    MENU_SELECT_SIG = Q_USER_SIG,
    TICK_SIG
};

struct TickEvt : public QEvent 
{
    uint8_t fine_time;                              // the fine 1/10 s counter
};

struct MenuSelectEvt : public QEvent 
{
    uint8_t  item_index;
};

class PSConsole : public QHsm 
{
    static void PrintMenuTitles(uint8_t count, const char **menu);
    static void ShowParams();
  
public:
    PSConsole()
        : QHsm((QStateHandler)&PSConsole::Initial)
    {};
    int8_t MenuSelect();

protected:
    static QState Initial(PSConsole *me, QEvent const *e);
    static QState Inactive(PSConsole *me, QEvent const *e);
    static QState Active(PSConsole *me, QEvent const *e);
    static QState MainMenu(PSConsole *me, QEvent const *e);
    static QState ChangeSettingsMenu(PSConsole *me, QEvent const *e);
    static QState ChangeModeMenu(PSConsole *me, QEvent const *e);
    static QState ChangeApertureMenu(PSConsole *me, QEvent const *e);
    static QState ChangeShutterSpeedMenu(PSConsole *me, QEvent const *e);
    static QState ChangeWBMenu(PSConsole *me, QEvent const *e);
    static QState ChangeImageModeMenu(PSConsole *me, QEvent const *e);
    static QState ChangeExpCompMenu(PSConsole *me, QEvent const *e);
    static QState ChangeIsoMenu(PSConsole *me, QEvent const *e);
    static QState ChangeCamOutputMenu(PSConsole *me, QEvent const *e);
    static QState ChangeZoomMenu(PSConsole *me, QEvent const *e);
};

#endif // __PSCONSOLE_H__
