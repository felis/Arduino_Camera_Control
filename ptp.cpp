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
#include "ptpconst.h"
#include "ptp.h"
#include "ptpdebug.h"

void PTP::SetInitialState()
{
	busyTime = 0;
	hangTime = 0;
	idSession = 0;
	idTransaction = 0;
	SetState(PTP_STATE_SESSION_NOT_OPENED);
}

void PTPStateHandlers::OnDeviceDisconnectedState(PTP *ptp)
{
}

void PTPStateHandlers::OnSessionNotOpenedState(PTP *ptp)
{
	if (ptp->OpenSession() == PTP_RC_OK)
	{
		PTPTRACE("Session opened\r\n");
		ptp->SetState(PTP_STATE_SESSION_OPENED);
	}
}

void PTPStateHandlers::OnSessionOpenedState(PTP *ptp)
{
	ptp->SetState(PTP_STATE_DEVICE_INITIALIZED);
}

void PTPStateHandlers::OnDeviceInitializedState(PTP *ptp)
{
}

void PTPStateHandlers::OnDeviceNotRespondingState(PTP *ptp)
{
}

void PTPStateHandlers::OnDeviceBusyState(PTP *ptp)
{
}

void uint16_to_char(uint16_t integer, unsigned char *data)
{
	data[0] = (integer & 0xff);
	data[1] = ((integer >> 8) & 0xff);
}

void uint32_to_char(uint32_t integer, unsigned char *data)
{
	data[0] = (integer & 0xff);
	data[1] = ((integer >> 8) & 0xff);
	data[2] = ((integer >> 16) & 0xff);
	data[3] = ((integer >> 24) & 0xff);
}

void char_to_uint16(unsigned char *scan)
{
	char c	= scan[0];
	scan[0] = scan[1];
	scan[1] = c;
}
  
void char_to_uint32(unsigned char *scan)
{
	char c	= scan[0];
	scan[0] = scan[3];
	scan[3] = c;
	c		= scan[1];
	scan[1] = scan[2];
	scan[2] = c;
}

void UsbStringToASCII(uint8_t *str)
{
    uint8_t i, j, len = (uint8_t)str[0];     
    
    // length and descriptor type one byte each should be skipped
    for (i=0, j=2; j<len; i++, j+=2)
        str[i] = str[j]; 
    str[i] = 0;
}

void Notify(const char* msg)
{
	if(!msg) return;
	char c;

	while((c = pgm_read_byte(msg++)))
		Serial.print(c,BYTE);
}

void Message(const char* msg, uint16_t rcode = 0)
{
	Notify(msg);
	Notify(PSTR(": "));
	PrintHex<uint16_t>(rcode);
	Notify(PSTR("\r\n"));
}

//void Message(const char* msg, uint16_t rcode = 0)
//{
//	Notify(msg);
//	msg = PSTR(": 0x");
//	Notify(msg);
//
//    uint16_t    mask = 0x1000;
//    
//    while (mask > 1)
//    {
//		if (rcode < mask)
//			Serial.print("0");
//		mask >>= 4;
//    }
//	Serial.println( rcode, HEX );
//}

PTP::PTP(uint8_t addr, uint8_t epin, uint8_t epout, uint8_t epint, uint8_t nconf, PTPStateHandlers *s) : 
	theState(0),
	busyTime(0),
	hangTime(0),
	idTransaction(0), 
	idSession(0), 
	devAddress(addr),
	epDataIn(epin),
	epDataOut(epout),
	epInterrupt(epint),
	numConf(nconf),
	stateMachine(s)
{
};

void PTP::Task2()
{
	switch (theState)
	{
	//case PTP_STATE_DEVICE_DISCONNECTED:
//		idSession = 0;
//		idTransaction = 0;
//		if (stateMachine)
	//		stateMachine->OnDeviceDisconnectedState(this);
	//	break;
	case PTP_STATE_SESSION_NOT_OPENED:
		if (stateMachine)
			stateMachine->OnSessionNotOpenedState(this);
		break;
	case PTP_STATE_SESSION_OPENED:
		if (stateMachine)
			stateMachine->OnSessionOpenedState(this);
		break;
	case PTP_STATE_DEVICE_INITIALIZED:
		if (stateMachine)
			stateMachine->OnDeviceInitializedState(this);
		break;
	case PTP_STATE_DEVICE_NOT_RESPONDING:
		if (stateMachine)
			stateMachine->OnDeviceNotRespondingState(this);
		break;
	case PTP_STATE_DEVICE_BUSY:
		if (stateMachine)
			stateMachine->OnDeviceBusyState(this);
		break;
	// Error state
	default:
		;
	}
}

