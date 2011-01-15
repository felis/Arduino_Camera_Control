/* Copyright (C) 2010-2011 Circuits At Home, LTD. All rights reserved.

This software may be distributed and modified under the terms of the GNU
General Public License version 2 (GPL2) as published by the Free Software
Foundation and appearing in the file GPL2.TXT included in the packaging of
this file. Please note that GPL2 Section 2[b] requires that all works based
on this software must also be made publicly available under the terms of
the GPL2 ("Copyleft").

Contact information
-------------------

Circuits At Home, LTD
Web      :  http://www.circuitsathome.com
e-mail   :  support@circuitsathome.com
*/
#if !defined(__VALUELIST_H__)
#define __VALUELIST_H__

#include <avr/eeprom.h> 
#include "WProgram.h"


template <class ValueType, const uint8_t TitleSize> 
struct ValueTitle
{
	ValueType	value;
	const char	title[TitleSize];
};

template <class ValueType, const uint16_t ListSize>
class ValueList
{
public:
	uint16_t	listSize;
	ValueType	valueList[ListSize];
	ValueType	currentValue;

	ValueType GetNext()
	{
		for (uint16_t i=0; i<listSize; i++)
			if (valueList[i] == currentValue)
				return ((i+1 < listSize) ? valueList[i+1] : currentValue);
		
		return currentValue;
	};

	ValueType GetPrev()
	{
		for (uint16_t i=0; i<listSize; i++)
			if (valueList[i] == currentValue)
				return ((i-1 >= 0) ? valueList[i-1] : currentValue);
		
		return currentValue;
	};
};



template <class ValueType, const uint8_t TitleSize> 
const char* FindTitle(uint8_t size, const ValueTitle<ValueType, TitleSize> *p, ValueType val)
{
	for (int i=0; i<size; i++)
	{
		if (pgm_read_byte(&(p[i].value)) == val)
			return (const char*)p[i].title;
	}
	return PSTR("N/A");
}


class EEPROMByteList
{
        const uint16_t  listOffset;
        const uint8_t   maxListSize;
	uint8_t	        listSize;

	uint16_t GetValueAddress(uint8_t val)
	{
                uint16_t tail = listOffset+listSize+2;
                
		for (uint16_t i=listOffset+1; i<tail; i++)
			if (eeprom_read_byte((uint8_t*)i) == val)
                                return i;

		return 0xffff;
	};
        
public:
        EEPROMByteList(uint16_t list_offset, uint16_t max_size) : listOffset(list_offset), maxListSize(max_size), listSize(0)
        {
        };
        
	uint16_t GetValueIndex(uint8_t val)
	{
		uint16_t addr = GetValueAddress(val);

		return (addr == 0xffff) ? addr : addr - listOffset - 1;
	};

        void SetSize(uint8_t size)
        {
                listSize = (size < maxListSize) ? size : maxListSize;
                
                if (eeprom_read_byte((uint8_t*) listOffset) != listSize) 
                        eeprom_write_byte((uint8_t*) listOffset, listSize);
        };
        
        uint8_t GetSize()
        {
            return listSize;
        };
        
        
        uint8_t Get(uint8_t i)
        {
             return (eeprom_read_byte((uint8_t*)(listOffset + 1 + ((i < listSize) ? i : listOffset+listSize-1))));
        };
        
        void Set(uint8_t i, uint8_t val)
        {
            if (i < listSize)
            {
                uint16_t pos = listOffset + i + 1;
                
                if (eeprom_read_byte((uint8_t*) pos) != val)
                    eeprom_write_byte((uint8_t*) pos, val);
            }
        };
        
	uint8_t GetNext(uint8_t val, uint8_t di=1)
	{
		uint16_t addr = GetValueAddress(val);

		uint16_t tail = listOffset+listSize;

		if (addr == 0xffff)
			return eeprom_read_byte((uint8_t*)tail);

		addr += di;

	    	return eeprom_read_byte((uint8_t*)((addr > tail) ? tail : addr));
	};

	uint8_t GetPrev(uint8_t val, uint8_t di=1)
	{
		uint16_t addr = GetValueAddress(val);

		if (addr == 0xffff)
			return eeprom_read_byte((uint8_t*)(listOffset+1));

		addr -= di;

	    	return eeprom_read_byte((uint8_t*)((addr <= listOffset) ? listOffset+1 : addr));
	};
};



template <class VALUE_TYPE, const uint16_t MAX_LIST_SIZE>
class SRAMValueList
{
	VALUE_TYPE		theList[MAX_LIST_SIZE];
	uint16_t	        listSize;

	uint16_t GetValueAddress(VALUE_TYPE val)
	{               
		for (uint16_t i=0; i<listSize; i++)
			if (theList[i] == val)
                                return i;

		return 0xffff;
	};

public:       
        SRAMValueList() : listSize(0)
        {
        };

	uint16_t GetValueIndex(VALUE_TYPE val)
	{
		return GetValueAddress(val);
	};
        
        void SetSize(uint16_t size)
        {
                listSize = (size <= MAX_LIST_SIZE) ? size : MAX_LIST_SIZE;
        };
        
        uint16_t GetSize()
        {
            return listSize;
        };
        
        
        VALUE_TYPE Get(uint16_t i)
        {
             return (theList[(i < listSize) ? i : listSize-1]);
        };
        
        void Set(uint16_t i, VALUE_TYPE val)
        {
            if (i < listSize)
		theList[i] = val;
        };
        
        void Append(VALUE_TYPE val)
        {
            if (listSize < MAX_LIST_SIZE)
		theList[listSize++] = val;
        };
        
	uint8_t GetNext(VALUE_TYPE val, uint8_t di=1)
	{
		uint16_t addr = GetValueAddress(val);

		if (addr == 0xffff)
			return theList[(addr < listSize) ? addr : listSize - 1];

		addr += di;

	    	return theList[(addr < listSize) ? addr : listSize-1];
	};

	uint8_t GetPrev(VALUE_TYPE val, uint8_t di=1)
	{
		uint16_t addr = GetValueAddress(val);

		if (addr == 0xffff)
			return theList[0];

		addr -= di;

	    	return theList[(addr < listSize) ? addr : 0];
	};
};

#endif // #define __VALUELIST_H__