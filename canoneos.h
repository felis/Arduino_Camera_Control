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
#ifndef __CANONEOS_H__
#define __CANONEOS_H__

#include "ptp.h"

// PTP Operation Codes (EOS specific)
#define PTP_OC_EOS_GetStorageIDs			0x9101
#define PTP_OC_EOS_GetStorageInfo			0x9102
#define PTP_OC_EOS_GetObject				0x9107
#define PTP_OC_EOS_GetDeviceInfoEx			0x9108
#define PTP_OC_EOS_GetObjectIDs				0x9109
#define PTP_OC_EOS_Capture				0x910f
#define PTP_OC_EOS_SetDevicePropValue			0x9110
#define PTP_OC_EOS_SetPCConnectMode			0x9114
#define PTP_OC_EOS_SetExtendedEventInfo			0x9115
#define PTP_OC_EOS_GetEvent				0x9116
#define PTP_OC_EOS_TransferComplete			0x9117
#define PTP_OC_EOS_CancelTransfer			0x9118
#define PTP_OC_EOS_ResetTransfer			0x9119
#define PTP_OC_EOS_GetDevicePropValue 			0x9127
#define PTP_OC_EOS_GetLiveViewPicture			0x9153
#define PTP_OC_EOS_MoveFocus				0x9155

// PTP Device Properties
#define PTP_DPC_EOS_CameraDescription			0xD402

// Non-PTP Device properties
#define EOS_DPC_Aperture            0xD101
#define EOS_DPC_Exposure            0xD102
#define EOS_DPC_Iso                 0xD103
#define EOS_DPC_ExposureCorrection  0xD104
#define EOS_DPC_ShootingMode        0xD105
#define EOS_DPC_DriveMode           0xD106
#define EOS_DPC_ExpMeterringMode    0xD107
#define EOS_DPC_AFMode              0xD108
#define EOS_DPC_WhiteBalance        0xD109
#define EOS_DPC_PictureStyle        0xD110
#define EOS_DPC_TransferOption      0xD111
#define EOS_DPC_UnixTime            0xD113
#define EOS_DPC_AvailableShots      0xD11B
#define EOS_DPC_CaptureDestination  0xD11C  //2-camera, 4-host, 6-both
#define EOS_DPC_BracketMode         0xD11D
#define EOS_DPC_ImageQuality        0xD120
#define EOS_DPC_LiveView            0xD1B0  //0-off, 1-camera LCD, 2-PC
#define EOS_DPC_EVFMode             0xD1B1
#define EOS_DPC_EVFSharpness        0xD1B3
#define EOS_DPC_EVFRecordStatus     0xD1B8  // 4 = start, 0 = stop
#define EOS_DPC_LvViewTypeSelect    0xD1BC
#define EOS_DPC_FocusInfoEx         0xD1D3
#define EOS_DPC_LensID              0xD1DD

// Non-PTP Events
#define EOS_EC_DevPropChanged				0xC189
#define EOS_EC_ObjectCreated				0xC181
#define EOS_EC_DevPropValuesAccepted		0xC18A
#define EOS_EC_Capture						0xC18B
#define EOS_EC_HalfPushReleaseButton		0xC18E



class EOSStateHandlers : public PTPStateHandlers
{
public:
      virtual void OnSessionOpenedState(PTP *ptp);
};


class ImgQualitySupplier : PTPDataSupplier
{
	uint32_t	pictFormat;

public:
	ImgQualitySupplier() {};
	void SetPictureFormat(uint32_t format) { pictFormat = format; };
	virtual uint32_t GetDataSize();
	virtual void GetData(const uint16_t len, uint8_t *pbuf);
};

class CanonEOS : public PTP
{
public:
	enum { modeADEP=5, modeM=3, modeAv=2, modeTv=1, modeP=0, modeAuto=9, modePortrait=0xC, modeLandscape=0xD, 
			modeMacro=0xE, modeSport=0xB, modeNight=0xA, modeFlashOff=0xF};

	enum { driveSingleShot = 0, driveSelf = 1, driveContinuous = 0x10 };

	enum { iso100 = 0x48, iso200 = 0x50, iso400 = 0x58, iso800 = 0x60, iso1600 = 0x68 };

	enum { styleStandard = 0x81, stylePortrait, styleLandscape, styleNeutral, styleFaithful, styleMonochrome,
			styleUser1 = 0x21, styleUser2, styleUser3 };

	enum { afmodeOneShot, afmodeAIServo, afmodeAIFocus };

	enum { pictSFine = 0x00000321, pictSNormal = 0x00000221, pictMFine = 0x00000311, pictMNormal = 0x00000211,
			pictLFine = 0x00000301, pictLNormal = 0x00000201, pictRaw = 0x00000406, pictRawPlusL = 0x00301406 };

	enum { wbAuto, wbDaylight, wbCloudy, wbTungsten, wbFluorescent, wbStrobe, wbWhitePaper, wbShade = 8 };

	enum 
	{
		ecPlus2		= 0x10,			//"+2"
		ecPlus1_2d3	= 0x0d,			//"+1 2/3"
		ecPlus1_1d3	= 0x0b,			//"+1 1/3"
		ecPlus1		= 0x08,			//"+1"
		ecPlus0_2d3	= 0x05,			//"+2/3"
		ecPlus0_1d3	= 0x03,			//"+1/3"
		ecZerro		= 0x00,			//"0"						
		ecMinus0_1d3= 0xfd,			//"-1/3"
		ecMinus0_2d3= 0xfb,			//"-2/3"
		ecMinus1	= 0xf8,			//"-1"					
		ecMinus1_1d3= 0xf5,			//"-1 1/3"
		ecMinus1_2d3= 0xf3,			//"-1 2/3"
		ecMinus2	= 0xf0,			//"-2"					
	};

