#include "devpropparser.h"

const char* DevPropParser::dtNames1[] PROGMEM = 
{
	msgUNDEF,
	msgINT8,	
	msgUINT8,	
	msgINT16,	
	msgUINT16,
	msgINT32,	
	msgUINT32,
	msgINT64,	
	msgUINT64,
	msgINT128,
	msgUINT128
};

const char* DevPropParser::dtNames2[] PROGMEM = 
{
	msgUNDEF,
	msgAINT8,
	msgAUINT8,	
	msgAINT16,	
	msgAUINT16,
	msgAINT32,
	msgAUINT32,
	msgAINT64,	
	msgAUINT64,
	msgAINT128,
	msgAUINT128
};

const char* DevPropParser::prNames[] PROGMEM = 
{
	msgUndefined,					
	msgBatteryLevel,				
	msgFunctionalMode,			
	msgImageSize,					
	msgCompressionSetting,		
	msgWhiteBalance,				
	msgRGBGain,					
	msgFNumber,					
	msgFocalLength,				
	msgFocusDistance,				
	msgFocusMode,					
	msgExposureMeteringMode,		
	msgFlashMode,					
	msgExposureTime,				
	msgExposureProgramMode,		
	msgExposureIndex,				
	msgExposureBiasCompensation,	
	msgDateTime,					
	msgCaptureDelay,				
	msgStillCaptureMode,			
	msgContrast,					
	msgSharpness,					
	msgDigitalZoom,				
	msgEffectMode,				
	msgBurstNumber,				
	msgBurstInterval,				
	msgTimelapseNumber,			
	msgTimelapseInterval,			
	msgFocusMeteringMode,			
	msgUploadURL,					
	msgArtist,					
	msgCopyrightInfo				
};

void DevPropParser::PrintDataType(uint8_t **pp, uint16_t *pcntdn)
{
	dataType = *((uint16_t*)(*pp));
        bytesSize = GetDataSize();
        
	Notify(PSTR("Data Type:\t\t"));

	switch (((dataType >> 8) & 0xFF))
	{
	case 0x00:
		if ((dataType & 0xFF) <= (PTP_DTC_UINT128 & 0xFF))
			Notify((char*)pgm_read_word(&dtNames1[(dataType & 0xFF)]));
		break;
	case 0x40:
		if ((dataType & 0xFF) <= (PTP_DTC_AUINT128 & 0xFF))
			Notify((char*)pgm_read_word(&dtNames2[(dataType & 0xFF)]));
		break;
	case 0xFF:
		Notify(PSTR("STR"));
		break;
	default:
		Notify(PSTR("Unknown"));
	}
	Notify(PSTR("\r\n"));
	(*pp)		+= 2;		
	(*pcntdn)	-= 2;
}

void DevPropParser::PrintDevProp(uint8_t **pp, uint16_t *pcntdn)
{
	uint16_t	op = *((uint16_t*)(*pp));

	Notify(PSTR("\r\nDevice Property:\t"));

	if ((((op >> 8) & 0xFF) == 0x50) && ((op & 0xFF) <= (PTP_DPC_CopyrightInfo & 0xFF)))
	{
		PrintHex<uint16_t>(op);
		Notify(PSTR("\t"));
		Notify((char*)pgm_read_word(&prNames[(op & 0xFF)]));
		Notify(PSTR("\r\n"));
	}
	else
	{
		PrintHex<uint16_t>(op);
		Notify(PSTR(" (Vendor defined)\r\n"));
	}
	(*pp)		+= 2;		
	(*pcntdn)	-= 2;
}

void DevPropParser::PrintGetSet(uint8_t **pp, uint16_t *pcntdn)
{
	Notify(PSTR("Get/Set:\t\t"));
	Notify(((**pp) == 0x01) ? PSTR("Get/Set\r\n") : (!(**pp)) ? PSTR("Get\r\n") : PSTR("Illegal\r\n"));
	(*pp) ++;		
	(*pcntdn) --;
}

