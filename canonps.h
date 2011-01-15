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
#ifndef __CANONPS_H__
#define __CANONPS_H__

#include "ptp.h"

// PTP Operation Codes (PowerShot specific)
#define PS_OC_GetObjectSize					0x9001
#define PS_OC_StartShootingMode				0x9008
#define PS_OC_EndShootingMode				0x9009
#define PS_OC_ViewfinderOn					0x900B
#define PS_OC_ViewfinderOff					0x900C
#define PS_OC_ReflectChanges				0x900D
#define PS_OC_CheckEvent					0x9013
#define PS_OC_FocusLock						0x9014
#define PS_OC_FocusUnlock					0x9015
#define PS_OC_InitiateCaptureInMemory		0x901A
#define PS_OC_GetPartialObject				0x901B
#define PS_OC_GetViewfinderImage			0x901d
#define PS_OC_GetChanges					0x9020
#define PS_OC_GetFolderEntries				0x9021
 
// PTP PowerShot Extention Events
#define PS_EC_ShutDownCFDoorWasOpened		0xC001		/* The Device has shut down due to the opening of the SD card cover.*/
#define PS_EC_ResetHwError					0xC005		/* The device has generated a hardware error. */
#define PS_EC_AbortPCEvf					0xC006		/* The Viewfinder mode has been cancelled. */
#define PS_EC_EnablePCEvf					0xC007		/* The Viewfinder mode has been enablede. */
#define PS_EC_FullViewReleased				0xC008		/* Transfer timing of main image data */
#define PS_EC_ThumbnailReleased				0xC009		/* Transfer timing of thumbnail image data */
#define PS_EC_ChangeBatteryStatus			0xC00A		/* The power condition of the camera has changed. */
#define PS_EC_PushedReleaseSw				0xC00B		/* User has pressed the release swtich on camera. */
#define PS_EC_PropertyChanged				0xC00C		/* A group of properties relating to release control have been changed. */
#define PS_EC_RotationAngleChanged			0xC00D		/* The angle of rotation of the camera has been changed. */
#define PS_EC_ChangedByCamUI				0xC00E		/* An operation control on the camera has been operated.*/
#define PS_EC_Shutdown						0xD001		/* Shutdown */
#define PS_EC_StartDirectTransfer			0xC011
#define PS_EC_StopDirectTransfer			0xC013