uint16_t PTP::Transaction(uint16_t opcode, OperFlags *flags, uint32_t *params = NULL, void *pVoid = NULL)
{
	uint8_t		rcode;
	{
		uint8_t		cmd[PTP_USB_BULK_HDR_LEN + 12];		// header + 3 uint32_t parameters

		ZerroMemory(PTP_USB_BULK_HDR_LEN + 12, cmd);

		// Make command PTP container header
		uint16_to_char(PTP_USB_CONTAINER_COMMAND,	(unsigned char*)(cmd + PTP_CONTAINER_CONTYPE_OFF));			// type
		uint16_to_char(opcode,						(unsigned char*)(cmd + PTP_CONTAINER_OPCODE_OFF));			// code
		uint32_to_char(idTransaction++,				(unsigned char*)(cmd + PTP_CONTAINER_TRANSID_OFF));			// transaction id
		
		uint8_t		n = flags->opParams, len;

		if (params && *params)
		{
			*((uint8_t*)cmd) = len = PTP_USB_BULK_HDR_LEN + (n << 2);

			for (uint32_t *p1 = (uint32_t*)(cmd + PTP_CONTAINER_PAYLOAD_OFF), *p2 = (uint32_t*)params; n--; p1++, p2++)
				uint32_to_char(*p2, (unsigned char*)p1);
		}
		else
			*((uint8_t*)cmd) = len = PTP_USB_BULK_HDR_LEN;
		
		rcode = Usb.outTransfer(devAddress, epDataOut, len, (char*)cmd);

		if (rcode)
		{
			PTPTRACE2("Transaction: Command block send error", rcode);
			return PTP_RC_GeneralError;
		}
	}
	{
		uint8_t		data[PTP_MAX_RX_BUFFER_LEN];

		if (flags->txOperation)
		{
			if (flags->typeOfVoid && !pVoid)
			{
				PTPTRACE("Transaction: pVoid is NULL\n");
				return PTP_RC_GeneralError;
			}
			ZerroMemory(PTP_MAX_RX_BUFFER_LEN, data);

			uint32_t	bytes_left =	(flags->typeOfVoid == 3) ? PTP_USB_BULK_HDR_LEN + flags->dataSize :
							((flags->typeOfVoid == 1) ? PTP_USB_BULK_HDR_LEN + ((PTPDataSupplier*)pVoid)->GetDataSize() : 12);

			// Make data PTP container header
			*((uint32_t*)data) = bytes_left;
			uint16_to_char(PTP_USB_CONTAINER_DATA,	(unsigned char*)(data + PTP_CONTAINER_CONTYPE_OFF));		// type
			uint16_to_char(opcode,					(unsigned char*)(data + PTP_CONTAINER_OPCODE_OFF));			// code
			uint32_to_char(idTransaction,			(unsigned char*)(data + PTP_CONTAINER_TRANSID_OFF));		// transaction id

			uint16_t	len;

			if (flags->typeOfVoid == 1)
				len = (bytes_left < PTP_MAX_RX_BUFFER_LEN) ? bytes_left : PTP_MAX_RX_BUFFER_LEN;
			
			if (flags->typeOfVoid == 3)
			{
				uint8_t		*p1 = (data + PTP_USB_BULK_HDR_LEN);
				uint8_t		*p2 = (uint8_t*)pVoid;

				for (uint8_t i=flags->dataSize; i; i--, p1++, p2++)
					*p1 = *p2;

				len = PTP_USB_BULK_HDR_LEN + flags->dataSize;
			}
			bool first_time = true;

			while (bytes_left)
			{
				if (flags->typeOfVoid == 1)
					((PTPDataSupplier*)pVoid)->GetData(	(first_time) ? len - PTP_USB_BULK_HDR_LEN : len, 
														(first_time) ? (data + PTP_USB_BULK_HDR_LEN) : data);
				
				rcode = Usb.outTransfer(devAddress, epDataOut, len, (char*)data);

				if (rcode)
				{
					PTPTRACE2("Transaction: Data block send error.", rcode);
					return PTP_RC_GeneralError;
				}

				bytes_left -= len;

				len = (bytes_left < PTP_MAX_RX_BUFFER_LEN) ? bytes_left : PTP_MAX_RX_BUFFER_LEN;

				first_time = false;
			}
		}

		// Because inTransfer does not return the actual number of bytes recieved, it should be 
		// calculated here.
		uint32_t	total = 0, data_off = 0; 	// Total PTP data packet size, Data offset
		uint8_t		inbuffer = 0;			// Number of bytes read into buffer
		uint16_t	loops = 0;			// Number of loops necessary to get all the data from device
		uint8_t		timeoutcnt = 0;

		while (1)
		{
			ZerroMemory(PTP_MAX_RX_BUFFER_LEN, data);

			rcode = Usb.inTransfer(devAddress, epDataIn, PTP_MAX_RX_BUFFER_LEN, (char*)data);

			if (rcode)
			{
				PTPTRACE("Fatal USB Error\r\n");

				// in some cases NAK handling might be necessary
				PTPTRACE2("Transaction: Response recieve error", rcode);
				return PTP_RC_GeneralError;
			}

			// This can occure in case of unsupported operation or successive response after data reception stage
			if ((!loops || total == data_off) && *((uint16_t*)(data + PTP_CONTAINER_CONTYPE_OFF)) == PTP_USB_CONTAINER_RESPONSE)
			{
				uint16_t	response = *((uint16_t*)(data + PTP_CONTAINER_OPCODE_OFF));

				if (response == PTP_RC_OK && *((uint32_t*)data) > PTP_USB_BULK_HDR_LEN)
				{
					// number of params = (container length - 12) / 4
					uint8_t	n = (*((uint32_t*)data) - PTP_USB_BULK_HDR_LEN) >> 2;

					// BUG: n should be checked!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
					flags->rsParams = n;
					
					for (uint32_t *p1 = (uint32_t*)(data + PTP_USB_BULK_HDR_LEN), *p2 = (uint32_t*)params; n; n--, p1++, p2++)
						p2 = p1;
				}
				if (response != PTP_RC_OK)
				{
					PTPTRACE2("Transaction: Response recieve error", response);
					data_off = 0;
				}
				return response;
			}

			if (loops == 0)
			{
				total		=	*((uint32_t*)data);
				inbuffer	=	(total < PTP_MAX_RX_BUFFER_LEN) ? (uint8_t)total : PTP_MAX_RX_BUFFER_LEN;
			}
			else
				inbuffer = ((total - data_off) > PTP_MAX_RX_BUFFER_LEN) ? PTP_MAX_RX_BUFFER_LEN : (uint8_t)(total - data_off);

			if (pVoid)
			{
				if (flags->typeOfVoid == 0x01)
					((PTPReadParser*)pVoid)->Parse(inbuffer, data, (const uint32_t&)data_off);

				if (flags->typeOfVoid == 0x03)
					for (uint32_t i=0, j=data_off; i<inbuffer && j<flags->dataSize; i++, j++)
						((uint8_t*)pVoid)[j] = data[i];
			}
			data_off += inbuffer;

			loops ++;
			//delay(10);
		} // while(1)
	} // end of scope
}


