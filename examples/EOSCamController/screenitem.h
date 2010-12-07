#if !defined(__SCREENITEM_H__)
#define __SCREENITEM_H__

#include <inttypes.h>
#include <Max_LCD.h>
#include "dataitem.h"

struct ScreenItemAttributes
{
    uint16_t  bmLeft         : 4;
    uint16_t  bmTop          : 2;
    uint16_t  bmLen          : 5;
    uint16_t  bmHighlighted  : 1;
    uint16_t  bmHlChanged    : 1;
    uint16_t  bmPgmString    : 1;
};

class ScreenItem
{
      ScreenItemAttributes    itemAttribs;
      void                    *dataItem;
      
      void Print(Max_LCD *p);
      
      void SetAttribs(uint8_t left, uint8_t top, uint8_t len, bool highlighted, bool pgm);
      
public:
    ScreenItem(uint8_t left, uint8_t top, uint8_t len, bool highlighted, DataItemBase *item);
    ScreenItem(uint8_t left, uint8_t top, uint8_t len, bool highlighted, const char *item);
    
    void Update(Max_LCD *pLcd, bool initial = true);
    void Highlight(bool on) { itemAttribs.bmHighlighted = (on) ? 1 : 0; itemAttribs.bmHlChanged = 1; };
    bool IsHighlighted() { return (itemAttribs.bmHighlighted == 1); };
};

#endif