uint8_t DevPropParser::GetDataSize()
{
	switch (dataType)
	{
	case PTP_DTC_INT8:	
	case PTP_DTC_UINT8:	
            bytesSize = 1;
	    break;
	case PTP_DTC_AINT8:	
	case PTP_DTC_AUINT8:
            bytesSize = 1;
	    enumParser.Initialize(4, bytesSize, &theBuffer);
	    break;
	case PTP_DTC_INT16:	
	case PTP_DTC_UINT16:	
            bytesSize = 2;
	    enumParser.Initialize(4, bytesSize, &theBuffer);
            break;
	case PTP_DTC_AINT16:	
	case PTP_DTC_AUINT16:
            bytesSize = 2;
	    break;
	case PTP_DTC_STR:
            bytesSize = 2;
	    enumParser.Initialize(1, bytesSize, &theBuffer);
	    break;
	case PTP_DTC_INT32:	
	case PTP_DTC_UINT32:	
            bytesSize = 4;
	    break;
	case PTP_DTC_AINT32:	
	case PTP_DTC_AUINT32:
            bytesSize = 4;
	    enumParser.Initialize(4, bytesSize, &theBuffer);
	    break;
	case PTP_DTC_INT64:	
	case PTP_DTC_UINT64:	
            bytesSize = 8;
	    break;
	case PTP_DTC_AINT64:	
	case PTP_DTC_AUINT64:
            bytesSize = 8;
	    enumParser.Initialize(4, bytesSize, &theBuffer);
	    break;
	case PTP_DTC_INT128:	
	case PTP_DTC_UINT128:	
            bytesSize = 16;
	    break;
	case PTP_DTC_AINT128:	
	case PTP_DTC_AUINT128:
            bytesSize = 16;
            enumParser.Initialize(4, bytesSize, &theBuffer);
	    break;
	}
    return bytesSize;
}

bool DevPropParser::PrintValue(uint8_t **pp, uint16_t *pcntdn)
{
	switch (dataType)
	{
	case PTP_DTC_INT8:	
	case PTP_DTC_UINT8:	
		PrintHex<uint8_t>(**pp);
		(*pp) ++;
		(*pcntdn) --;
		break;
	case PTP_DTC_AINT8:	
	case PTP_DTC_AUINT8:
		if (!enumParser.Parse(pp, pcntdn, (PTP_ARRAY_EL_FUNC)&PrintByte))
			return false;
		break;
	case PTP_DTC_INT16:	
	case PTP_DTC_UINT16:	
		PrintHex<uint16_t>(*((uint16_t*)*pp));
		(*pp)		+= 2;
		(*pcntdn)	-= 2;
		break;
	case PTP_DTC_AINT16:	
	case PTP_DTC_AUINT16:
		if (!enumParser.Parse(pp, pcntdn, (PTP_ARRAY_EL_FUNC)&PrintTwoBytes))
			return false;
		break;
	case PTP_DTC_INT32:	
	case PTP_DTC_UINT32:	
		PrintHex<uint32_t>(*((uint32_t*)*pp));
		(*pp)		+= 4;
		(*pcntdn)	-= 4;
		break;
	case PTP_DTC_AINT32:	
	case PTP_DTC_AUINT32:
		if (!enumParser.Parse(pp, pcntdn, (PTP_ARRAY_EL_FUNC)&PrintFourBytes))
			return false;
		break;
	case PTP_DTC_INT64:	
	case PTP_DTC_UINT64:	
		(*pp)		+= 8;
		(*pcntdn)	-= 8;
		break;
	case PTP_DTC_AINT64:	
	case PTP_DTC_AUINT64:
		if (!enumParser.Parse(pp, pcntdn, (PTP_ARRAY_EL_FUNC)&PrintEightBytes))
			return false;
		break;
	case PTP_DTC_INT128:	
	case PTP_DTC_UINT128:	
		(*pp)		+= 16;
		(*pcntdn)	-= 16;
		break;
	case PTP_DTC_AINT128:	
	case PTP_DTC_AUINT128:
		if (!enumParser.Parse(pp, pcntdn, NULL))
			return false;
		break;
	case PTP_DTC_STR:
		if (!enumParser.Parse(pp, pcntdn, (PTP_ARRAY_EL_FUNC)&PrintChar))
			return false;
		break;
	}
	return true;
}

