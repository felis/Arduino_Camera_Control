#if !defined(__EOSCONSOLE_H__)
#define __EOSCONSOLE_H__

#include <inttypes.h>
#include <avr/pgmspace.h>
#include <qep_port.h>
#include <WProgram.h>
#include <canoneos.h>

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

class EOSConsole : public QHsm 
{
    static void PrintMenuTitles(uint8_t count, const char **menu);
    static void ShowParams();
  
public:
    EOSConsole()
        : QHsm((QStateHandler)&EOSConsole::Initial)
    {};
    int8_t MenuSelect();

protected:
    static QState Initial(EOSConsole *me, QEvent const *e);
    static QState Inactive(EOSConsole *me, QEvent const *e);
    static QState Active(EOSConsole *me, QEvent const *e);
    static QState MainMenu(EOSConsole *me, QEvent const *e);
    static QState ChangeSettingsMenu(EOSConsole *me, QEvent const *e);
    static QState ChangeApertureMenu(EOSConsole *me, QEvent const *e);
    static QState ChangeShutterSpeedMenu(EOSConsole *me, QEvent const *e);
    static QState ChangeWBMenu(EOSConsole *me, QEvent const *e);
    static QState ChangePStyleMenu(EOSConsole *me, QEvent const *e);
    static QState ChangeExpCompMenu(EOSConsole *me, QEvent const *e);
    static QState ChangeIsoMenu(EOSConsole *me, QEvent const *e);
};

#endif // __EOSCONSOLE_H__
