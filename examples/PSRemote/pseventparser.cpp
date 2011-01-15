#include "pseventparser.h"

void PSEventParser::Parse(const uint16_t len, const uint8_t *pbuf, const uint32_t &offset)
{
	uint16_t	cntdn	= (uint16_t)len;
	uint8_t		*p	= (uint8_t*)pbuf;

	switch (nStage)
	{
	case 0:
		p	+= 12;
		cntdn	-= 12;

		if (!cntdn)
			return;
		nStage	++;

	case 1:
		theBuffer.valueSize = 4;
		valueParser.Initialize(&theBuffer);
		nStage ++;
	case 2:
		if (!valueParser.Parse(&p, &cntdn))
			return;

		//PrintHex<uint32_t>(*((uint32_t*)theBuffer.pValue));
		nStage ++;
	case 3:
		//Notify(PSTR("\r\nNumber of Fields:\t"));
		theBuffer.valueSize = 2;
		valueParser.Initialize(&theBuffer);
		nStage ++;
	case 4:
		if (!valueParser.Parse(&p, &cntdn))
			return;

		//PrintHex<uint16_t>(*((uint16_t*)theBuffer.pValue));
		nStage ++;
	case 5:
		//Notify(PSTR("\r\nEvent Code:\t"));
		theBuffer.valueSize = 2;
		valueParser.Initialize(&theBuffer);
		nStage ++;
	case 6:
		if (!valueParser.Parse(&p, &cntdn))
			return;

                eventCode = *((uint16_t*)theBuffer.pValue);
		//PrintHex<uint16_t>(*((uint16_t*)theBuffer.pValue));
		nStage ++;
	case 7:
		//Notify(PSTR("\r\nTransaction ID:\t"));
		theBuffer.valueSize = 4;
		valueParser.Initialize(&theBuffer);
		nStage ++;
	case 8:
		if (!valueParser.Parse(&p, &cntdn))
			return;

		//PrintHex<uint32_t>(*((uint32_t*)theBuffer.pValue));
		nStage ++;
    case 9:
        if (eventCode == PTP_EC_ObjectAdded)
			Notify(PSTR("\r\nObject Added:\t\t"));

		theBuffer.valueSize = 4;
		valueParser.Initialize(&theBuffer);
		nStage ++;
    case 10:
        if (eventCode == PTP_EC_ObjectAdded)
        {
			if (!valueParser.Parse(&p, &cntdn))
				return;

			objHandle = *((uint32_t*)theBuffer.pValue);
			PrintHex<uint32_t>(*((uint32_t*)theBuffer.pValue));
			Notify(PSTR("\r\n"));
        }
		if (eventCode == PTP_EC_CaptureComplete)
			Notify(PSTR("\r\nCapture complete.\r\n"));
		nStage ++;
    case 11:
		nStage = 0;
	}
}
