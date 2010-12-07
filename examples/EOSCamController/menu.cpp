#include "menu.h"

bool Menu::OnInitialState() 
{ 
    menuItems[itemSelected].screenItem->Highlight(true);
    Screen::Set(screenId);
    return true; 
};

bool Menu::OnEncoderChanged(int8_t value) 
{
      menuItems[itemSelected].screenItem->Highlight(false);
      
      int8_t new_val = (int8_t)itemSelected + value;
      
      if (new_val < 0)
          itemSelected = 0;
      else if (new_val >= numItems)
          itemSelected = numItems - 1;
      else
          itemSelected = (uint8_t)new_val;
          
      menuItems[itemSelected].screenItem->Highlight(true);

      return true;
};

bool Menu::OnEncButtonDown() 
{ 
      if (menuItems[itemSelected].ptrFunction)
          (menuItems[itemSelected].ptrFunction)();
};

bool Menu::OnExtButtonDown() 
{ 
      if (returnState)
          StateMachine::SetState(returnState);
};