// PowerShot-specific Device Properties
#define PS_DPC_BeepMode						0xD001
#define PS_DPC_BatteryKind					0xD002
#define PS_DPC_BatteryStatus				0xD003
#define PS_DPC_UILockType					0xD004
#define PS_DPC_CameraMode					0xD005
#define PS_DPC_ImageQuality					0xD006
#define PS_DPC_FullViewFileFormat			0xD007
#define PS_DPC_ImageSize					0xD008
#define PS_DPC_SelfTime						0xD009
#define PS_DPC_FlashMode					0xD00A
#define PS_DPC_Beep							0xD00B
#define PS_DPC_ShootingMode					0xD00C
#define PS_DPC_ImageMode					0xD00D
#define PS_DPC_DriveMode					0xD00E
#define PS_DPC_EZoom						0xD00F
#define PS_DPC_MeteringMode					0xD010
#define PS_DPC_AFDistance					0xD011
#define PS_DPC_FocusingPoint				0xD012
#define PS_DPC_WhiteBalance					0xD013
#define PS_DPC_SlowShutterSetting			0xD014
#define PS_DPC_AFMode						0xD015
#define PS_DPC_ImageStabilization			0xD016
#define PS_DPC_Contrast						0xD017
#define PS_DPC_ColorGain					0xD018
#define PS_DPC_Sharpness					0xD019
#define PS_DPC_Sensitivity					0xD01A
#define PS_DPC_ParameterSet					0xD01B
#define PS_DPC_ISOSpeed						0xD01C
#define PS_DPC_Aperture						0xD01D
#define PS_DPC_ShutterSpeed					0xD01E
#define PS_DPC_ExpCompensation				0xD01F
#define PS_DPC_FlashCompensation			0xD020
#define PS_DPC_AEBExposureCompensation		0xD021
#define PS_DPC_AvOpen						0xD023
#define PS_DPC_AvMax						0xD024
#define PS_DPC_FocalLength					0xD025
#define PS_DPC_FocalLengthTele				0xD026
#define PS_DPC_FocalLengthWide				0xD027
#define PS_DPC_FocalLengthDenominator		0xD028
#define PS_DPC_CaptureTransferMode			0xD029
#define PS_DPC_Zoom							0xD02A
#define PS_DPC_NamePrefix					0xD02B
#define PS_DPC_SizeQualityMode				0xD02C
#define PS_DPC_SupportedThumbSize			0xD02D
#define PS_DPC_SizeOfOutputDataFromCamera	0xD02E
#define PS_DPC_SizeOfInputDataToCamera		0xD02F
#define PS_DPC_RemoteAPIVersion				0xD030
#define PS_DPC_FirmwareVersion				0xD031
#define PS_DPC_CameraModel					0xD032
#define PS_DPC_CameraOwner					0xD033
#define PS_DPC_UnixTime						0xD034
#define PS_DPC_CameraBodyID					0xD035
#define PS_DPC_CameraOutput					0xD036
#define PS_DPC_DispAv						0xD037
#define PS_DPC_AvOpenApex					0xD038
#define PS_DPC_DZoomMagnification			0xD039
#define PS_DPC_MlSpotPos					0xD03A
#define PS_DPC_DispAvMax					0xD03B
#define PS_DPC_AvMaxApex					0xD03C
#define PS_DPC_EZoomStartPosition			0xD03D
#define PS_DPC_FocalLengthOfTele			0xD03E
#define PS_DPC_EZoomSizeOfTele				0xD03F
#define PS_DPC_PhotoEffect					0xD040
#define PS_DPC_AssistLight					0xD041
#define PS_DPC_FlashQuantityCount			0xD042
#define PS_DPC_RotationAngle				0xD043
#define PS_DPC_RotationScene				0xD044
#define PS_DPC_EventEmulateMode				0xD045
#define PS_DPC_DPOFVersion					0xD046
#define PS_DPC_TypeOfSupportedSlideShow		0xD047
#define PS_DPC_AverageFilesizes				0xD048
#define PS_DPC_ModelID						0xD049


class PSStateHandlers : public PTPStateHandlers
{
public:
      virtual void OnSessionOpenedState(PTP *ptp);
};



class CanonPS : public PTP
{
public:
	// ISO Speed Values
	enum { IsoAuto = 0, Iso80 = 0x45, Iso100 = 0x48, Iso200 = 0x50, Iso400 = 0x58, Iso800=0x60 };

	// White Balance Values
	enum { WbAuto = 0, WbSunny, WbCloudy, WbTungsten, WbFluorescent, WbFlash, WbCustom, WbUnknown };

	// Exposure Compensation Values (same values for both exposure compensation and flash compensation)
	enum {	ExpCompDown2	= 0x08, ExpCompDown1_2d3= 0x0B, ExpCompDown1_1d3= 0x0D, ExpCompDown1 = 0x10, ExpCompDown2d3 = 0x13, 
			ExpCompDown1d3	= 0x15, ExpComp_0		= 18,	ExpCompUp1d3	= 0x1B, ExpCompUp2d3 = 0x1D, ExpCompUp1		= 0x20, 
			ExpCompUp1_1d3	= 0x23, ExpCompUp1_2d3	= 0x25, ExpCompUp2		= 0x28 };

	// Image Quality Values
	enum { ImgQualityNormal = 2, ImageQualityFine = 3, ImageQualitySuperb = 5 };

	// Image Size Values
	enum { ImgSizeLarge, ImgSizeMedium1, ImgSizeSmall, ImgSizeMedium2 };

	CanonPS(uint8_t addr, uint8_t epin, uint8_t epout, uint8_t epint, uint8_t nconf, PTPStateHandlers *s);

	uint16_t Initialize(bool binit);

	uint16_t Capture();
	uint16_t EventCheck(PTPReadParser *parser);
};

#endif // __CANONPS_H__