#ifndef __CONTROLS_H__
#define __CONTROLS_H__

#include <inttypes.h>
#include <avr/pgmspace.h>

class StateMachine
{
    static StateMachine    *currentState;
    
protected:
    virtual bool OnInitialState() { return true; };

public:
    static void SetState(StateMachine *state) { currentState = state;  currentState->OnInitialState(); };
    static StateMachine* GetState() { return StateMachine::currentState; };
};

class ControlEvents : public StateMachine
{
public:
    virtual bool OnEncoderChanged(int8_t value) { return true; };
    virtual bool OnEncButtonUp()   { return true; };
    virtual bool OnEncButtonDown() { return true; };
    virtual bool OnExtButtonUp()   { return true; };
    virtual bool OnExtButtonDown() { return true; };
};

#define  GPIN_ENCODER_MASK    0x03
#define  GPIN_ENC_BUT_MASK    0x08
#define  GPIN_EXT_BUT_MASK    0x10

#define  GPIN_ENC_BUT_MASK    0x04
#define GPIN_EXT_BUT_MASK   0x08


typedef uint8_t ControlStates;

class GPInRegister
{
    ControlStates    controlStates;
    MAX3421E        *pMax;
    int8_t          encoderValue;
    
public:
    GPInRegister(MAX3421E *pmax) : controlStates(GPIN_ENCODER_MASK | GPIN_ENC_BUT_MASK | GPIN_EXT_BUT_MASK), pMax(pMax), encoderValue(0) {};
    void CheckControls();
};

#endif // __CONTROLS_H__
