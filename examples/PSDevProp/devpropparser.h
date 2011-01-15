#ifndef __DEVPROPPARSER_H__
#define __DEVPROPPARSER_H__

#include <inttypes.h>
#include <avr/pgmspace.h>
#include <../ptp/ptpconst.h>
#include <../ptp/ptp.h>

// Device properties
const char msgUndefined					[] PROGMEM = "Undefined";			
const char msgBatteryLevel				[] PROGMEM = "BatteryLevel";				
const char msgFunctionalMode			[] PROGMEM = "FunctionalMode";				
const char msgImageSize					[] PROGMEM = "ImageSize";					
const char msgCompressionSetting		[] PROGMEM = "CompressionSetting";			
const char msgWhiteBalance				[] PROGMEM = "WhiteBalance";				
const char msgRGBGain					[] PROGMEM = "RGBGain";						
const char msgFNumber					[] PROGMEM = "FNumber";						
const char msgFocalLength				[] PROGMEM = "FocalLength";					
const char msgFocusDistance				[] PROGMEM = "FocusDistance";				
const char msgFocusMode					[] PROGMEM = "FocusMode";					
const char msgExposureMeteringMode		[] PROGMEM = "ExposureMeteringMode";		
const char msgFlashMode					[] PROGMEM = "FlashMode";					
const char msgExposureTime				[] PROGMEM = "ExposureTime";				
const char msgExposureProgramMode		[] PROGMEM = "ExposureProgramMode";			
const char msgExposureIndex				[] PROGMEM = "ExposureIndex";				
const char msgExposureBiasCompensation	[] PROGMEM = "ExposureBiasCompensation";	
const char msgDateTime					[] PROGMEM = "DateTime";					
const char msgCaptureDelay				[] PROGMEM = "CaptureDelay";				
const char msgStillCaptureMode			[] PROGMEM = "StillCaptureMode";			
const char msgContrast					[] PROGMEM = "Contrast";					
const char msgSharpness					[] PROGMEM = "Sharpness";					
const char msgDigitalZoom				[] PROGMEM = "DigitalZoom";					
const char msgEffectMode				[] PROGMEM = "EffectMode";					
const char msgBurstNumber				[] PROGMEM = "BurstNumber";					
const char msgBurstInterval				[] PROGMEM = "BurstInterval";				
const char msgTimelapseNumber			[] PROGMEM = "TimelapseNumber";				
const char msgTimelapseInterval			[] PROGMEM = "TimelapseInterval";			
const char msgFocusMeteringMode			[] PROGMEM = "FocusMeteringMode";			
const char msgUploadURL					[] PROGMEM = "UploadURL";					
const char msgArtist					[] PROGMEM = "Artist";					
const char msgCopyrightInfo				[] PROGMEM = "CopyrightInfo";				

// Data Types
const char msgUNDEF		[] PROGMEM = "UNDEF";
const char msgINT8		[] PROGMEM = "INT8";
const char msgUINT8		[] PROGMEM = "UINT8";
const char msgINT16		[] PROGMEM = "INT16";
const char msgUINT16	[] PROGMEM = "UINT16";
const char msgINT32		[] PROGMEM = "INT32";
const char msgUINT32	[] PROGMEM = "UINT32";
const char msgINT64		[] PROGMEM = "INT64";
const char msgUINT64	[] PROGMEM = "UINT64";
const char msgINT128	[] PROGMEM = "INT128";
const char msgUINT128	[] PROGMEM = "UINT128";
const char msgAINT8		[] PROGMEM = "AINT8";
const char msgAUINT8	[] PROGMEM = "AUINT8";
const char msgAINT16	[] PROGMEM = "AINT16";
const char msgAUINT16	[] PROGMEM = "AUINT16";
const char msgAINT32	[] PROGMEM = "AINT32";
const char msgAUINT32	[] PROGMEM = "AUINT32";
const char msgAINT64	[] PROGMEM = "AINT64";
const char msgAUINT64	[] PROGMEM = "AUINT64";
const char msgAINT128	[] PROGMEM = "AINT128";
const char msgAUINT128	[] PROGMEM = "AUINT128";
const char msgSTR	[] PROGMEM = "STR";