bool DevPropParser::ParseEnum(uint8_t **pp, uint16_t *pcntdn)
{
    if ((dataType & 0xFF00) == 0)
        return ParseEnumSingle(pp, pcntdn);
    else
        return ParseEnumArray(pp, pcntdn);
}

bool DevPropParser::ParseEnumArray(uint8_t **pp, uint16_t *pcntdn)
{
    switch(enStage)
    {
    case 0:
        theBuffer.valueSize = 2;
	valParser.Initialize(&theBuffer);
        enStage = 1;
    case 1:
	if (!valParser.Parse(pp, pcntdn))
            return false;
        enLen = 0;
        enLen = *((uint16_t*)theBuffer.pValue);
        enLenCntdn = enLen;
        enumParser.Initialize(4, bytesSize, &theBuffer);
        enStage = 2;
    case 2:
        for (; enLenCntdn; enLenCntdn--)
        {
            if (!enumParser.Parse(pp, pcntdn, (PTP_ARRAY_EL_FUNC)&PrintEnumValue), PTPListParser::modeArray, this)
                return false;
                
            Notify(PSTR("\r\n"));
            enumParser.Initialize(4, bytesSize, &theBuffer);
        }
        enStage = 0;
    } // switch
    return true;
}

bool DevPropParser::ParseEnumSingle(uint8_t **pp, uint16_t *pcntdn)
{
    switch(enStage)
    {
    case 0:
        enumParser.Initialize(2, bytesSize, &theBuffer);
//        Serial.println(bytesSize, DEC);
        enStage = 1;
    case 1:
        if (!enumParser.Parse(pp, pcntdn, (PTP_ARRAY_EL_FUNC)&PrintEnumValue, this))
            return false;
        enStage = 0;
    } // switch
    return true;
}

void DevPropParser::Parse(const uint16_t len, const uint8_t *pbuf, const uint32_t &offset)
{
	uint16_t	cntdn	= (uint16_t)len;
	uint8_t		*p	= (uint8_t*)pbuf;

	switch (nStage)
	{
	case 0:
		p	+= 12;
		cntdn	-= 12;
                nStage = 1;
        case 1:
		PrintDevProp(&p, &cntdn);
		nStage = 2; //++;
        case 2:
		PrintDataType(&p, &cntdn);
		nStage = 3; //++;
        case 3:
		PrintGetSet(&p, &cntdn);
		nStage = 4; //++;
	case 4:
		Notify(PSTR("Default Value:\t\t"));
		nStage = 5; //++;
	case 5:
		if (!PrintValue(&p, &cntdn))
			return;
		Notify(PSTR("\r\n"));
		nStage = 6; //++;
	case 6:
		Notify(PSTR("Current Value:\t\t"));
		nStage = 7; //++;
	case 7:
		if (!PrintValue(&p, &cntdn))
			return;
		Notify(PSTR("\r\n"));
		nStage = 8; //++;
	case 8:
		formFlag = (*p);
		p ++;		
		cntdn --;
		nStage = 9; //++;
	case 9:
		if (formFlag == 1)
		{
			Notify(PSTR("Range (Min,Max,Step):\t\t{"));
			enumParser.Initialize(2, bytesSize, &theBuffer, PTPListParser::modeRange);
		}
		if (formFlag == 2)
			Notify(PSTR("Enumeration:\t\t{"));
                nStage = 10; //++;
	case 10:
		if (formFlag == 1)
			if (!enumParser.Parse(&p, &cntdn, (PTP_ARRAY_EL_FUNC)&PrintEnumValue))
                              return;

                if (formFlag == 2)
			if (!ParseEnum(&p, &cntdn))
				return;

                if (formFlag)
			Notify(PSTR("}\r\n"));
		
		nStage = 0;
	}
}

