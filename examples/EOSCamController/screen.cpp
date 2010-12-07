#include "screen.h"

uint8_t              Screen::currentScreen   = 0;
uint8_t              Screen::totalScreens    = 0;
bool                 Screen::screenChanged   = true;

Screen::Screen(const uint8_t num_items, ScreenItem *pItems) : numItems(num_items)
{ 
    screenItems = (ScreenItem*)pgm_read_word(pItems);
    
    if (totalScreens < MAX_SCREENS) 
        screenPool2[totalScreens++] = this;
};

void Screen::Update(Max_LCD *pLcd, bool initial)
{
    for (uint8_t i=0; i<numItems; i++)
        screenItems[i].Update(pLcd, initial);
};

void Screen::Set(uint8_t screen_id)
{
    if (screen_id >= totalScreens)
        return;
        
    currentScreen = screen_id;
    screenChanged = true;
};

void Screen::Run(Max_LCD *pLcd)
{
    if (screenChanged)
    {
        pLcd->clear();
        pLcd->home();
    }
    if (screenPool2[currentScreen])
    {
        screenPool2[currentScreen]->Update(pLcd, (screenChanged) ? true : false);
        screenChanged = false;
    }
}

