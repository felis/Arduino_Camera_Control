#ifndef __EOSEVENTPARSER_H__
#define __EOSEVENTPARSER_H__

#include <inttypes.h>
#include <avr/pgmspace.h>
#include <Max_LCD.h>
#include <ptpcallback.h>
#include <canoneos.h>


struct EOSParamValues
{
	uint8_t		upperValue;
	uint8_t		currentValue;
	uint8_t		lowerValue;
};

#define MAX_OBJ_IN_LIST		8

class EOSEventParser : public PTPReadParser
{
	const uint16_t		constInitialEventCode;

	uint8_t				paramsChanged;

	struct EOSEvent
	{
		uint16_t	eventCode;
		uint16_t	propCode;
		uint32_t	propValue;
	};

	uint8_t					nStage;
	uint8_t					nRecStage;
	uint16_t				nRecSize;
        MultiValueBuffer		        theBuffer;
	uint32_t				varBuffer;
	EOSEvent				eosEvent;
	uint16_t				paramCountdown;
	uint16_t				paramCount;

	MultiByteValueParser	                valueParser;
	ByteSkipper				byteSkipper;

	bool EventRecordParse(uint8_t **pp, uint16_t *pcntdn);
	bool IsLastEventRecord() { return (eosEvent.eventCode == 0); };
	void InitEOSEventStruct();

public:
	EOSEventParser() : 
		constInitialEventCode(0xFFFF),
		nStage(0),
		nRecStage(0),
		nRecSize(0),
		varBuffer(0),
		paramCountdown(0),
		paramsChanged(0)
		{ 
			theBuffer.pValue = &varBuffer; 
		};

	void Reset()
	{
		nStage			= 0;
		nRecStage		= 0;
		nRecSize		= 0;
		varBuffer		= 0;
		paramCountdown	= 0;
		paramsChanged	= 0;
	};

	virtual void Parse(const uint16_t len, const uint8_t *pbuf, const uint32_t &offset);
};

#endif // __EOSEVENTPARSER_H__