uint16_t PTP::EventCheck(PTPReadParser *pParser)
{
	uint8_t		data[PTP_MAX_EV_BUFFER_LEN];
	uint8_t		rcode;

	// Because inTransfer does not return the actual number of bytes recieved, it should be 
	// calculated here.
	uint32_t	total = 0, data_off = 0;
	uint8_t		inbuffer = 0;
	uint16_t	loops = 0;
	uint8_t		timeoutcnt = 0;

	while (1)
	{
		ZerroMemory(PTP_MAX_EV_BUFFER_LEN, data);

		rcode = Usb.inTransfer(devAddress, epInterrupt, PTP_MAX_EV_BUFFER_LEN, (char*)data);

		switch (rcode)
		{
		// In case of no event occured
		case 0xFF:
				return PTP_EC_Undefined;

		default:
			// in case of a usb error
			PTPTRACE2("EventCheck USB error: ", rcode);
			return PTP_RC_GeneralError;
		}

		if (loops == 0)
		{
			total		=	*((uint32_t*)data);
			inbuffer	=	(total < PTP_MAX_EV_BUFFER_LEN) ? (uint8_t)total : PTP_MAX_EV_BUFFER_LEN;
		}
		else
			inbuffer = ((total - data_off) > PTP_MAX_EV_BUFFER_LEN) ? PTP_MAX_EV_BUFFER_LEN : (uint8_t)(total - data_off);

		if (pParser)
			pParser->Parse(inbuffer, data, (const uint32_t&)data_off);
		
		data_off += inbuffer;

		loops ++;
		delay(50);
	} // while(1)
}

