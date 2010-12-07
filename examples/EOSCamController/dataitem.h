#if !defined(__DATAITEM_H__)
#define __DATAITEM_H__

#include <inttypes.h>
#include <canoneos.h>
#include <SimpleTimer.h>
#include <valuelist.h>

#define MUL10( a ) (((a) << 3 ) + ((a) << 1 ))

char* itoa2(int val, uint8_t buf_size, char *buf, int8_t base, char c);
char* itoa2(uint8_t val, uint8_t buf_size, char *buf, int8_t base, char c);
char* itoa2(uint16_t val, uint8_t buf_size, char *buf, int8_t base, char c);

class DataItemBase
{
protected:
    bool   isUpdated;
    
public:
    virtual void GetText(char** str, bool &is_pgm) = 0;
    
    bool IsUpdated() { return isUpdated; };
    void SetUpdated(bool upd) { isUpdated = upd; };
};

class TimeSpanDataItem : public DataItemBase
{
    uint32_t   dataValue;
    
    static char textValue[9];
    
public:   
    TimeSpanDataItem(uint32_t s) : dataValue(s) {};

    virtual void GetText(char** str, bool &is_pgm)
    {
        {
        uint16_t  h = dataValue / 3600;
        itoa2((uint16_t) h, 3, (char*)&textValue, 10, '0');
        }
        {
        textValue[2] = ':';
        uint16_t s = dataValue % 3600;
        itoa2((uint16_t) s / 60, 3, (char*)(textValue+3), 10, '0');
        textValue[5] = ':';
        itoa2((uint16_t) s % 60, 3, (char*)(textValue+6), 10, '0');
        }
        *str = textValue;
        is_pgm = false;
    };
    virtual void Set(uint32_t &val) 
    { 
        dataValue = val;
        isUpdated = true;
    };
};

#define MUL60( a ) (((a) << 6 ) - ((a) << 2 ))
#define MUL3600( a ) (((a) << 12 ) - ((a) << 9 ) + ((a) << 4 ))

class TimerDataItem : public DataItemBase
{
    SimpleTimer    &dataValue;
    
    static char textValue[9];
    
public:   
    TimerDataItem(SimpleTimer &t) : dataValue(t) {};

    virtual void GetText(char** str, bool &is_pgm)
    {
        uint16_t time_left = dataValue.TimeLeft() / 1000;
        
        uint16_t  ss = time_left % 60;
        time_left /= 60;
        uint16_t  mm = time_left % 60;
        time_left /= 60;
        uint16_t  hh = time_left;
        
        itoa2((uint16_t)hh, 3, (char*)&textValue, 10, '0');
        textValue[2] = ':';
        itoa2((uint16_t)mm, 3, (char*)(textValue+3), 10, '0');
        textValue[5] = ':';
        itoa2((uint16_t)ss, 3, (char*)(textValue+6), 10, '0');
        *str = textValue;
        is_pgm = false;
    };
    
};

template <class VALUE_TYPE, const uint8_t TABLE_SIZE, const uint8_t TEXT_SIZE>
class KeyValuePairDataItem : public DataItemBase
{
    VALUE_TYPE                                  dataValue;
    const ValueTitle<VALUE_TYPE, TEXT_SIZE>     *ptrTitles;
    
public:
    KeyValuePairDataItem(VALUE_TYPE val, const ValueTitle<VALUE_TYPE, TEXT_SIZE> *p) : dataValue(val), ptrTitles(p)
    {};
    
    virtual void GetText(char** str, bool &is_pgm)
    {
        *str = (char*)FindTitle<VALUE_TYPE, TEXT_SIZE>(TABLE_SIZE, ptrTitles, dataValue);
        is_pgm = true;
    };
    
    VALUE_TYPE Get() { return dataValue; };
    void Set(VALUE_TYPE val) { dataValue = val; isUpdated = true; };
};

template <class VALUE_TYPE, const uint8_t TEXT_LEN>
class IntDataItem : public DataItemBase
{
    VALUE_TYPE      dataValue;
    static char     valString[TEXT_LEN];
    
public:
    IntDataItem() : dataValue(0) { isUpdated = true; };
    IntDataItem(VALUE_TYPE data) : dataValue(data) {};
    
    virtual void Set(VALUE_TYPE data)
    {
        dataValue = data;
        isUpdated = true;
    };
    
    VALUE_TYPE Get() { return dataValue;  };
    
    virtual void GetText(char** str, bool &is_pgm) 
    { 
        *str = itoa2(dataValue, TEXT_LEN, (char*)&valString, 10, '0');
        is_pgm   = false;
    };
    void operator ++(int val)
    {
        dataValue += val;
        isUpdated = true;
    };
    void operator --(int val)
    {
        dataValue -= val;
        isUpdated = true;
    };
};

template <class VALUE_TYPE, const uint8_t TEXT_LEN>
char IntDataItem<VALUE_TYPE,TEXT_LEN>::valString[TEXT_LEN] = "";

class PgmStringDataItem : public DataItemBase
{
    const char      *pStr;
    
public:
    PgmStringDataItem(const char *str) : pStr(str) { isUpdated = true; };
    
    void SetText(const char *str) { pStr = str; isUpdated = true; };
    
    virtual void GetText(char** str, bool &is_pgm) 
    { 
        *str = (char*)pStr;
        is_pgm   = true;
    };
};

template <const uint8_t STRLEN>
class StringDataItem : public DataItemBase
{
    char      theString[STRLEN];
    
    void CopyString(char *src, char *dst)
    {
        char     *s = src, *d = dst;
        
        for (uint8_t  cnt = 0; *s && cnt < STRLEN-1; cnt++, s++, d++)
            *d = *s;
            
        *d = 0;
    };
public:
    StringDataItem() { theString[0] = 0; isUpdated = true; };
    StringDataItem(char *str) { CopyString(str, &theString); isUpdated = true; };

    void SetText(char *str) { CopyString(str, &theString); isUpdated = true; };
    
    virtual void GetText(char** str, bool &is_pgm) 
    { 
        *str = &theString;
        is_pgm   = false;
    };
};


#endif // __DATAITEM_H__
