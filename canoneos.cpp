#include "canoneos.h"

CanonEOS::CanonEOS(uint8_t addr, uint8_t epin, uint8_t epout, uint8_t epint, uint8_t nconf, PTPMAIN pfunc)
: PTP(addr, epin, epout, epint, nconf, pfunc)
{
}

uint16_t CanonEOS::Initialize(bool binit)
{
	uint16_t	result1, result2;
	uint32_t	params[1];

	params[0] = (binit) ? 1 : 0;

	if ((result1 = Operation((binit) ? PTP_OC_EOS_SetPCConnectMode : PTP_OC_EOS_SetExtendedEventInfo, 1, params)) != PTP_RC_OK)
		Message(PSTR("PC Connect mode failed: "), result1);

	delay(50);

	if ((result1 = Operation((binit) ? PTP_OC_EOS_SetExtendedEventInfo : PTP_OC_EOS_SetPCConnectMode, 1, params)) != PTP_RC_OK)
		Message(PSTR("Ext. event info mode failed: "), result2);

	delay(50);


	return (((result1 == PTP_RC_OK) && (result2 == PTP_RC_OK)) ? PTP_RC_OK : PTP_RC_GeneralError);
}

uint16_t CanonEOS::StartBulb()
{
	uint32_t	params[3];

	params[0] = 0xfffffff8;
	params[1] = 0x00001000;
	params[2] = 0x00000000;

	Operation(0x911A, 3, params);
	Operation(0x911B, 0, NULL);
	Operation(0x9125, 0, NULL);

	return PTP_RC_OK;
}

uint16_t CanonEOS::StopBulb()
{
	uint32_t	params[3];

    params[0] = 0xffffffff;
	params[1] = 0x00001000;
	params[2] = 0x00000000;
	Operation(0x911A, 3, params);
    
    params[0] = 0xfffffffc;
	Operation(0x911A, 3, params);
    
	Operation(0x9126, 0, NULL);
    delay(50);
	Operation(0x911C, 0, NULL);
    delay(50);
}

uint16_t CanonEOS::EventCheck(PTPReadParser *parser)
{
	uint16_t	ptp_error	= PTP_RC_GeneralError;
	OperFlags	flags		= { 0, 0, 0, 1, 1, 0 };

	if ( (ptp_error = Transaction(0x9116, &flags, NULL, parser)) != PTP_RC_OK)
		Message(PSTR("EOSEventCheck: Error."), ptp_error);

	return ptp_error;
}

uint16_t CanonEOS::Capture()
{
	return Operation(PTP_OC_EOS_Capture, 0, NULL);
}

uint16_t CanonEOS::Test()
{
	uint16_t	ptp_error	= PTP_RC_GeneralError;

	if ((ptp_error = Operation(0x9802, 0, NULL)) != PTP_RC_OK)
		Message(PSTR("Test: Error: "), ptp_error);

	return ptp_error;
}

uint16_t CanonEOS::SetProperty(uint16_t prop, uint32_t val)
{
	uint16_t	ptp_error	= PTP_RC_GeneralError;
	OperFlags	flags		= { 0, 0, 1, 1, 3, 12 };
	uint32_t	params[3];

	params[0] = 0x0000000C;
	params[1] = (uint32_t)prop;
	params[2] = val;

	if ( (ptp_error = Transaction(PTP_OC_EOS_SetDevicePropValue, &flags, NULL, (void*)params)) != PTP_RC_OK)
		Message(PSTR("SetProperty: Error."), ptp_error);

	return ptp_error;
}


//CanonEOS::SetAperture(uint8_t val)
//{
//	uint16_t	ptp_error	= PTP_RC_GeneralError;
//	OperFlags	flags		= { 0, 0, 1, 1, 3, 12 };
//	uint32_t	params[3];
//
//	params[0] = 0x0000000C;
//	params[1] = 0x0000D101;
//	params[2] = (uint32_t)val;
//
//	if ( (ptp_error = Transaction(PTP_OC_EOS_SetDevicePropValue, &flags, NULL, (void*)params)) != PTP_RC_OK)
//		Message(PSTR("EOSSet: Error."), ptp_error);
//
//	return ptp_error;
//}
//
//CanonEOS::SetExposure(uint8_t val)
//{
//	uint16_t	ptp_error	= PTP_RC_GeneralError;
//	OperFlags	flags		= { 0, 0, 1, 1, 3, 12 };
//	uint32_t	params[3];
//
//	params[0] = 0x0000000C;
//	params[1] = 0x0000D102;
//	params[2] = (uint32_t)val;
//
//	if ( (ptp_error = Transaction(PTP_OC_EOS_SetDevicePropValue, &flags, NULL, (void*)params)) != PTP_RC_OK)
//		Message(PSTR("EOSSet: Error."), ptp_error);
//
//	return ptp_error;
//}