bool PTP::CheckEvent(uint8_t size, uint8_t *buf)
{
	ZerroMemory(size, buf);

	uint8_t rcode = Usb.inTransfer(devAddress, epInterrupt, size, (char*)buf);

	// if no interrupts pending - return false

	// there should be some error type checking involved i.e. timeout error
	if (rcode)
		return false;

	return true;
}

bool PTP::EventWait(uint8_t size, uint8_t *event_buf, uint16_t timeout)
{
	const uint16_t	milisec = 0xA0;		// must be polling interval for the interrupt pipe
	bool			occured	= false;

	while (!(occured = CheckEvent(size, event_buf)) && (timeout -= milisec))
		delay(milisec);

	return occured;
}

uint16_t PTP::OpenSession()
{
	uint32_t	params[1];
	OperFlags	flags = { 1, 0, 0, 0, 0, 0 };

	idSession		= 1;
	idTransaction	= 1;

	params[0]	= idSession;

	return Transaction(PTP_OC_OpenSession, &flags, params);
}

uint16_t PTP::ResetDevice()
{
	OperFlags	flags = { 0, 0, 0, 0, 0, 0 };
	return Transaction(PTP_OC_ResetDevice, &flags);
}

uint16_t PTP::GetNumObjects(uint32_t &retval, uint32_t storage_id, uint16_t format, uint32_t assoc)
{
	uint16_t	ptp_error = PTP_RC_GeneralError;
	OperFlags	flags = { 3, 1, 0, 0, 0, 0 };
	uint32_t	params[3];

	if ( (ptp_error = Transaction(PTP_OC_GetNumObjects, &flags, params)) == PTP_RC_OK)
		retval = params[0];

	return ptp_error;
}

uint16_t PTP::GetObject(uint32_t handle, PTPReadParser *parser)
{
	OperFlags	flags = { 1, 0, 0, 1, 1, 0 };
	uint32_t	params[1];

	params[0] = handle;

	return Transaction(PTP_OC_GetObject, &flags, params, parser);
}

uint16_t PTP::GetThumb(uint32_t handle, PTPReadParser *parser)
{
	OperFlags	flags = { 1, 0, 0, 1, 1, 0 };
	uint32_t	params[1];

	params[0] = handle;

	return Transaction(PTP_OC_GetThumb, &flags, params, parser);
}

uint16_t PTP::DeleteObject(uint32_t handle, uint16_t format)
{
	OperFlags	flags = { 2, 0, 0, 0, 0, 0 };
	uint32_t	params[2];

	params[0] = handle;
	params[1] = (uint32_t)format;

	return Transaction(PTP_OC_DeleteObject, &flags, params);
}

uint16_t PTP::SetObjectProtection(uint32_t handle, uint16_t attrib)
{
	OperFlags	flags = { 2, 0, 0, 0, 0, 0 };
	uint32_t	params[2];

	params[0] = handle;
	params[1] = (uint32_t)attrib;

	return Transaction(PTP_OC_SetObjectProtection, &flags, params);
}

uint16_t PTP::MoveObject(uint32_t handle, uint32_t storage_id, uint32_t parent)
{
	OperFlags	flags = { 3, 0, 0, 0, 0, 0 };
	uint32_t	params[2];

	params[0] = handle;
	params[1] = storage_id;
	params[2] = parent;

	return Transaction(PTP_OC_MoveObject, &flags, params);
}

uint16_t PTP::CopyObject(uint32_t handle, uint32_t storage_id, uint32_t parent, uint32_t &new_handle)
{
	uint16_t	ptp_error = PTP_RC_GeneralError;
	OperFlags	flags = { 3, 1, 0, 0, 0, 0 };
	uint32_t	params[3];

	params[0] = handle;
	params[1] = storage_id;
	params[2] = parent;

	if ( (ptp_error = Transaction(PTP_OC_CopyObject, &flags, params)) == PTP_RC_OK)
		new_handle = params[0];

	return ptp_error;
}

