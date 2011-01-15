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
#ifndef __PTPDEBUG_H__
#define __PTPDEBUG_H__

#include <inttypes.h>
#include <avr/pgmspace.h>
#include "ptpcallback.h"

void Notify(const char* msg);
void Message(const char* msg, uint16_t rcode);

template <class T> 
void PrintHex(T val)
{
    T    mask = (((T)1) << (((sizeof(T) << 1) - 1) << 2));
    
    while (mask > 1)
    {
		if (val < mask)
		  Serial.print("0");

		mask >>= 4;
    }
    Serial.print((T)val, HEX);
}

template <class T> 
void PrintHex2(Print *prn, T val)
{
    T    mask = (((T)1) << (((sizeof(T) << 1) - 1) << 2));
    
    while (mask > 1)
    {
		if (val < mask)
		  prn->print("0");

		mask >>= 4;
    }
    prn->print((T)val, HEX);
}

class HexDump : public PTPReadParser
{
	uint8_t		byteCount;
	uint16_t    byteTotal;

public:
	HexDump() : byteCount(0), byteTotal(0) {};
	void Initialize() { byteCount = 0; byteTotal = 0; };
	virtual void Parse(const uint16_t len, const uint8_t *pbuf, const uint32_t &offset);
};

#include "ptpcallback.h"

class EOSEventDump : public PTPReadParser
{
	uint32_t	ptppktSize;
	uint16_t	recordSize;
	uint8_t		parseStage;
	uint8_t		parseSubstage;

	MultiByteValueParser	valueParser;
	MultiValueBuffer		valueBuffer;
	uint32_t				theBuffer;
public:
	EOSEventDump() : ptppktSize(0), recordSize(0), parseStage(0), parseSubstage(0) 
		{ valueBuffer.valueSize = 4; valueBuffer.pValue = &theBuffer; };
	void Initialize() { ptppktSize = 0; recordSize = 0; parseStage = 0; valueParser.Initialize(&valueBuffer); };
	virtual void Parse(const uint16_t len, const uint8_t *pbuf, const uint32_t &offset);
};
#endif // __PTPDEBUG_H__