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
#ifndef __PTPDPPARSER_H__
#define __PTPDPPARSER_H__

#include <inttypes.h>
#include <avr/pgmspace.h>
#include <ptpconst.h>
#include <ptp.h>
#include <valuelist.h>

template <class VALUE_TYPE>
struct PTPDevicePropValue
{
    VALUE_TYPE        valCurrent;    
    uint8_t           listForm;
    VALUE_TYPE        arrValues[3];

    static void SaveEnumValue(MultiValueBuffer *p, uint32_t count, void *me) 
    { 
          PTPDevicePropValue<VALUE_TYPE>    *dp = (PTPDevicePropValue<VALUE_TYPE>*)me;
          
          if (!dp)
          {
              PTPTRACE("NULL pointer!!!\r\n");
              return;
          }
          switch (dp->listForm)
          {
          case 2:
              if (dp->arrValues[1] != dp->valCurrent || (dp->arrValues[1] == dp->valCurrent && dp->arrValues[2] <= dp->valCurrent)) 
              {
                  dp->arrValues[0] = dp->arrValues[1];
                  dp->arrValues[1] = dp->arrValues[2];
                  dp->arrValues[2] = *((VALUE_TYPE*)p->pValue);
              }
              break;
          case 1:
              dp->arrValues[count] = *((VALUE_TYPE*)p->pValue);
              break;
          }
    };
};

template <class VALUE_TYPE>
class PTPDevPropParser : public PTPReadParser
{
	uint8_t				nStage;
    uint8_t             enStage;
    uint8_t				formFlag;
    MultiValueBuffer	theBuffer;
    uint8_t				varBuffer[sizeof(VALUE_TYPE)];
    uint16_t            enLen;
    uint16_t            enLenCntdn;
        
	MultiByteValueParser				valParser;
    PTPDevicePropValue<VALUE_TYPE>     *pDPValue;
        
	bool ParseValue		(uint8_t **pp, uint16_t *pcntdn, VALUE_TYPE&);       
	bool ParseEnumSingle(uint8_t **pp, uint16_t *pcntdn);       
    
    PTPListParser      enumParser;
    
    uint8_t GetDataSize();
        
public:
	PTPDevPropParser(PTPDevicePropValue<VALUE_TYPE> *p) :
        pDPValue(p),
		nStage(0),
		enStage(0),
		formFlag(0),
        enLen(0),
        enLenCntdn(0)
	{
        theBuffer.valueSize = sizeof(VALUE_TYPE);
		theBuffer.pValue = varBuffer;
	}
	virtual void Parse(const uint16_t len, const uint8_t *pbuf, const uint32_t &offset);
};

template <class VALUE_TYPE>
bool PTPDevPropParser<VALUE_TYPE>::ParseValue(uint8_t **pp, uint16_t *pcntdn, VALUE_TYPE &val)
{
    val = *((VALUE_TYPE*)*pp);
    (*pp)	+= sizeof(VALUE_TYPE);
    (*pcntdn)	-= sizeof(VALUE_TYPE);
    return true;
}

template <class VALUE_TYPE>
bool PTPDevPropParser<VALUE_TYPE>::ParseEnumSingle(uint8_t **pp, uint16_t *pcntdn)
{
    switch(enStage)
    {
    case 0:
        enumParser.Initialize(2, sizeof(VALUE_TYPE), &theBuffer);
        enStage ++;
    case 1:
        if (!enumParser.Parse(pp, pcntdn, (PTP_ARRAY_EL_FUNC)&PTPDevicePropValue<VALUE_TYPE>::SaveEnumValue, pDPValue))
            return false;
        enStage = 0;
    } // switch
    return true;
}