uint16_t PTP::InitiateCapture(uint32_t storage_id, uint16_t format)
{
	uint16_t	ptp_error = PTP_RC_GeneralError;
	OperFlags	flags = { 2, 0, 0, 0, 0, 0 };
	uint32_t	params[2];

	params[0] = storage_id;
	params[1] = (uint32_t)format;

	if ( (ptp_error = Transaction(PTP_OC_InitiateCapture, &flags, params)) == PTP_RC_OK)
	{}

	return ptp_error;
}

uint16_t PTP::InitiateOpenCapture(uint32_t storage_id, uint16_t format)
{
	uint16_t	ptp_error = PTP_RC_GeneralError;
	OperFlags	flags = { 2, 0, 0, 0, 0, 0 };
	uint32_t	params[2];

	params[0] = storage_id;
	params[1] = (uint32_t)format;

	if ( (ptp_error = Transaction(PTP_OC_InitiateOpenCapture, &flags, params)) == PTP_RC_OK)
	{}

	return ptp_error;
}
	
uint16_t PTP::TerminateOpenCapture(uint32_t trans_id)
{
	OperFlags	flags = { 1, 0, 0, 0, 0, 0 };
	uint32_t	params[1];

	params[0] = trans_id;

	return Transaction(PTP_OC_TerminateOpenCapture, &flags, params);
}

uint16_t PTP::PowerDown()
{
	OperFlags	flags = { 0, 0, 0, 0, 0, 0 };
	return Transaction(PTP_OC_PowerDown, &flags);
}

uint16_t PTP::SelfTest(uint16_t type = 0)
{
	OperFlags	flags = { 1, 0, 0, 0 };
	uint32_t	params[1];
	params[0]	= type;

	return Transaction(PTP_OC_SelfTest, &flags, params);
}

uint16_t PTP::CloseSession()
{
	uint16_t	ptp_error = PTP_RC_GeneralError;
	OperFlags	flags = { 0, 0, 0, 0, 0, 0 };

	if ( (ptp_error = Transaction(PTP_OC_CloseSession, &flags)) == PTP_RC_OK)
		idSession = idTransaction = 0;
	
	return ptp_error;
}

uint16_t PTP::GetDeviceInfo(PTPReadParser *parser)
{
	OperFlags	flags = { 0, 0, 0, 1, 1, 0 };
	return Transaction(PTP_OC_GetDeviceInfo, &flags, NULL, parser);
}

uint16_t PTP::GetObjectInfo(uint32_t handle, PTPReadParser *parser)
{
	OperFlags	flags = { 1, 0, 0, 1, 1, 0 };
	uint32_t	params[1];
	params[0] = handle;

	return Transaction(PTP_OC_GetObjectInfo, &flags, params, parser);
}

uint16_t PTP::GetDevicePropDesc(const uint16_t pcode, PTPReadParser *parser)
{
	OperFlags	flags		= { 1, 0, 0, 1, 1, 0 };
	uint32_t	params[1];

	params[0] = (uint32_t)pcode;

	return Transaction(PTP_OC_GetDevicePropDesc, &flags, params, parser);
}

uint16_t PTP::GetDevicePropValue(const uint16_t pcode, PTPReadParser *parser)
{
	OperFlags	flags		= { 1, 0, 0, 1, 1, 0 };
	uint32_t	params[1];

	params[0] = (uint32_t)pcode;

	return Transaction(PTP_OC_GetDevicePropValue, &flags, params, parser);
}

uint16_t PTP::GetDevicePropValue(const uint16_t pcode, uint8_t &val)
{
	uint16_t	ptp_error	= PTP_RC_GeneralError;
	OperFlags	flags		= { 1, 0, 0, 0, 3, 13 };
	uint32_t	params[1];
	uint8_t		buf[13];

	params[0] = (uint32_t)pcode;

	if ( (ptp_error = Transaction(PTP_OC_GetDevicePropValue, &flags, params, buf)) == PTP_RC_OK)
		val = buf[12];

	return ptp_error;
}

uint16_t PTP::GetDevicePropValue(const uint16_t pcode, uint16_t &val)
{
	uint16_t	ptp_error	= PTP_RC_GeneralError;
	OperFlags	flags		= { 1, 0, 0, 0, 3, 14 };
	uint32_t	params[1];
	uint16_t	buf[7];

	params[0] = pcode;

	if ( (ptp_error = Transaction(PTP_OC_GetDevicePropValue, &flags, params, buf)) == PTP_RC_OK)
		val = buf[6];

	return ptp_error;
}

