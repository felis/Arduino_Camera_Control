#include "screenitem.h"

void ScreenItem::SetAttribs(uint8_t left, uint8_t top, uint8_t len, bool highlighted, bool pgm)
{
    itemAttribs.bmLeft        = left; 
    itemAttribs.bmTop         = top; 
    itemAttribs.bmLen         = len; 
    itemAttribs.bmHighlighted = (highlighted) ? 1 : 0; 
    itemAttribs.bmHlChanged   = 1;                        // item highlighting changed flag
    itemAttribs.bmPgmString   = (pgm) ? 1 : 0;
};

ScreenItem::ScreenItem(uint8_t left, uint8_t top, uint8_t len, bool highlighted, DataItemBase *item) : 
    dataItem(item) 
{ 
    SetAttribs(left, top, len, highlighted, 0);
}

ScreenItem::ScreenItem(uint8_t left, uint8_t top, uint8_t len, bool highlighted, const char *item) : 
    dataItem((void*)item) 
{ 
    SetAttribs(left, top, len, highlighted, 1);
}

void ScreenItem::Print(Max_LCD *p)
{
    char*     str;
    bool      is_pgm = false;
    
    if (itemAttribs.bmPgmString == 1)
    {
        is_pgm = true;
        str = (char*)dataItem;
    }
    else
        ((DataItemBase*)dataItem)->GetText(&str, is_pgm);
    
    if(!str)
        return;
    
    uint8_t    cnt = 0;
    
    if (is_pgm)
    {
        char c;
        while((c = pgm_read_byte(str++)) && cnt < itemAttribs.bmLen)
        {
            p->print(c,BYTE);
            cnt ++;
        }
    }
    else
    {
        char *pc = str;
        while(*pc && cnt < itemAttribs.bmLen)
        {
            p->print(*pc++,BYTE);
            cnt ++;
        }
    }
}

void ScreenItem::Update(Max_LCD *pLcd, bool initial)
{
    if (initial || ( itemAttribs.bmPgmString == 0 && ((DataItemBase*)dataItem)->IsUpdated() ) || itemAttribs.bmHlChanged)
    {
        pLcd->home();
        
        if (itemAttribs.bmLeft > 0)
        {
            pLcd->setCursor(itemAttribs.bmLeft-1, itemAttribs.bmTop);
            
            pLcd->print((itemAttribs.bmHighlighted) ? '>' : ' ', BYTE);
        }
        else
            pLcd->setCursor(itemAttribs.bmLeft, itemAttribs.bmTop);
            
        Print(pLcd);
        
        if (itemAttribs.bmLeft + itemAttribs.bmLen < 16)
            pLcd->print((itemAttribs.bmHighlighted) ? '<' : ' ', BYTE);
        
        if (itemAttribs.bmPgmString == 0)
            ((DataItemBase*)dataItem)->SetUpdated(false);
            
        itemAttribs.bmHlChanged = 0;
    }
}

