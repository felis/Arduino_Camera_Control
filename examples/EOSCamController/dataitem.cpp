#include "dataitem.h"

char TimeSpanDataItem::textValue[9] = "";
char TimerDataItem::textValue[9] = "";

char* itoa2(int val, uint8_t buf_size, char *buf, int8_t base, char c)
{
    char sign = (val < 0) ? '-' : c;
    buf[buf_size-1] = 0;

    int8_t i = buf_size-2;
    	
    for(; val && i ; --i, val /= base)
        buf[i] = "0123456789abcdef"[(val < 0) ? -(val % base) : val % base];
    
    for (; i > 0; i--)
        buf[i] = c;
        
    buf[0] = sign;
    return buf;
}

char* itoa2(uint8_t val, uint8_t buf_size, char *buf, int8_t base, char c)
{
    buf[buf_size-1] = 0;

    int8_t i = buf_size-2;
    	
    for(; val && i>=0 ; --i, val /= base)
        buf[i] = "0123456789abcdef"[(val < 0) ? -(val % base) : val % base];
    
    for (; i >= 0; i--)
        buf[i] = c;
        
    return buf;
}

char* itoa2(uint16_t val, uint8_t buf_size, char *buf, int8_t base, char c)
{
    buf[buf_size-1] = 0;

    int8_t i = buf_size-2;
    	
    for(; val && i>=0 ; --i, val /= base)
        buf[i] = "0123456789abcdef"[val % base];
    
    for (; i >= 0; i--)
        buf[i] = c;
        
    return buf;
}