uint16_t PTP::GetDevicePropValue(const uint16_t pcode, uint32_t &val)
{
	uint16_t	ptp_error	= PTP_RC_GeneralError;
	OperFlags	flags		= { 1, 0, 0, 0, 3, 16 };
	uint32_t	params[1];
	uint32_t	buf[4];

	params[0] = pcode;

	if ( (ptp_error = Transaction(PTP_OC_GetDevicePropValue, &flags, params, buf)) == PTP_RC_OK)
		val = buf[3];

	return ptp_error;
}

uint16_t PTP::SetDevicePropValue(uint16_t pcode, uint8_t val)
{
	OperFlags	flags		= { 1, 0, 1, 1, 3, 1 };
	uint32_t	params[1];
	uint8_t		value;

	params[0]	= (uint32_t)pcode;
	value		= val;

	return Transaction(PTP_OC_SetDevicePropValue, &flags, params, (void*)&value);
}

uint16_t PTP::SetDevicePropValue(uint16_t pcode, uint16_t val)
{
	OperFlags	flags		= { 1, 0, 1, 1, 3, 2 };
	uint32_t	params[1];
	uint16_t	value;

	params[0]	= (uint32_t)pcode;
	value		= val;

	return Transaction(PTP_OC_SetDevicePropValue, &flags, params, (void*)&value);
}

uint16_t PTP::SetDevicePropValue(uint16_t pcode, uint32_t val)
{
	OperFlags	flags		= { 1, 0, 1, 1, 3, 4 };
	uint32_t	params[1];
	uint32_t	value;

	params[0]	= (uint32_t)pcode;
	value		= val;

	return Transaction(PTP_OC_SetDevicePropValue, &flags, params, (void*)&value);
}

uint16_t PTP::ResetDevicePropValue(const uint16_t pcode)
{
	OperFlags	flags		= { 1, 0, 0, 0 };
	uint32_t	params[1];

	params[0] = (uint32_t)pcode;

	return Transaction(PTP_OC_ResetDevicePropValue, &flags, params);
}

uint16_t PTP::Operation(uint16_t opcode, uint8_t nparams, uint32_t *params)
{
	OperFlags	flags		= { 0, 0, 0, 0, 0, 0 };

	flags.opParams = nparams;

	return Transaction(opcode, &flags, params);
}

uint16_t PTP::GetStorageInfo(uint32_t storage_id, PTPReadParser *parser)
{
	OperFlags	flags		= { 1, 0, 0, 1, 1, 0 };

	uint32_t	params[1]; 
	params[0]	= storage_id;

	return Transaction(PTP_OC_GetStorageInfo, &flags, params, parser);
}

uint16_t PTP::FormatStore(uint32_t storage_id, uint32_t fsformat)
{
	OperFlags	flags		= { 2, 0, 0, 0, 0, 0 };

	uint32_t	params[2]; 
	params[0]	= storage_id;
	params[1]	= fsformat;

	return Transaction(PTP_OC_FormatStore, &flags, params);
}

uint16_t PTP::CaptureImage()
{
	uint16_t	ptp_error = PTP_RC_GeneralError;
	uint32_t	params[2] = {0, 0x00003801};
	OperFlags	flags = { 2, 0, 0, 0 };

	if ( (ptp_error = Transaction(PTP_OC_InitiateCapture, &flags, params)) != PTP_RC_OK)
	{
		PTPTRACE2("CaptureImage error", ptp_error);
		return ptp_error;
	}
	PTPUSBEventContainer	evnt;
	bool					occured;

	// multiple objects can be added depending on current camera shooting mode
	while ((occured = EventWait(sizeof(PTPUSBEventContainer), (uint8_t*)&evnt, 500)) && evnt.code == PTP_EC_ObjectAdded)
		PTPTRACE("CaptureImage: New object added.\r\n");
	
	if (!occured)
	{
		PTPTRACE("CaptureImage: Timeout ellapsed.\r\n");
		return PTP_RC_Undefined;
	}
	switch (evnt.code)
	{
	case PTP_EC_CaptureComplete:
		PTPTRACE("CaptureImage: Image captured.\r\n");
		return PTP_RC_OK;

	case PTP_EC_StoreFull:
		PTPTRACE("CaptureImage: Storage is full.\r\n");
		return PTP_RC_StoreFull;

	default:
		PTPTRACE2("CaptureImage: Unexpected event\r\n", evnt.code);
		return PTP_RC_Undefined;
	}
}

