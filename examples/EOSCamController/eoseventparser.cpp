#include "eoseventparser.h"
#include "dataitem.h"
#include "camcontroller.h"

extern DIT_MODE          diMode;
extern DIT_APERTURE      diAperture;
extern DIT_WB            diWb;
extern DIT_SHUTTER_SPEED diShutterSpeed;
extern DIT_PSTYLE        diPStyle;
extern DIT_ISO           diIso;
extern DIT_EXPCOMP       diExpComp;

extern EEPROMByteList    vlAperture;
extern EEPROMByteList    vlShutterSpeed;
extern EEPROMByteList    vlWhiteBalance;
extern EEPROMByteList    vlPictureStyle;
extern EEPROMByteList    vlExpCompensation;
extern EEPROMByteList    vlIso;

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
                                            diAperture.Set(varBuffer);
                                            break;
                                        case EOS_DPC_ShutterSpeed:
                                            diShutterSpeed.Set(varBuffer);
                                            break;
                                        case EOS_DPC_ShootingMode:
                                            diMode.Set(varBuffer);
                                            break;
                                        case EOS_DPC_WhiteBalance:
                                            diWb.Set(varBuffer);
                                            break;
                                        case EOS_DPC_PictureStyle:
                                            diPStyle.Set(varBuffer);
                                            break;
                                        case EOS_DPC_Iso:
                                            diIso.Set(varBuffer);
                                            break;
                                        case EOS_DPC_ExposureCompensation:
                                            diExpComp.Set(varBuffer);
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
	uint8_t		*p	= (uint8_t*) pbuf;
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
