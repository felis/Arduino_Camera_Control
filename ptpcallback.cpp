#include "ptpcallback.h"
#include "ptpdebug.h"

bool MultiByteValueParser::Parse(uint8_t **pp, uint16_t *pcntdn)
{
	if (!pBuf)
	{
		//Notify(PSTR("Buffer pointer is NULL!\r\n"));
		return false;
	}
	for (; countDown && (*pcntdn); countDown--, (*pcntdn)--, (*pp)++)
		pBuf[valueSize-countDown] = (**pp);

	if (countDown)
		return false;

	countDown = valueSize;
	return true;
}

bool PTPListParser::Parse(uint8_t **pp, uint16_t *pcntdn, PTP_ARRAY_EL_FUNC pf)
{
	switch (nStage)
	{
	case 0:
		pBuf->valueSize = lenSize;
		theParser.Initialize(pBuf);
		nStage ++;

	case 1:
		if (!theParser.Parse(pp, pcntdn))
			return false;
		
		arLen = 0;
		arLen = (pBuf->valueSize >= 4) ? *((uint32_t*)pBuf->pValue) : (uint32_t)(*((uint16_t*)pBuf->pValue));

		//Serial.println(arLen, DEC);

		arLenCntdn = arLen;
		nStage ++;

	case 2:
		//Serial.println(valSize, DEC);

		pBuf->valueSize = valSize;
		theParser.Initialize(pBuf);
		nStage ++;

	case 3:
		for ( ;arLenCntdn; arLenCntdn--)
		{
			if (!theParser.Parse(pp, pcntdn))
				return false;
			
			if (pf)
				pf(pBuf, arLen - arLenCntdn);
		}
		nStage = 0;
	}
	return true;
}