template <class VALUE_TYPE>
void PTPDevPropParser<VALUE_TYPE>::Parse(const uint16_t len, const uint8_t *pbuf, const uint32_t &offset)
{
	uint16_t	cntdn	= (uint16_t)len;
	uint8_t		*p	= (uint8_t*)pbuf;

        VALUE_TYPE      vt = 1;

	switch (nStage)
	{
	case 0:
		p	+= 17;
		cntdn	-= 17;
                nStage = 1;
	case 1:
		if (!ParseValue(&p, &cntdn, vt))
			return;
		nStage = 2;
	case 2:
		if (!ParseValue(&p, &cntdn, pDPValue->valCurrent))
			return;
		nStage = 3;
	case 3:
                for (uint8_t i=0; i<3; i++)
                    pDPValue->arrValues[i] = pDPValue->valCurrent;
                    
		formFlag = (*p);
                pDPValue->listForm = formFlag;
		p ++;		
		cntdn --;
		nStage = 4;
	case 4:
		if (formFlag == 1)
			enumParser.Initialize(2, sizeof(VALUE_TYPE), &theBuffer, PTPListParser::modeRange);
                nStage = 5;
	case 5:
		if (formFlag == 1)
			if (!enumParser.Parse(&p, &cntdn, (PTP_ARRAY_EL_FUNC)&PTPDevicePropValue<VALUE_TYPE>::SaveEnumValue, pDPValue))
                              return;
                
                if (formFlag == 2)
			if (!ParseEnumSingle(&p, &cntdn))
				return;
                
		nStage = 0;
	}
}

template <class VALUE_TYPE>
uint16_t StepUp(PTP *ptp, uint16_t prop)
{
    PTPDevicePropValue<VALUE_TYPE>  val;
    PTPDevPropParser<VALUE_TYPE>    prs(&val);

    uint16_t ret = ptp->GetDevicePropDesc(prop, &prs);
    
    if (ret != PTP_RC_OK)
        return ret;

    if (val.listForm == 2)
        if (val.arrValues[1] == val.valCurrent)
            return ptp->SetDevicePropValue(prop, (VALUE_TYPE)val.arrValues[2]);
            
    if (val.listForm == 1)
        if (val.valCurrent + val.arrValues[2] <= val.arrValues[1])
            return ptp->SetDevicePropValue(prop, (VALUE_TYPE)(val.valCurrent + val.arrValues[2]));
    
    return PTP_RC_OK;
}

template <class VALUE_TYPE>
uint16_t StepDown(PTP *ptp, uint16_t prop)
{
    PTPDevicePropValue<VALUE_TYPE>  val;
    PTPDevPropParser<VALUE_TYPE>    prs(&val);
    
    uint16_t ret = ptp->GetDevicePropDesc(prop, &prs);
    
    if (ret != PTP_RC_OK)
        return ret;
        
    if (val.listForm == 2)
    {    
        if (val.arrValues[1] == val.valCurrent && val.arrValues[0] < val.arrValues[1])
            return ptp->SetDevicePropValue(prop, (VALUE_TYPE)val.arrValues[0]);
        
        if (val.arrValues[2] == val.valCurrent)
            return ptp->SetDevicePropValue(prop, (VALUE_TYPE)val.arrValues[1]);
    }
    if (val.listForm == 1)
    {
        VALUE_TYPE new_val = val.valCurrent - val.arrValues[2]; 
        if (new_val >= val.arrValues[0] && new_val < val.valCurrent)
            return ptp->SetDevicePropValue(prop, (VALUE_TYPE)(val.valCurrent - val.arrValues[2]));
    }       
    return PTP_RC_OK;
}

template <class VALUE_TYPE, class LIST_VALUE_TYPE, const uint8_t TABLE_SIZE, const uint8_t TEXT_SIZE>
uint16_t GetValueTitle(PTP *ptp, uint16_t prop, const ValueTitle<LIST_VALUE_TYPE, TEXT_SIZE> *p, const char **t)
{
    VALUE_TYPE  val;
    uint16_t ret = ptp->GetDevicePropValue(prop, val);
    
    if (ret != PTP_RC_OK)
        return ret;
        
    *t = (char*)FindTitle<LIST_VALUE_TYPE, TEXT_SIZE>(TABLE_SIZE, p, (LIST_VALUE_TYPE)val);
    return ret;
}

template <class VALUE_TYPE, class LIST_VALUE_TYPE, const uint8_t TABLE_SIZE, const uint8_t TEXT_SIZE>
uint16_t PrintValueTitle(PTP *ptp, uint16_t prop, const ValueTitle<LIST_VALUE_TYPE, TEXT_SIZE> *p )
{
	const char	*title;

	if (GetValueTitle<LIST_VALUE_TYPE, LIST_VALUE_TYPE, TABLE_SIZE, TEXT_SIZE>(ptp, prop, p, &title) == PTP_RC_OK)
		Notify(title);
}

#endif // __PTPDPPARSER_H__
