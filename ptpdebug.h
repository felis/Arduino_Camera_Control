#ifndef __PTPDEBUG_H__
#define __PTPDEBUG_H__

#include "ptp.h"

class HexDump : public PTPReadParser
{
	uint8_t		byteCount;
	uint16_t    byteTotal;

public:
	HexDump() : byteCount(0), byteTotal(0) {};
	virtual void Parse(const uint16_t len, const uint8_t *pbuf, const uint32_t &offset);
};

#endif // __PTPDEBUG_H__