uint16_t PTP::GetStorageIDs(PTPReadParser *parser)
{
	OperFlags	flags = { 0, 0, 0, 1, 1, 0 };
	return Transaction(PTP_OC_GetStorageIDs, &flags, NULL, parser);
}

uint16_t PTP::GetStorageIDs(uint8_t bufsize, uint8_t *pbuf)
{
	OperFlags	flags = { 0, 0, 0, 1, 3, 0 };

	flags.dataSize = bufsize;

	return Transaction(PTP_OC_GetStorageIDs, &flags, NULL, pbuf);
}

uint16_t PTP::GetObjectHandles(uint32_t storage_id, uint16_t format, uint16_t assoc, PTPReadParser *parser)
{
	OperFlags	flags = { 3, 0, 0, 1, 1, 0 };
	uint32_t	params[3];

	params[0] = storage_id;
	params[1] = (uint32_t)format;
	params[2] = (uint32_t)assoc;

	return Transaction(PTP_OC_GetObjectHandles, &flags, params, parser);
}

void PTP::Task()
{
    Max.Task();
    Usb.Task();

    if( Usb.getUsbTaskState() == USB_DETACHED_SUBSTATE_WAIT_FOR_DEVICE ) 
	{
		idSession = 0;
		idTransaction = 0;

		stateMachine->OnDeviceDisconnectedState(this);
	}
	//wait for addressing state
    if( Usb.getUsbTaskState() == USB_STATE_CONFIGURING ) 
	{  
		/* Initialize data structures */
	    epRecord[ 0 ] = *( Usb.getDevTableEntry( 0,0 ));  //copy endpoint 0 parameters
	    
	    // Data-In EP
	    epRecord[ 1 ].epAddr     = epDataIn;
	    epRecord[ 1 ].Attr       = 0x02;
	    epRecord[ 1 ].MaxPktSize = 0x0040;
	    epRecord[ 1 ].Interval   = 0;
	    epRecord[ 1 ].sndToggle  = bmSNDTOG0;
	    epRecord[ 1 ].rcvToggle  = bmRCVTOG0;
	    
	    // Data-Out EP
	    epRecord[ 2 ].epAddr     = epDataOut;
	    epRecord[ 2 ].Attr       = 0x02;
	    epRecord[ 2 ].MaxPktSize = 0x0040;
	    epRecord[ 2 ].Interval   = 0;
	    epRecord[ 2 ].sndToggle  = bmSNDTOG0;
	    epRecord[ 2 ].rcvToggle  = bmRCVTOG0;
	    
	    // Interrupt EP
	    epRecord[ 3 ].epAddr     = epInterrupt;
	    epRecord[ 3 ].Attr       = 0x03;
	    epRecord[ 3 ].MaxPktSize = 0x0008;
	    epRecord[ 3 ].Interval   = 0x0A;
	    epRecord[ 3 ].sndToggle  = bmSNDTOG0;
	    epRecord[ 3 ].rcvToggle  = bmRCVTOG0;
	
	    Usb.setDevTableEntry( devAddress, epRecord );

		uint8_t	rcode;

	    /* Configure device */
	    if ((rcode = Usb.setConf( devAddress, 0, numConf )))
	        HaltOnError(MsgErrDeviceConf, rcode);

        Usb.setUsbTaskState( USB_STATE_RUNNING );

		SetInitialState();
	}
	if( Usb.getUsbTaskState() == USB_STATE_RUNNING ) 
	{  
		Task2();
    }
}

uint8_t PTP::GetConfDescr( byte addr, byte conf )
{
	char buf[ PTP_MAX_RX_BUFFER_LEN ];
	byte rcode;
	unsigned int total_length;
	rcode = Usb.getConfDescr( addr, 0, 4, conf, buf );  //get total length
	//LOBYTE( total_length ) = buf[ 2 ];
	//HIBYTE( total_length ) = buf[ 3 ];

	total_length = *((uint16_t*)(buf + 2));

	if( total_length > PTP_MAX_RX_BUFFER_LEN ) 
	{    //check if total length is larger than buffer
		//Serial.println("Total length truncated to 256 bytes");
		total_length = PTP_MAX_RX_BUFFER_LEN;
	}
	rcode = Usb.getConfDescr( addr, 0, total_length, conf, buf ); //get the whole descriptor
	return( 0 );
}
