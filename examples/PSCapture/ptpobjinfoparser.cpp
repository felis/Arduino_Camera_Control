#include "ptpobjinfoparser.h"

const char* PTPObjInfoParser::acNames[] PROGMEM = 
{
	msgUndefined,
	msgAssociation,	
	msgScript,		
	msgExecutable,	
	msgText,			
	msgHTML,			
	msgDPOF,			
	msgAIFF,	 		
	msgWAV,			
	msgMP3,		
	msgAVI,			
	msgMPEG,			
	msgASF,			
	msgQT			
};

const char* PTPObjInfoParser::imNames[] PROGMEM = 
{
	msgUndefined,
	msgEXIF_JPEG,			
	msgTIFF_EP,			
	msgFlashPix,			
	msgBMP,				
	msgCIFF,				
	msgUndefined_0x3806,	
	msgGIF,				
	msgJFIF,				
	msgPCD,				
	msgPICT,				
	msgPNG,				
	msgUndefined_0x380C,	
	msgTIFF,				
	msgTIFF_IT,			
	msgJP2,				
	msgJPX,				
};

void PTPObjInfoParser::PrintFormat(uint16_t op)
{
	Serial.print(op, HEX);
	Serial.print("\t");
	//Notify(msgTab);

	if ((((op >> 8) & 0xFF) == 0x30) && ((op & 0xFF) <= (PTP_OFC_QT & 0xFF)))
		Notify((char*)pgm_read_word(&acNames[(op & 0xFF)]));
	else
		if ((((op >> 8) & 0xFF) == 0x38) && ((op & 0xFF) <= (PTP_OFC_JPX & 0xFF)))
			Notify((char*)pgm_read_word(&imNames[(op & 0xFF)]));
		else
		{
			switch (op)
			{
			case MTP_OFC_Undefined_Firmware:
				Notify(msgUndefined_Firmware);
				break;
			case MTP_OFC_Windows_Image_Format:	
				Notify(msgWindows_Image_Format);
				break;
			case MTP_OFC_Undefined_Audio:			
				Notify(msgUndefined_Audio);
				break;
			case MTP_OFC_WMA:						
				Notify(msgWMA);
				break;
			case MTP_OFC_OGG:						
				Notify(msgOGG);
				break;
			case MTP_OFC_AAC:						
				Notify(msgAAC);
				break;
			case MTP_OFC_Audible:					
				Notify(msgAudible);
				break;
			case MTP_OFC_FLAC:					
				Notify(msgFLAC);
				break;
			case MTP_OFC_Undefined_Video:			
				Notify(msgUndefined_Video);
				break;
			case MTP_OFC_WMV:						
				Notify(msgWMV);
				break;
			case MTP_OFC_MP4_Container:			
				Notify(msgMP4_Container);
				break;
			case MTP_OFC_MP2:						
				Notify(msgMP2);
				break;
			case MTP_OFC_3GP_Container:
				Notify(msg3GP_Container);
				break;
			default:
				Notify(PSTR("Vendor defined"));
			}
		}
	Notify(PSTR("\r\n"));
}