class DevPropParser : public PTPReadParser
{
	static const char* dtNames1[];
	static const char* dtNames2[];
	static const char* prNames[];

	uint8_t				nStage;
        uint8_t                         enStage;
	uint16_t			dataType;
        uint8_t				formFlag;
        uint8_t				bytesSize;
        MultiValueBuffer	        theBuffer;
        uint8_t				varBuffer[16];
        uint16_t                        enLen;
        uint16_t                        enLenCntdn;
        
	MultiByteValueParser		valParser;

	void PrintDevProp	(uint8_t **pp, uint16_t *pcntdn);
	void PrintDataType	(uint8_t **pp, uint16_t *pcntdn);
	void PrintGetSet	(uint8_t **pp, uint16_t *pcntdn);
	bool PrintValue		(uint8_t **pp, uint16_t *pcntdn);       
	bool PrintEnum		(uint8_t **pp, uint16_t *pcntdn);       
	bool ParseEnum		(uint8_t **pp, uint16_t *pcntdn);       
	bool ParseEnumSingle	(uint8_t **pp, uint16_t *pcntdn);       
	bool ParseEnumArray	(uint8_t **pp, uint16_t *pcntdn);       
        
	static void PrintChar(const MultiValueBuffer * const p, uint32_t count, const void *me)
	{
		if (((unsigned char*)p->pValue)[0])
			Serial.print(((unsigned char*)p->pValue)[0]);
	};
	static void PrintByte(const MultiValueBuffer * const p, uint32_t count, const void *me)
	{
		if (count)
			Notify(PSTR(","));
		PrintHex<uint8_t>(((uint8_t*)p->pValue)[0]);
	};
	static void PrintTwoBytes(const MultiValueBuffer * const p, uint32_t count, const void *me)
	{
		if (count)
			Notify(PSTR(","));
		PrintHex<uint16_t>(((uint16_t*)p->pValue)[0]);
	};
	static void PrintFourBytes(const MultiValueBuffer * const p, uint32_t count, const void *me)
	{
		if (count)
			Notify(PSTR(","));
		PrintHex<uint32_t>(((uint32_t*)p->pValue)[0]);
	};
	static void PrintEightBytes(const MultiValueBuffer * const p, uint32_t count, const void *me)
	{
		if (count)
			Notify(PSTR(","));
    		for (uint8_t i=p->valueSize; i; i--)
    	  	    PrintHex<uint8_t>(((uint8_t*)p->pValue)[i-1]);
	};
	static void PrintEnumValue(const MultiValueBuffer * const p, uint32_t count, const void *me) 
        { 
            if (count) 
                Serial.print(", "); 
            
            switch (((MultiValueBuffer*)p)->valueSize)
            {
            case 1: 
    		PrintHex<uint8_t>(*((uint8_t*)p->pValue)); 
                break;
            case 2:  
    		PrintHex<uint16_t>(*((uint16_t*)p->pValue)); 
                break;
            case 4:  
    		PrintHex<uint32_t>(*((uint32_t*)p->pValue)); 
                break;
    	    default:
    		for (uint8_t i=p->valueSize; i; i--)
    	  	    PrintHex<uint8_t>(((uint8_t*)p->pValue)[i-1]);
            }
            count++; 
        };
    
        PTPListParser      enumParser;
    
        uint8_t GetDataSize();
        
public:
	DevPropParser() :
		nStage(0),
		enStage(0),
		dataType(0),
		formFlag(0),
		bytesSize(0),
                enLen(0),
                enLenCntdn(0)
	{
		theBuffer.pValue = varBuffer;
	}
	virtual void Parse(const uint16_t len, const uint8_t * const pbuf, const uint32_t &offset);
};

#endif // __DEVPROPPARSER_H__
