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
#ifndef __CANONEOS_H__
#define __CANONEOS_H__

#include "WProgram.h"
#include "ptp.h"

// PTP Operation Codes (EOS specific)
#define EOS_OC_GetStorageIDs				0x9101
#define EOS_OC_GetStorageInfo				0x9102
#define EOS_OC_GetObject					0x9107
#define EOS_OC_GetDeviceInfoEx				0x9108
#define EOS_OC_GetObjectIDs					0x9109
#define EOS_OC_Capture						0x910f
#define EOS_OC_SetDevicePropValue			0x9110
#define EOS_OC_SetPCConnectMode				0x9114
#define EOS_OC_SetExtendedEventInfo			0x9115
#define EOS_OC_GetEvent						0x9116
#define EOS_OC_TransferComplete				0x9117
#define EOS_OC_CancelTransfer				0x9118
#define EOS_OC_ResetTransfer				0x9119
#define EOS_OC_GetDevicePropValue 			0x9127
#define EOS_OC_GetLiveViewPicture			0x9153
#define EOS_OC_MoveFocus					0x9155

// PTP Device Properties
#define EOS_DPC_CameraDescription			0xD402

// Non-PTP Device properties
#define EOS_DPC_Aperture					0xD101
#define EOS_DPC_ShutterSpeed				0xD102
#define EOS_DPC_Iso							0xD103
#define EOS_DPC_ExposureCompensation		0xD104
#define EOS_DPC_ShootingMode				0xD105
#define EOS_DPC_DriveMode					0xD106
#define EOS_DPC_ExpMeterringMode			0xD107
#define EOS_DPC_AFMode						0xD108
#define EOS_DPC_WhiteBalance				0xD109
#define EOS_DPC_PictureStyle				0xD110
#define EOS_DPC_TransferOption				0xD111
#define EOS_DPC_UnixTime					0xD113
#define EOS_DPC_ImageQuality				0xD120
#define EOS_DPC_LiveView					0xD1B0
#define EOS_DPC_AvailableShots  			0xD11B
#define EOS_DPC_CaptureDestination    		0xD11C
#define EOS_DPC_BracketMode           		0xD11D

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


#endif // __CANONEOS_H__