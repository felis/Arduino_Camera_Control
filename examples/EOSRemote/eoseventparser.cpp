#include "eoseventparser.h"
#include <eosvaluetitles.h>

extern EEPROMByteList          vlAperture;
extern EEPROMByteList          vlShutterSpeed;
extern EEPROMByteList          vlWhiteBalance;
extern EEPROMByteList          vlPictureStyle;
extern EEPROMByteList          vlExpCompensation;
extern EEPROMByteList          vlIso;

extern uint8_t  dpMode;
extern uint8_t  dpAperture;
extern uint8_t  dpShutterSpeed;
extern uint8_t  dpWb;
extern uint8_t  dpPStyle;
extern uint8_t  dpIso;
extern uint8_t  dpExpComp;


bool EOSEventParser::EventRecordParse(uint8_t **pp, uint16_t *pcntdn)
{
	switch (nRecStage)
	{
	case 0:
		// Retrieves the size of the event record
		if (!valueParser.Parse(pp, pcntdn))
			return false;

		nRecSize = (uint16_t)varBuffer;

		// calculates the number of event parameters ( size / 4 - 1 )
		paramCountdown	= (nRecSize >> 2) - 1;
                
		paramCount		=  1;
		nRecSize		-= 4;
		nRecStage ++;
	case 1:
		for (; paramCountdown; paramCountdown--, paramCount++, nRecSize -= 4)
		{
			if (!valueParser.Parse(pp, pcntdn))
				return false;

			switch (paramCount)
			{
			// Event Code
			case 1:
				eosEvent.eventCode = (uint16_t)varBuffer;
				break;
			// Property Code
			case 2:
//				if (eosEvent.eventCode == EOS_EC_ObjectCreated)
//				{
//				}
				eosEvent.propCode = (uint16_t)varBuffer;
				break;
			// C189 - Property Value, C18A - Enumerator Type
			case 3:
				eosEvent.propValue = varBuffer;
                                        
				if (eosEvent.eventCode == EOS_EC_DevPropChanged)
				{
                                        
                                        switch (eosEvent.propCode)
                                        {
                                        case EOS_DPC_Aperture:
                                            dpAperture = (uint8_t) varBuffer;
                                            Notify(PSTR("F:"));
                                            Notify((char*)FindTitle<VT_APERTURE, VT_APT_TEXT_LEN>(VT_APT_COUNT, ApertureTitles, dpAperture));
                                            Notify(PSTR("\r\n"));
                                            break;
                                        case EOS_DPC_ShutterSpeed:
                                            dpShutterSpeed = (uint8_t) varBuffer;
                                            Notify(PSTR("T:"));
                                            Notify((char*)FindTitle<VT_SHSPEED, VT_SHSPEED_TEXT_LEN>(VT_SHSPEED_COUNT, ShutterSpeedTitles, dpShutterSpeed));
                                            Notify(PSTR("\r\n"));
                                            break;
                                        case EOS_DPC_ShootingMode:
                                            dpMode = (uint8_t) varBuffer;
                                            Notify(PSTR("Mode:"));
                                            Notify((char*)FindTitle<VT_MODE, VT_MODE_TEXT_LEN>(VT_MODE_COUNT, ModeTitles, dpMode));
                                            Notify(PSTR("\r\n"));
                                            break;
                                        case EOS_DPC_WhiteBalance:
                                            dpWb = (uint8_t) varBuffer;
                                            Notify(PSTR("WB:"));
                                            Notify((char*)FindTitle<VT_WB, VT_WB_TEXT_LEN>(VT_WB_COUNT, WbTitles, dpWb));
                                            Notify(PSTR("\r\n"));
                                            break;
                                        case EOS_DPC_PictureStyle:
                                            dpPStyle = (uint8_t) varBuffer;
                                            Notify(PSTR("Pict Style:"));
                                            Notify((char*)FindTitle<VT_PSTYLE, VT_PSTYLE_TEXT_LEN>(VT_PSTYLE_COUNT, PStyleTitles, dpPStyle));
                                            Notify(PSTR("\r\n"));
                                            break;
                                        case EOS_DPC_Iso:
                                            dpIso = (uint8_t) varBuffer;
                                            Notify(PSTR("ISO:"));
                                            Notify((char*)FindTitle<VT_ISO, VT_ISO_TEXT_LEN>(VT_ISO_COUNT, IsoTitles, dpIso));
                                            Notify(PSTR("\r\n"));
                                            break;
                                        case EOS_DPC_ExposureCompensation:
                                            dpExpComp = (uint8_t) varBuffer;
                                            Notify(PSTR("Exp Comp:"));
                                            Notify((char*)FindTitle<VT_EXPCOMP, VT_EXPCOMP_TEXT_LEN>(VT_EXPCOMP_COUNT, ExpCompTitles, dpExpComp));
                                            Notify(PSTR("\r\n"));
                                            break;
                                        };
				}
				break;
			// C18A/enumType == 3 - Size of enumerator array
			case 4:
				if (eosEvent.eventCode == EOS_EC_DevPropValuesAccepted)
				{
                                        switch (eosEvent.propCode)
                                        {
					case EOS_DPC_Aperture:
                                                vlAperture.SetSize((uint8_t)varBuffer);
                                                break;
					case EOS_DPC_ShutterSpeed:
                                                vlShutterSpeed.SetSize((uint8_t)varBuffer);
                                                break;
					case EOS_DPC_WhiteBalance:
                                                vlWhiteBalance.SetSize((uint8_t)varBuffer);
                                                break;
					case EOS_DPC_PictureStyle:
                                                vlPictureStyle.SetSize((uint8_t)varBuffer);
                                                break;
					case EOS_DPC_Iso:
                                                vlIso.SetSize((uint8_t)varBuffer);
                                                break;
					case EOS_DPC_ExposureCompensation:
                                                vlExpCompensation.SetSize((uint8_t)varBuffer);
                                                break;
                                        };
				}
				break;
			// C18A/enumType == 3 - Enumerator Values
			default:
				if (eosEvent.eventCode == EOS_EC_DevPropValuesAccepted)
				{
                                        switch (eosEvent.propCode)
                                        {
					case EOS_DPC_Aperture:
                                                vlAperture.Set(paramCount-5, (uint8_t)varBuffer);
                                                break;
					case EOS_DPC_ShutterSpeed:
                                                vlShutterSpeed.Set(paramCount-5, (uint8_t)varBuffer);
                                                break;
					case EOS_DPC_WhiteBalance:
                                                vlWhiteBalance.Set(paramCount-5, (uint8_t)varBuffer);
                                                break;
					case EOS_DPC_PictureStyle:
                                                vlPictureStyle.Set(paramCount-5, (uint8_t)varBuffer);
                                                break;
					case EOS_DPC_ExposureCompensation:
                                                vlExpCompensation.Set(paramCount-5, (uint8_t)varBuffer);
                                                break;
					case EOS_DPC_Iso:
                                                vlIso.Set(paramCount-5, (uint8_t)varBuffer);
                                                break;
                                        } // switch (eosEvent.propCode)
				}
			} // switch (paramCount)
		} // for
		nRecStage ++;
	case 2:
		if (nRecSize)
			if (!byteSkipper.Skip(pp, pcntdn, nRecSize))
				return false;

		nRecSize = 0;
		nRecStage = 0;
	}
	return true;
}

void EOSEventParser::InitEOSEventStruct()
{
	eosEvent.eventCode	= constInitialEventCode;
	eosEvent.propCode	= 0;
	eosEvent.propValue	= 0;
}


void EOSEventParser::Parse(const uint16_t len, const uint8_t *pbuf, const uint32_t &offset)
{
	uint8_t		*p		= (uint8_t*) pbuf;
	uint16_t	cntdn	= len;

	switch (nStage)
	{
	case 0:
		p	+= 12;
		cntdn	-= 12;
		nStage	++;
	case 1:
		theBuffer.valueSize = 4;
		valueParser.Initialize(&theBuffer);
		InitEOSEventStruct();
		nStage ++;
	case 2:
		while (1)
		{
			if (!EventRecordParse(&p, &cntdn))
				return;
			if (IsLastEventRecord())
				break;
			InitEOSEventStruct();
		}
		nStage = 0;
	}
}
