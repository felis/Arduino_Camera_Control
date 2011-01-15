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
#include "canonps.h"

void PSStateHandlers::OnSessionOpenedState(PTP *ptp)
{
	if (FAILED(((CanonPS*)ptp)->SetDevicePropValue(PS_DPC_EventEmulateMode, (uint16_t)4)) )
		PTPTRACE("EventEmulateMode error\r\n");
	
	if (FAILED(((CanonPS*)ptp)->Initialize(true)) )
		PTPTRACE("Initialization error\r\n");
	
	ptp->SetState(PTP_STATE_DEVICE_INITIALIZED);
}


CanonPS::CanonPS(uint8_t addr, uint8_t epin, uint8_t epout, uint8_t epint, uint8_t nconf, PTPStateHandlers *s)
: PTP(addr, epin, epout, epint, nconf, s)
{
}

uint16_t CanonPS::EventCheck(PTPReadParser *parser)
{
	uint16_t	ptp_error	= PTP_RC_GeneralError;
	OperFlags	flags		= { 0, 0, 0, 1, 1, 0 };

	if ( (ptp_error = Transaction(PS_OC_CheckEvent, &flags, NULL, parser)) != PTP_RC_OK)
		PTPTRACE2("EOSEventCheck error: ", ptp_error);

	return ptp_error;
}

uint16_t CanonPS::Initialize(bool binit)
{
	uint16_t	ptp_error;

	if (binit)
	{
		if ((ptp_error = Operation(PS_OC_StartShootingMode, 0, NULL)) != PTP_RC_OK)
			PTPTRACE2("StartShootingMode failed: ", ptp_error);
	}
	else
	{
		if ((ptp_error = Operation(PS_OC_EndShootingMode, 0, NULL)) != PTP_RC_OK)
			PTPTRACE2("EndShootingMode failed: ", ptp_error);
	}
	return ptp_error;
}

uint16_t CanonPS::Capture()
{
	uint16_t	ptp_error;

	if ((ptp_error = Operation(PS_OC_FocusLock, 0, NULL)) != PTP_RC_OK)
		PTPTRACE2("Focus Lock Error: ", ptp_error);

	if ((ptp_error = Operation(PS_OC_InitiateCaptureInMemory, 0, NULL)) != PTP_RC_OK)
		PTPTRACE2("Capture Error: ", ptp_error);

	return ptp_error;
}

