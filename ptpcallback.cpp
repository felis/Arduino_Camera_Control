/*****************************************************************************
*
* Copyright (C) 2010 Circuits At Home, LTD. All rights reserved.
*
* This software may be distributed and modified under the terms of the GNU
* General Public License version 2 (GPL) as published by the Free Software
* Foundation and appearing in the file GPL.TXT included in the packaging of
* this file. Please note that GPL Section 2[b] requires that all works based
* on this software must also be made publicly available under the terms of
* the GPL ("Copyleft").
*
* Contact information:
* Circuits At Home Web site:  http://www.circuitsathome.com
* e-mail:                     support@circuitsathome.com
*****************************************************************************/
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
