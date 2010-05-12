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
    Serial.print(val, HEX);
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