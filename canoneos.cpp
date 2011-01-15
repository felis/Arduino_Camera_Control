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
#include "canoneos.h"


void EOSStateHandlers::OnSessionOpenedState(PTP *ptp)
{
	if (!FAILED(((CanonEOS*)ptp)->SetPCConnectMode(1)) && !FAILED(((CanonEOS*)ptp)->SetExtendedEventInfo(1)))
		ptp->SetState(PTP_STATE_DEVICE_INITIALIZED);
}

uint32_t ImgQualitySupplier::GetDataSize()
{
	return ((pictFormat & 0xFFFF0000) ? 0x0000002C : 0x0000001C);
}

void ImgQualitySupplier::GetData(const uint16_t len, uint8_t *pbuf)
{
	uint8_t		num_files = (pictFormat & 0xFFFF0000) ? 2 : 1;

	((uint32_t*)pbuf)[0] =  (num_files == 2) ? 0x0000002C : 0x0000001C;
	((uint32_t*)pbuf)[1] =	(uint32_t) EOS_DPC_ImageQuality;
	((uint32_t*)pbuf)[2] =	(uint32_t) num_files;

	uint32_t	format = pictFormat;
	
	for (uint8_t i=0, pos=3; i<num_files; i++)
	{
		((uint32_t*)pbuf)[pos++] = 0x00000010;

		for (uint8_t j=0; j<3; j++, format >>= 4)
			((uint32_t*)pbuf)[pos++] = (uint32_t)(format & 0xF);
	}
}

CanonEOS::CanonEOS(uint8_t addr, uint8_t epin, uint8_t epout, uint8_t epint, uint8_t nconf, PTPStateHandlers *s)
: PTP(addr, epin, epout, epint, nconf, s)
{
}

uint16_t CanonEOS::SetImageQuality(uint32_t format)
{
	uint16_t	ptp_error	= PTP_RC_GeneralError;
	OperFlags	flags		= { 0, 0, 1, 1, 1, 0 };

	ImgQualitySupplier		sup;
	sup.SetPictureFormat(format);

	if ( (ptp_error = Transaction(EOS_OC_SetDevicePropValue, &flags, NULL, (void*)&sup)) != PTP_RC_OK)
		PTPTRACE2("SetImageQuality error", ptp_error);

	return ptp_error;
}

uint16_t CanonEOS::SetPCConnectMode(uint8_t mode)
{
	uint32_t	params[1];
	params[0] = (uint32_t) mode;
	return Operation(EOS_OC_SetPCConnectMode, 1, params);
}

uint16_t CanonEOS::SetExtendedEventInfo(uint8_t mode)
{
	uint32_t	params[1];
	params[0] = (uint32_t) mode;
	return Operation(EOS_OC_SetExtendedEventInfo, 1, params);
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
	Operation(0x911C, 0, NULL);

	return PTP_RC_OK;
}

uint16_t CanonEOS::CancelTransfer(uint32_t object_id)
{
	uint32_t	params[1];
	params[0] = object_id;

	return Operation(EOS_OC_CancelTransfer, 1, params);
}

uint16_t CanonEOS::ResetTransfer(uint32_t object_id)
{
	uint32_t	params[1];
	params[0] = object_id;

	return Operation(EOS_OC_ResetTransfer, 1, params);
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
				PTPTRACE2("LiveView start failure:", ptp_error);
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

	if ( (ptp_error = Transaction(EOS_OC_MoveFocus, &flags, params, NULL)) != PTP_RC_OK)
		PTPTRACE2("MoveFocus error: ", ptp_error);

	return ptp_error;
}

uint16_t CanonEOS::EventCheck(PTPReadParser *parser)
{
	uint16_t	ptp_error	= PTP_RC_GeneralError;
	OperFlags	flags		= { 0, 0, 0, 1, 1, 0 };

	if ( (ptp_error = Transaction(0x9116, &flags, NULL, parser)) != PTP_RC_OK)
		PTPTRACE2("EOSEventCheck error:", ptp_error);

	return ptp_error;
}

uint16_t CanonEOS::Capture()
{
	return Operation(EOS_OC_Capture, 0, NULL);
}


uint16_t CanonEOS::SetProperty(uint16_t prop, uint32_t val)
{
	uint16_t	ptp_error	= PTP_RC_GeneralError;
	OperFlags	flags		= { 0, 0, 1, 1, 3, 12 };
	uint32_t	params[3];

	params[0] = 0x0000000C;
	params[1] = (uint32_t)prop;
	params[2] = val;

	if ( (ptp_error = Transaction(EOS_OC_SetDevicePropValue, &flags, NULL, (void*)params)) != PTP_RC_OK)
		PTPTRACE2("SetProperty error:", ptp_error);

	return ptp_error;
}

uint16_t CanonEOS::GetProperty(uint16_t prop, PTPReadParser *parser)
{
	uint16_t	ptp_error	= PTP_RC_GeneralError;
	OperFlags	flags		= { 1, 0, 0, 1, 1, 0 };
	uint32_t	params[1];

	params[0] = (uint32_t)prop;

	if ( (ptp_error = Transaction(EOS_OC_GetDevicePropValue, &flags, params, (void*)parser)) != PTP_RC_OK)
		PTPTRACE2("GetProperty error:", ptp_error);

	return ptp_error;
}

uint16_t CanonEOS::GetDeviceInfoEx(PTPReadParser *parser)
{
	uint16_t	ptp_error	= PTP_RC_GeneralError;
	OperFlags	flags		= { 0, 0, 0, 1, 1, 0 };

	if ( (ptp_error = Transaction(EOS_OC_GetDeviceInfoEx, &flags, NULL, (void*)parser)) != PTP_RC_OK)
		PTPTRACE2("GetDeviceInfo error:", ptp_error);

	return ptp_error;
}

uint16_t CanonEOS::GetObject(uint32_t object_id, uint32_t parent_id, PTPReadParser *parser)
{
	uint16_t	ptp_error	= PTP_RC_GeneralError;
	OperFlags	flags		= { 2, 0, 0, 1, 1, 0 };
	uint32_t	params[2];

	params[0] = object_id;
	params[1] = parent_id;

	if ( (ptp_error = Transaction(EOS_OC_GetObject, &flags, params, (void*)parser)) != PTP_RC_OK)
		PTPTRACE2("GetObject error:", ptp_error);

	return ptp_error;
}
