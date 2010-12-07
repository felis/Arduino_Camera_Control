#include <Max3421e.h>
#include "controls.h"

StateMachine* StateMachine::currentState = NULL;

void GPInRegister::CheckControls()
{
    int8_t           enc_states[]    = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
    ControlStates    previous        = controlStates;
                     controlStates   = (pMax->gpioRd() & (GPIN_ENCODER_MASK | GPIN_ENC_BUT_MASK | GPIN_EXT_BUT_MASK));
    ControlStates    changes         = (previous ^ controlStates);
    
    if (previous == controlStates)
        return;
        
    ControlEvents *state_machine = (ControlEvents*)StateMachine::GetState();
    
    if ((changes & GPIN_ENCODER_MASK))
    {
        uint8_t  encoder = (previous & GPIN_ENCODER_MASK);
        encoder     <<= 2;
        encoder     |= (controlStates & GPIN_ENCODER_MASK);
        encoderValue += enc_states[(encoder & 0x0f)];
        
        if (encoderValue > 3 || encoderValue < -3)
        {
            if (state_machine)
                state_machine->OnEncoderChanged(encoderValue >> 2);
            encoderValue = 0;
        }
    }
    if (!state_machine)
        return;
        
    if ((changes & GPIN_ENC_BUT_MASK))
    {
        if ((controlStates & GPIN_ENC_BUT_MASK))
            state_machine->OnEncButtonUp();
        else
            state_machine->OnEncButtonDown();
    }
    if ((changes & GPIN_EXT_BUT_MASK))
    {
        if ((controlStates & GPIN_EXT_BUT_MASK))
            state_machine->OnExtButtonUp();
        else
            state_machine->OnExtButtonDown();
    }
}

