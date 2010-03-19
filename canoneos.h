#ifndef __CANONEOS_H__
#define __CANONEOS_H__

#include "ptp.h"

/* Canon EOS commands. Checked to work with following cameras:
  400D
  450D (XSi)
  
  Certain commands are not supported by all cameras
  */


// PTP Operation Codes (EOS specific)
#define PTP_OC_EOS_GetStorageIDs			0x9101
#define PTP_OC_EOS_GetStorageInfo			0x9102
#define PTP_OC_EOS_GetObject				0x9107
#define PTP_OC_EOS_GetDeviceInfo			0x9108
#define PTP_OC_EOS_GetObjectIDs				0x9109
#define PTP_OC_EOS_Capture					0x910f
#define PTP_OC_EOS_SetDevicePropValue		0x9110
#define PTP_OC_EOS_SetPCConnectMode			0x9114
#define PTP_OC_EOS_SetExtendedEventInfo		0x9115
#define PTP_OC_EOS_GetEvent					0x9116
#define PTP_OC_EOS_GetLiveViewPicture		0x9153
#define PTP_OC_EOS_MoveFocus				0x9155

// PTP Device Properties
#define PTP_DPC_EOS_CameraDescription		0xD402

// Non-PTP Device properties
#define EOS_DPC_Aperture					0xD101
#define EOS_DPC_Exposure					0xD102
#define EOS_DPC_Iso							0xD103
#define EOS_DPC_ExposureCorrection			0xD104
#define EOS_DPC_DriveMode					0xD106
#define EOS_DPC_ExpMeterringMode			0xD107
#define EOS_DPC_AFMode						0xD108
#define EOS_DPC_WhiteBalance				0xD109
#define EOS_DPC_PictureStyle				0xD110
#define EOS_DPC_AFFocusPoint				0xD113
#define EOS_DPC_ImageQuality				0xD120
#define EOS_DPC_LiveView					0xD1B0

// Non-PTP Events
#define EOS_EC_DevPropChanged				0xC189
#define EOS_EC_ObjectCreated				0xC186

class CanonEOS : public PTP //, public Camera
{
public:
	CanonEOS(uint8_t addr, uint8_t epin, uint8_t epout, uint8_t epint, uint8_t nconf, PTPMAIN pfunc);

	uint16_t Initialize(bool binit);

	uint16_t Test();

	uint16_t Capture();
	uint16_t StartBulb();
	uint16_t StopBulb();
	//uint16_t SwitchLiveView(bool on);
	//uint16_t MoveFocus(uint16_t step);
	uint16_t SetProperty(uint16_t prop, uint32_t val);


	uint16_t EOS9110(uint8_t size, void *params);

	//uint16_t EOS911A(uint32_t *p);
	//uint16_t EOS911B();
	//uint16_t EOS911C();
	//uint16_t EOS9125();
	//uint16_t EOS9126();
	//uint16_t EOS9155(uint32_t *p);
	//uint16_t EOS9158(uint32_t *p);

	//uint16_t EOSSet();
	//uint16_t EOSSetEventMask();
	virtual uint16_t EventCheck(PTPReadParser *parser);

	//// Aperture
	//virtual uint16_t GetAperture(uint8_t val);
	//virtual uint16_t SetAperture(uint8_t val);

	//// Exposure
	//virtual uint16_t GetExposure(uint8_t val);
	//virtual uint16_t SetExposure(uint8_t val);

	//// ISO
	//virtual uint16_t GetISO(uint8_t val);
	//virtual uint16_t SetISO(uint8_t val);

	//// Image quality
	//virtual uint16_t GetImgQuality(uint8_t val);
	//virtual uint16_t SetImgQuality(uint8_t val);

	//// White balance
	//virtual uint16_t GetWB(uint8_t val);
	//virtual uint16_t SetWB(uint8_t val);

	//// Exposure metering mode
	//virtual uint16_t GetMtrMode(uint8_t val);
	//virtual uint16_t SetMtrMode(uint8_t val);

	//// Exposure compensation
	//virtual uint16_t GetExpComp(uint8_t val);
	//virtual uint16_t SetExpComp(uint8_t val);

	//virtual uint16_t Capture();
};