void PTPObjInfoParser::Parse(const uint16_t len, const uint8_t *pbuf, const uint32_t &offset)
{
	uint16_t	cntdn	= (uint16_t)len;
	uint8_t		*p	= (uint8_t*)pbuf;

	switch (nStage)
	{
	case 0:
		p	+= 12;
		cntdn	-= 12;
		nStage	++;
	case 1:
		Notify(PSTR("Storage ID:\t\t"));
		theBuffer.valueSize = 4;
		valueParser.Initialize(&theBuffer);
		nStage	++;
	case 2:
		if (!valueParser.Parse(&p, &cntdn))
			return;
		PrintHex<uint32_t>(*((uint32_t*)theBuffer.pValue));
		nStage	++;
	case 3:
		Notify(PSTR("\r\nObject Format:\t\t"));
		theBuffer.valueSize = 2;
		valueParser.Initialize(&theBuffer);
		nStage	++;
	case 4:
		if (!valueParser.Parse(&p, &cntdn))
			return;
		PrintFormat(*((uint16_t*)theBuffer.pValue));
		nStage	++;
	case 5:
		Notify(PSTR("Protection Status:\t"));
		theBuffer.valueSize = 2;
		valueParser.Initialize(&theBuffer);
		nStage	++;
	case 6:
		if (!valueParser.Parse(&p, &cntdn))
			return;
		PrintHex<uint16_t>(*((uint16_t*)theBuffer.pValue));
		nStage	++;
	case 7:
		Notify(PSTR("\r\nObject Compressed Size:\t"));
		theBuffer.valueSize = 4;
		valueParser.Initialize(&theBuffer);
		nStage	++;
	case 8:
		if (!valueParser.Parse(&p, &cntdn))
			return;
		PrintHex<uint32_t>(*((uint32_t*)theBuffer.pValue));
		nStage	++;
	case 9:
		Notify(PSTR("\r\nThumb Format:\t\t"));
		theBuffer.valueSize = 2;
		valueParser.Initialize(&theBuffer);
		nStage	++;
	case 10:
		if (!valueParser.Parse(&p, &cntdn))
			return;
		PrintFormat(*((uint16_t*)theBuffer.pValue));
		nStage	++;
	case 11:
		Notify(PSTR("Thumb Compressed Size:\t"));
		theBuffer.valueSize = 4;
		valueParser.Initialize(&theBuffer);
		nStage	++;
	case 12:
		if (!valueParser.Parse(&p, &cntdn))
			return;
		PrintHex<uint32_t>(*((uint32_t*)theBuffer.pValue));
		nStage	++;
	case 13:
		Notify(PSTR("\r\nThumb Pix Width:\t"));
		theBuffer.valueSize = 4;
		valueParser.Initialize(&theBuffer);
		nStage	++;
	case 14:
		if (!valueParser.Parse(&p, &cntdn))
			return;
		PrintHex<uint32_t>(*((uint32_t*)theBuffer.pValue));
		nStage	++;
	case 15:
		Notify(PSTR("\r\nThumb Pix Height:\t"));
		theBuffer.valueSize = 4;
		valueParser.Initialize(&theBuffer);
		nStage	++;
	case 16:
		if (!valueParser.Parse(&p, &cntdn))
			return;
		PrintHex<uint32_t>(*((uint32_t*)theBuffer.pValue));
		nStage	++;
	case 17:
		Notify(PSTR("\r\nImage Pix Width:\t"));
		theBuffer.valueSize = 4;
		valueParser.Initialize(&theBuffer);
		nStage	++;
	case 18:
		if (!valueParser.Parse(&p, &cntdn))
			return;
		PrintHex<uint32_t>(*((uint32_t*)theBuffer.pValue));
		nStage	++;
	case 19:
		Notify(PSTR("\r\nImage Pix Height:\t"));
		theBuffer.valueSize = 4;
		valueParser.Initialize(&theBuffer);
		nStage	++;
	case 20:
		if (!valueParser.Parse(&p, &cntdn))
			return;
		PrintHex<uint32_t>(*((uint32_t*)theBuffer.pValue));
		nStage	++;
	case 21:
		Notify(PSTR("\r\nImage Bit Depth:\t"));
		theBuffer.valueSize = 4;
		valueParser.Initialize(&theBuffer);
		nStage	++;
	case 22:
		if (!valueParser.Parse(&p, &cntdn))
			return;
		PrintHex<uint32_t>(*((uint32_t*)theBuffer.pValue));
		nStage	++;
	case 23:
		Notify(PSTR("\r\nParent Object:\t\t"));
		theBuffer.valueSize = 4;
		valueParser.Initialize(&theBuffer);
		nStage	++;
	case 24:
		if (!valueParser.Parse(&p, &cntdn))
			return;
		PrintHex<uint32_t>(*((uint32_t*)theBuffer.pValue));
		nStage	++;
	case 25:
		Notify(PSTR("\r\nAssociation Type:\t"));
		theBuffer.valueSize = 2;
		valueParser.Initialize(&theBuffer);
		nStage	++;
	case 26:
		if (!valueParser.Parse(&p, &cntdn))
			return;
		PrintHex<uint16_t>(*((uint16_t*)theBuffer.pValue));
		nStage	++;
	case 27:
		Notify(PSTR("\r\nAssociation Desc:\t"));
		theBuffer.valueSize = 4;
		valueParser.Initialize(&theBuffer);
		nStage	++;
	case 28:
		if (!valueParser.Parse(&p, &cntdn))
			return;
		PrintHex<uint32_t>(*((uint32_t*)theBuffer.pValue));
		nStage	++;
	case 29:
		Notify(PSTR("\r\nSequence Number:\t"));
		theBuffer.valueSize = 4;
		valueParser.Initialize(&theBuffer);
		nStage	++;
	case 30:
		if (!valueParser.Parse(&p, &cntdn))
			return;
		PrintHex<uint32_t>(*((uint32_t*)theBuffer.pValue));
		nStage	++;
	case 31:
		Notify(PSTR("\r\nFile Name:\t\t"));
		arrayParser.Initialize(1, 2, &theBuffer);
		nStage	++;
	case 32:
		if (!arrayParser.Parse(&p, &cntdn, (PTP_ARRAY_EL_FUNC)&PrintChar))
			return;
		nStage	++;
	case 33:
		Notify(PSTR("\r\nCapture Date:\t\t"));
		arrayParser.Initialize(1, 2, &theBuffer);
		nStage	++;
	case 34:
		if (!arrayParser.Parse(&p, &cntdn, (PTP_ARRAY_EL_FUNC)&PrintChar))
			return;
		nStage	++;
	case 35:
		Notify(PSTR("\r\nModification Date:\t"));
		arrayParser.Initialize(1, 2, &theBuffer);
		nStage	++;
	case 36:
		if (!arrayParser.Parse(&p, &cntdn, (PTP_ARRAY_EL_FUNC)&PrintChar))
			return;
		nStage	++;
	case 37:
		Notify(PSTR("\r\nKeywords:\t"));
		arrayParser.Initialize(1, 2, &theBuffer);
		nStage	++;
	case 38:
		if (!arrayParser.Parse(&p, &cntdn, (PTP_ARRAY_EL_FUNC)&PrintChar))
			return;
		Notify(PSTR("\r\n"));
		nStage	= 0;
	}
}