 	CanonEOS(uint8_t addr, uint8_t epin, uint8_t epout, uint8_t epint, uint8_t nconf, PTPStateHandlers *s);

	uint16_t SetPCConnectMode(uint8_t mode);
	uint16_t SetExtendedEventInfo(uint8_t mode);
	uint16_t Capture();
	uint16_t StartBulb();
	uint16_t StopBulb();
	uint16_t SwitchLiveView(bool on);
	uint16_t MoveFocus(uint16_t step);
	uint16_t SetProperty(uint16_t prop, uint32_t val);

	uint16_t GetProperty(uint16_t prop, PTPReadParser *parser);

	uint16_t GetDeviceInfoEx(PTPReadParser *parser);
	uint16_t SetImageQuality(uint32_t format);

	uint16_t GetObject(uint32_t object_id, uint32_t parent_id, PTPReadParser *parser);
	uint16_t ResetTransfer(uint32_t object_id);
	uint16_t CancelTransfer(uint32_t object_id);

	virtual uint16_t EventCheck(PTPReadParser *parser);
	uint16_t DigitalZoom(uint16_t magnify);

};


template <class ValueType, const uint16_t ListSize>
class ValueList
{
public:
	uint16_t	listSize;
	ValueType	valueList[ListSize];
	ValueType	currentValue;

	ValueType GetNext()
	{
		for (uint16_t i=0; i<listSize; i++)
			if (valueList[i] == currentValue)
				return ((i+1 < listSize) ? valueList[i+1] : currentValue);
		
		return currentValue;
	};

	ValueType GetPrev()
	{
		for (uint16_t i=0; i<listSize; i++)
			if (valueList[i] == currentValue)
				return ((i-1 >= 0) ? valueList[i-1] : currentValue);
		
		return currentValue;
	};
};



#define MAX_APERTURE_LIST_SIZE			32

#define MAX_SHUTTER_SPEED_LIST_SIZE		64


typedef ValueList<uint8_t, MAX_APERTURE_LIST_SIZE>			APERTURE_VALUE_LIST;
typedef ValueList<uint8_t, MAX_SHUTTER_SPEED_LIST_SIZE>		SHUTTER_SPEED_VALUE_LIST;

template <class ValueType, const uint8_t TitleSize> 
struct ValueTitle
{
	ValueType	value;
	const char	title[TitleSize];
};

template <class ValueType, const uint8_t TitleSize> 
const char* FindTitle(uint8_t size, const ValueTitle<ValueType, TitleSize> *p, ValueType val)
{
	for (int i=0; i<size; i++)
	{
		if (pgm_read_byte(&(p[i].value)) == val)
			return (const char*)p[i].title;
	}
	return PSTR("N/A");
}


#include <avr/eeprom.h> 

class EEPROMByteList
{
        const uint16_t  listOffset;
        const uint8_t   maxListSize;
	uint8_t	        listSize;

	uint16_t GetValueAddress(uint8_t val)
	{
                uint16_t tail = listOffset+listSize+2;
                
		for (uint16_t i=listOffset+1; i<tail; i++)
			if (eeprom_read_byte((uint8_t*)i) == val)
                                return i;

		return 0xffff;
	};
        
public:
        EEPROMByteList(uint16_t list_offset, uint16_t max_size) : listOffset(list_offset), maxListSize(max_size), listSize(0)
        {
        };
        
        void SetSize(uint8_t size)
        {
                listSize = (size < maxListSize) ? size : maxListSize;
                
                if (eeprom_read_byte((uint8_t*) listOffset) != listSize) 
                        eeprom_write_byte((uint8_t*) listOffset, listSize);
        };
        
        uint8_t GetSize()
        {
            return listSize;
        };
        
        
        uint8_t Get(uint8_t i)
        {
             return (eeprom_read_byte((uint8_t*)(listOffset + 1 + ((i < listSize) ? i : listOffset+listSize-1))));
        };
        
        void Set(uint8_t i, uint8_t val)
        {
            if (i < listSize)
            {
                uint16_t pos = listOffset + i + 1;
                
                if (eeprom_read_byte((uint8_t*) pos) != val)
                    eeprom_write_byte((uint8_t*) pos, val);
            }
        };
        
	uint8_t GetNext(uint8_t val, uint8_t di=1)
	{
		uint16_t addr = GetValueAddress(val);

		uint16_t tail = listOffset+listSize;

		if (addr == 0xffff)
			return eeprom_read_byte((uint8_t*)tail);

		addr += di;

	    	return eeprom_read_byte((uint8_t*)((addr > tail) ? tail : addr));
	};

	uint8_t GetPrev(uint8_t val, uint8_t di=1)
	{
		uint16_t addr = GetValueAddress(val);

		if (addr == 0xffff)
			return eeprom_read_byte((uint8_t*)(listOffset+1));

		addr -= di;

	    	return eeprom_read_byte((uint8_t*)((addr <= listOffset) ? listOffset+1 : addr));
	};
};

#endif // __CANONEOS_H__