enum ExpCorrection
{
	Plus3		= 0x18,			//"+3"));					
	Plus2_75	= 0x15,			//"+2 2/3"));				
	Plus2_5		= 0x14,			//"+2 1/2"));				
	Plus2_25	= 0x13,			//"+2 1/3"));				
	Plus2		= 0x10,			//"+2"));					
	Plus1_75	= 0x0d,			//"+1 2/3"));				
	Plus1_5		= 0x0c,			//"+1 1/2"));				
	Plus1_25	= 0x0b,			//"+1 1/3"));				
	Plus1		= 0x08,			//"+1"));					
	Plus0_75	= 0x05,			//"+2/3"));					
	Plus0_5		= 0x04,			//"+1/2"));					
	Plus0_25	= 0x03,			//"+1/3"));					
	Zerro		= 0x00,			//"0"));						
	Minus0_25	= 0xfd,			//"-1/3"));					
	Minus0_5	= 0xfc,			//"-1/2"));					
	Minus0_75	= 0xfb,			//"-2/3"));					
	Minus1		= 0xf8,			//"-1"));					
	Minus1_25	= 0xf5,			//"-1 1/3"));				
	Minus1_5	= 0xf4,			//"-1 1/2"));				
	Minus1_75	= 0xf3,			//"-1 2/3"));				
	Minus2		= 0xf0,			//"-2"));					
	Minus2_25	= 0xed,			//"-2 1/3"));				
	Minus2_5	= 0xec,			//"-2 1/2"));				
	Minus2_75	= 0xeb,			//"-2 2/3"));				
	Minus3		= 0xe8 //,			//"-3"));					
//	Unknown		= 0xffffffff	//,"unkown"));
};
//	
enum Apperture
{
	F0		= 0x00,		//"00"));//									
	F1		= 0x08,		//"1"));//									
	F1_1	= 0x0B,		//"1.1"));//									
	F1_2	= 0x0C,		//"1.2"));//									
	//F1_2	= 0x0D,		//"1.2"));//									
	F1_4	= 0x10,		//"1.4"));//									
	F1_6	= 0x13,		//"1.6"));//									
	F1_8	= 0x14,		//"1.8"));//									
	//F1_8	= 0x15,		//"1.8"));//									
	F2		= 0x18,		//"2"));//									
	F2_2	= 0x1B,		//"2.2"));//									
	F2_5	= 0x1C,		//"2.5"));//									
	//F2_5	= 0x1D,		//"2.5"));//									
	F2_8	= 0x20,		//"2.8"));//									
	F3_2	= 0x23,		//"3.2"));//									
	F3_5	= 0x24,		//"3.5"));//									
	//F3_5	= 0x25,		//"3.5"));//									
	F4		= 0x28,		//"4"));//									
	F4_5	= 0x2B,		//"4.5"));//									
	//F4_5	= 0x2C,		//"4.5"));//									
	F5		= 0x2D,		//"5.0"));//									
	F5_6	= 0x30,		//"5.6"));//									
	F6_3	= 0x33,		//"6.3"));//									
	F6_7	= 0x34,		//"6.7"));//									
	F7_1	= 0x35,		//"7.1"));//									
	F8		= 0x38,		//"8"));//									
	F9		= 0x3B,		//"9"));//									
	F9_5	= 0x3C,		//"9.5"));//									
	F10		= 0x3D,		//"10"));
	F11		= 0x40,		//"11"));
	F13		= 0x43,		//"13"));
	//F13		= 0x44,		//"13"));
	F14		= 0x45,		//"14"));
	F16		= 0x48,		//"16"));
	F18		= 0x4B,		//"18"));
	F19		= 0x4C,		//"19"));
	F20		= 0x4D,		//"20"));
	F22		= 0x50,		//"22"));
	F25		= 0x53,		//"25"));
	F27		= 0x54,		//"27"));
	F29		= 0x55,		//"29"));
	F32		= 0x58,		//"32"));
	F36		= 0x5B,		//"36"));
	F38		= 0x5C,		//"38"));
	F40		= 0x5D,		//"40"));
	F45		= 0x60,		//"45"));
	F51		= 0x63,		//"51"));
	F54		= 0x64,		//"54"));
	F57		= 0x65,		//"57"));
	F64		= 0x68,		//"64"));
	F72		= 0x6B,		//"72"));
	F76		= 0x6C,		//"76"));
	F80		= 0x6D,		//"80"));
	F91		= 0x70	//,		//"91"));
	//Unknown = 0xffffffff,"unkown"));
};
























































































#endif // __CANONEOS_H__