#ifndef __CANONPS_H__
#define __CANONPS_H__

#include "ptp.h"

// PTP Operation Codes (PowerSot specific)
#define PTP_OC_PS_GetObjectSize				0x9001
#define PTP_OC_PS_StartShootingMode			0x9008
#define PTP_OC_PS_EndShootingMode			0x9009
#define PTP_OC_PS_ViewfinderOn				0x900B
#define PTP_OC_PS_ViewfinderOff				0x900C
#define PTP_OC_PS_ReflectChanges			0x900D
#define PTP_OC_PS_CheckEvent				0x9013
#define PTP_OC_PS_FocusLock					0x9014
#define PTP_OC_PS_FocusUnlock				0x9015
#define PTP_OC_PS_InitiateCaptureInMemory	0x901A
#define PTP_OC_PS_GetPartialObject			0x901B
#define PTP_OC_PS_GetViewfinderImage		0x901d
#define PTP_OC_PS_GetChanges				0x9020
#define PTP_OC_PS_GetFolderEntries			0x9021
 
// PTP PowerShot Extention Events
#define PTP_EC_PS_ObjectCreated				0xC008
#define PTP_EC_PS_RequestObjectTransfer		0xC009
#define PTP_EC_PS_CameraModeChanged			0xC00C

 // PTP Device Properties
//#define PTP_DPC_EOS_CameraDescription		0xD402

// Non-PTP Device properties
#define PS_DPC_BeepMode						0xD001
#define PS_DPC_ViewfinderMode				0xD003
#define PS_DPC_ImageQuality					0xD006
#define PS_DPC_D007							0xD007
#define PS_DPC_ImageSize					0xD008
#define PS_DPC_FlashMode					0xD00A
#define PS_DPC_TvAvSetting					0xD00C
#define PS_DPC_MeteringMode					0xD010
#define PS_DPC_MacroMode					0xD011
#define PS_DPC_FocusingPoint				0xD012
#define PS_DPC_WhiteBalance					0xD013
#define PS_DPC_ISOSpeed						0xD01C
#define PS_DPC_Aperture						0xD01D
#define PS_DPC_ShutterSpeed					0xD01E
#define PS_DPC_ExpCompensation				0xD01F
#define PS_DPC_D029							0xD029
#define PS_DPC_Zoom							0xD02A
#define PS_DPC_SizeQualityMode				0xD02C
#define PS_DPC_FlashMemory					0xD031
#define PS_DPC_CameraModel					0xD032
#define PS_DPC_CameraOwner					0xD033
#define PS_DPC_UnixTime						0xD034
#define PS_DPC_ViewfinderOutput				0xD036
#define PS_DPC_RealImageWidth				0xD039
#define PS_DPC_PhotoEffect					0xD040
#define PS_DPC_AssistLight					0xD041
#define PS_DPC_D045							0xD045 

// Non-PTP Events
#define EOS_EC_DevPropChanged				0xC189
#define EOS_EC_ObjectCreated				0xC186

class CanonPS : public PTP //, public Camera
{
public:
	CanonPS(uint8_t addr, uint8_t epin, uint8_t epout, uint8_t epint, uint8_t nconf, PTPMAIN pfunc);

	uint16_t Initialize(bool binit);

	uint16_t Capture();
	//uint16_t StartBulb();
	//uint16_t StopBulb();
	//uint16_t SwitchLiveView(bool on);
	//uint16_t MoveFocus(uint16_t step);
	//uint16_t SetProperty(uint16_t prop, uint32_t val);


//	uint16_t EOS9110(uint8_t size, void *params);

	uint16_t EventCheck(PTPReadParser *parser);

};

#endif // __CANONPS_H__