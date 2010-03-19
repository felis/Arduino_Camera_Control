#include "canonps.h"

CanonPS::CanonPS(uint8_t addr, uint8_t epin, uint8_t epout, uint8_t epint, uint8_t nconf, PTPMAIN pfunc)
: PTP(addr, epin, epout, epint, nconf, pfunc)
{
}

uint16_t CanonPS::EventCheck(PTPReadParser *parser)
{
	uint16_t	ptp_error	= PTP_RC_GeneralError;
	OperFlags	flags		= { 0, 0, 0, 1, 1, 0 };

	if ( (ptp_error = Transaction(PTP_OC_PS_CheckEvent, &flags, NULL, parser)) != PTP_RC_OK)
		Message(PSTR("EOSEventCheck: Error."), ptp_error);

	return ptp_error;
}

uint16_t CanonPS::Initialize(bool binit)
{
	uint16_t	ptp_error;

	if (binit)
	{
		if ((ptp_error = Operation(PTP_OC_PS_StartShootingMode, 0, NULL)) != PTP_RC_OK)
			Message(PSTR("PC Connect mode failed: "), ptp_error);
	}
	else
	{
		if ((ptp_error = Operation(PTP_OC_PS_EndShootingMode, 0, NULL)) != PTP_RC_OK)
			Message(PSTR("PC Connect mode failed: "), ptp_error);
	}
	return ptp_error;
}

uint16_t CanonPS::Capture()
{
	uint16_t	ptp_error;

	if ((ptp_error = Operation(PTP_OC_PS_FocusLock, 0, NULL)) != PTP_RC_OK)
		Message(PSTR("Focus Lock: Error: "), ptp_error);

	if ((ptp_error = Operation(PTP_OC_PS_InitiateCaptureInMemory, 0, NULL)) != PTP_RC_OK)
		Message(PSTR("Capture: Error: "), ptp_error);

	return ptp_error;
}

#if 0
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

uint16_t CanonEOS::SwitchLiveView(bool on)
{
	uint16_t	ptp_error = PTP_RC_GeneralError;

	if ((ptp_error = SetProperty(EOS_DPC_LiveView, (on) ? 2 : 0)) == PTP_RC_OK)
	{
		if (on)
		{
			if ((ptp_error = SetProperty(0xD1B3, 0)) != PTP_RC_OK)
			{
				Message(PSTR("LiveView start failure:"), ptp_error);
				SetProperty(EOS_DPC_LiveView, 0);
				return PTP_RC_GeneralError;
			}
		}
	}
	return ptp_error;
}

uint16_t CanonEOS::MoveFocus(uint16_t step)
{
	uint16_t	ptp_error	= PTP_RC_GeneralError;
	OperFlags	flags		= { 1, 0, 0, 0, 0, 0 };
	uint32_t	params[1];

	params[0] = (uint32_t) step;

	if ( (ptp_error = Transaction(PTP_OC_EOS_MoveFocus, &flags, params, NULL)) != PTP_RC_OK)
		Message(PSTR("MoveFocus: Error."), ptp_error);
	else
		Message(PSTR("MoveFocus: Success."), ptp_error);

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
#endif 

