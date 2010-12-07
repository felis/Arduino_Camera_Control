#if !defined(__SCREEN_H__)
#define __SCREEN_H__

#include <inttypes.h>
#include "screenitem.h"

#define MAX_SCREENS      12

class Screen;

static Screen    *screenPool2[MAX_SCREENS];

class Screen
{
    static uint8_t              currentScreen;
    static uint8_t              totalScreens;
    static bool                 screenChanged;
  
    ScreenItem                  *screenItems;
    const uint8_t               numItems;
      
public:
    Screen(const uint8_t num_items, ScreenItem *pItems);
    
    static void     Run(Max_LCD *pLcd);
    static void     Set(uint8_t screen_id);
    static uint8_t  Get() { return currentScreen; };
    
    void Update(Max_LCD *pLcd, bool initial);
};

#endif // __SCREEN_H__
