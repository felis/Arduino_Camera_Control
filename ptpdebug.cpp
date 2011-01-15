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
#include "ptpdebug.h"

void HexDump::Parse(const uint16_t len, const uint8_t *pbuf, const uint32_t &offset)
{
	for (uint16_t j=0; j<len; j++, byteCount++, byteTotal++)
	{
        if (!byteCount)
        {
			PrintHex<uint16_t>(byteTotal);
            Serial.print(": ");
        }
		PrintHex<uint8_t>(pbuf[j]);
		Serial.print(" ");

		if (byteCount == 15)
		{
			Serial.println("");
			byteCount = 0xFF;
		}
	}
}

void EOSEventDump::Parse(const uint16_t len, const uint8_t *pbuf, const uint32_t &offset)
{
	uint8_t		*p		= (uint8_t*)pbuf;
	uint16_t	cntdn	= (uint16_t)len;

	switch (parseStage)
	{
	case 0:
		// Get PTP data packet size
		ptppktSize = *((uint32_t*)p);

		// Return if the packet has only one empty record
		if (ptppktSize == 0x14)
			return;

		Serial.println("\r\n");

		for (uint8_t i=0; i<4; i++) 
		{
			PrintHex<uint8_t>(((uint8_t*)&ptppktSize)[i]);
			Serial.print(" ");
		}

		// Skip PTP packet header
		p		+= 12;
		cntdn	-= 12;
		parseStage ++;
	case 1:
		parseSubstage = 0;
		parseStage ++;
	case 2:
		while (1)
		{
			switch (parseSubstage)
			{
			// CR after each event record
			case 0:
				Serial.println("");
				valueParser.Initialize(&valueBuffer);
				parseSubstage ++;

			// Parse record size value
			case 1:
				//Serial.println("1");
				if (!valueParser.Parse(&p, &cntdn))
					return;
				recordSize = (uint32_t)theBuffer;
				for (uint8_t i=0; i<4; i++) 
				{
					PrintHex<uint8_t>(((uint8_t*)&theBuffer)[i]);
					Serial.print(" ");
				}
				recordSize -= 4;
				valueParser.Initialize(&valueBuffer);
				parseSubstage ++;

			// Parse the first uint32_t value
			case 2:
				//Serial.println("2");
				if (!valueParser.Parse(&p, &cntdn))
					return;

				for (uint8_t i=0; i<4; i++) 
				{
					PrintHex<uint8_t>(((uint8_t*)&theBuffer)[i]);
					Serial.print(" ");
				}
				recordSize -= 4;

				// Return if empty(last) record
				if (recordSize == 0x08 && (uint32_t)theBuffer == 0)
				{
					parseSubstage	= 0;
					parseStage		= 0;
					return;
				}
				parseSubstage ++;

			// Print the rest of the record
			case 3:
				//Serial.println("3");
				for (; recordSize && cntdn; recordSize--, cntdn--, p++)
				{
					PrintHex<uint8_t>(*p);
					Serial.print(" ");
				}
				if (!recordSize)
					parseSubstage = 0;
				if (!cntdn)
					return;

			} // switch (parseSubstage)
		} // while(1)
	} // switch (parseStage)
}
