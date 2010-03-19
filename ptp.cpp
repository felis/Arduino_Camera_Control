#include "ptpconst.h"
#include "ptp.h"

void PTPObjHandleParser::Parse(const uint16_t len, const uint8_t *pbuf, const uint32_t &offset)
{
	uint16_t	cntdn	= (uint16_t)len;
	uint8_t		*p		= (uint8_t*)pbuf;

	arrayParser.Parse(&p, &cntdn, (PTP_ARRAY_EL_FUNC)&PrintHandle);
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
	msg = PSTR(": 0x");
	Notify(msg);

    uint16_t    mask = 0x1000;
    
    while (mask > 1)
    {
		if (rcode < mask)
			Serial.print("0");
		mask >>= 4;
    }
	Serial.println( rcode, HEX );
}


#ifdef PTP_HANDLE_RESPONSES
const char* PTP::stdResponse[] PROGMEM = {
	msgUndefined,
	msgOK,
	msgGeneralError,
	msgSessionNotOpen,
	msgInvalidTransactionID,
	msgOperationNotSupported,
	msgParameterNotSupported,
	msgIncompleteTransfer,
	msgInvalidStorageId,
	msgInvalidObjectHandle,
	msgDevicePropNotSupported,
	msgInvalidObjectFormatCode,
	msgStoreFull,
	msgObjectWriteProtected,
	msgStoreReadOnly,
	msgAccessDenied,
	msgNoThumbnailPresent,
	msgSelfTestFailed,
	msgPartialDeletion,
	msgStoreNotAvailable,
	msgSpecificationByFormatUnsupported,
	msgNoValidObjectInfo,
	msgInvalidCodeFormat,
	msgUnknownVendorCode,
	msgCaptureAlreadyTerminated,
	msgDeviceBusy,
	msgInvalidParentObject,
	msgInvalidDevicePropFormat,
	msgInvalidDevicePropValue,
	msgInvalidParameter,
	msgSessionAlreadyOpened,
	msgTransactionCanceled,
	msgSpecificationOfDestinationUnsupported
};	
#endif

#if 0
const char* PTP::dpValNames[] PROGMEM = {
	msgUndefined,
	msgBatteryLevel,				
	msgFunctionalMode,				
	msgImageSize,					
	msgCompressionSetting,			
	msgWhiteBalance,				
	msgRGBGain,						
	msgFNumber,						
	msgFocalLength,					
	msgFocusDistance,				
	msgFocusMode,					
	msgExposureMeteringMode,		
	msgFlashMode,					
	msgExposureTime,				
	msgExposureProgramMode,			
	msgExposureIndex,				
	msgExposureBiasCompensation,	
	msgDateTime,					
	msgCaptureDelay,				
	msgStillCaptureMode,			
	msgContrast,					
	msgSharpness,					
	msgDigitalZoom,					
	msgEffectMode,					
	msgBurstNumber,					
	msgBurstInterval,				
	msgTimelapseNumber,				
	msgTimelapseInterval,			
	msgFocusMeteringMode,			
	msgUploadURL,					
	msgArtist,						
	msgCopyrightInfo
};
#endif

PTP::PTP(uint8_t addr, uint8_t epin, uint8_t epout, uint8_t epint, uint8_t nconf, PTPMAIN pf) : 
	idTransaction(0), 
	idSession(0) 
	,devAddress(addr),
	epDataIn(epin),
	epDataOut(epout),
	epInterrupt(epint),
	numConf(nconf),
	pfRunning(pf)
{
};

#ifdef PTP_HANDLE_RESPONSES
void PTP::HandleResponse(uint16_t rc)
{
	if (((rc >> 8) & 0xFF) == 0x20)
		if ((rc & 0xFF) > (PTP_RC_SpecificationOfDestinationUnsupported & 0xFF))
			Message(PSTR("Undefined response: "), rc);
		else
			Message((char*)pgm_read_word(&stdResponse[(rc & 0xFF)]), rc);
	else
		Message(PSTR("Error: "), rc);
}
#endif

uint16_t PTP::Transaction(uint16_t opcode, OperFlags *flags, uint32_t *params = NULL, void *pVoid = NULL)
{
	uint8_t		rcode;
	{
		uint8_t		cmd[PTP_USB_BULK_HDR_LEN + 12];		// header + 3 uint32_t parameters

		ZerroMemory(PTP_USB_BULK_HDR_LEN + 12, cmd);

		// Make command PTP container header
		uint16_to_char(PTP_USB_CONTAINER_COMMAND,	(unsigned char*)(cmd + PTP_CONTAINER_CONTYPE_OFF));			// type
		uint16_to_char(opcode,						(unsigned char*)(cmd + PTP_CONTAINER_OPCODE_OFF));			// code
		uint32_to_char(++idTransaction,				(unsigned char*)(cmd + PTP_CONTAINER_TRANSID_OFF));			// transaction id
		
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
			Message(PSTR("Transaction: Command block send error."), rcode);
			return PTP_RC_GeneralError;
		}
		//Message(PSTR("Transaction: Command block successfully sent."), rcode);
		//delay(50);
	}
	{
		uint8_t		data[PTP_MAX_RX_BUFFER_LEN];

		if (flags->txOperation)
		{
			if (flags->typeOfVoid == 3 && pVoid)
			{
				ZerroMemory(PTP_MAX_RX_BUFFER_LEN, data);

				uint8_t		len = PTP_USB_BULK_HDR_LEN + flags->dataSize;

				// Make data PTP container header
				*((uint8_t*)data) = len;
				uint16_to_char(PTP_USB_CONTAINER_DATA,		(unsigned char*)(data + PTP_CONTAINER_CONTYPE_OFF));			// type
				uint16_to_char(opcode,						(unsigned char*)(data + PTP_CONTAINER_OPCODE_OFF));			// code
				uint32_to_char(idTransaction,				(unsigned char*)(data + PTP_CONTAINER_TRANSID_OFF));			// transaction id

				uint8_t		*p1 = (data + PTP_USB_BULK_HDR_LEN);
				uint8_t		*p2 = (uint8_t*)pVoid;

				for (uint8_t i=flags->dataSize; i; i--, p1++, p2++)
					*p1 = *p2;

				rcode = Usb.outTransfer(devAddress, epDataOut, len, (char*)data);

				if (rcode)
				{
					Message(PSTR("Transaction: Data block send error."), rcode);
					return PTP_RC_GeneralError;
				}
				//Message(PSTR("Transaction: Data block successfully sent."), rcode);
				//delay(50);
			}
		}

		// Because inTransfer does not return the actual number of bytes recieved, it should be 
		// calculated here.
		uint32_t	total = 0, data_off = 0;
		uint8_t		inbuffer = 0;
		uint16_t	loops = 0;
		uint8_t		timeoutcnt = 0;

		while (1)
		{
			ZerroMemory(PTP_MAX_RX_BUFFER_LEN, data);

			rcode = Usb.inTransfer(devAddress, epDataIn, PTP_MAX_RX_BUFFER_LEN, (char*)data);

			if (rcode)
			{
				// in some cases NAK handling might be necessary
				Message(PSTR("Transaction: Response recieve error."), rcode);
				return PTP_RC_GeneralError;
			}

			// This can occure in case of unsupported operation or successive response after data reception stage
			if (*((uint16_t*)(data + PTP_CONTAINER_CONTYPE_OFF)) == PTP_USB_CONTAINER_RESPONSE)
			{
				uint16_t	response = *((uint16_t*)(data + PTP_CONTAINER_OPCODE_OFF));

				if (response == PTP_RC_OK && *((uint32_t*)data) > PTP_USB_BULK_HDR_LEN)
				{
					// number of params = (container length - 12) / 4
					uint8_t	n = (*((uint32_t*)data) - PTP_USB_BULK_HDR_LEN) >> 2;
					flags->rsParams = n;

					for (uint32_t *p1 = (uint32_t*)(data + PTP_USB_BULK_HDR_LEN), *p2 = (uint32_t*)params; n--; p1++, p2++)
						p2 = p1;
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
			//delay(100);
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
			Message(PSTR("EventCheck: USB error: "), rcode);
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
	uint16_t	ptp_error = PTP_RC_GeneralError;

	idSession ++;
	
	Message(PSTR("Opening session #: "), idSession);

	uint32_t	params[1];
	params[0]	= idSession;

	OperFlags	flags = { 1, 0, 0, 0, 0, 0 };

	if ( (ptp_error = Transaction(PTP_OC_OpenSession, &flags, params)) != PTP_RC_OK)
		Message(PSTR("OpenSession: Transaction error."), ptp_error);
	else
		Message(PSTR("OpenSession: Session opened."), ptp_error);

	return ptp_error;
}

uint16_t PTP::ResetDevice()
{
	uint16_t	ptp_error = PTP_RC_GeneralError;
	OperFlags	flags = { 0, 0, 0, 0, 0, 0 };

	if ( (ptp_error = Transaction(PTP_OC_ResetDevice, &flags)) != PTP_RC_OK)
		Message(PSTR("ResetDevice: Error."), ptp_error);

	return ptp_error;
}

uint16_t PTP::PowerDown()
{
	uint16_t	ptp_error = PTP_RC_GeneralError;
	OperFlags	flags = { 0, 0, 0, 0, 0, 0 };

	if ( (ptp_error = Transaction(PTP_OC_PowerDown, &flags)) != PTP_RC_OK)
		Message(PSTR("PowerDown: Error: "), ptp_error);

	return ptp_error;
}

uint16_t PTP::SelfTest(uint16_t type = 0)
{
	uint16_t	ptp_error = PTP_RC_GeneralError;
	OperFlags	flags = { 1, 0, 0, 0 };
	uint32_t	params[1];
	params[0]	= type;

	if ( (ptp_error = Transaction(PTP_OC_SelfTest, &flags, params)) != PTP_RC_OK)
		Message(PSTR("SelfTest: Error."), ptp_error);

	return ptp_error;
}

uint16_t PTP::CloseSession()
{
	uint16_t	ptp_error = PTP_RC_GeneralError;
	
	Message(PSTR("Closing session #: "), idSession);

	OperFlags	flags = { 0, 0, 0, 0, 0, 0 };

	if ( (ptp_error = Transaction(PTP_OC_CloseSession, &flags)) != PTP_RC_OK)
		Message(PSTR("CloseSession error."), ptp_error);
	else
		Message(PSTR("Session closed."), ptp_error);

	return ptp_error;
}

uint16_t PTP::GetDeviceInfo(PTPReadParser *parser)
{
	uint16_t	ptp_error = PTP_RC_GeneralError;
	OperFlags	flags = { 0, 0, 0, 1, 1, 0 };

	if ( (ptp_error = Transaction(PTP_OC_GetDeviceInfo, &flags, NULL, parser)) != PTP_RC_OK)
		Message(PSTR("GetDeviceInfo: Årror."), ptp_error);

	return ptp_error;
}

uint16_t PTP::GetDevicePropDesc(const uint16_t pcode, PTPReadParser *parser)
{
	uint16_t	ptp_error	= PTP_RC_GeneralError;
	OperFlags	flags		= { 1, 0, 0, 1, 1, 0 };
	uint32_t	params[1];

	params[0] = (uint32_t)pcode;

	if ( (ptp_error = Transaction(PTP_OC_GetDevicePropDesc, &flags, params, parser)) != PTP_RC_OK)
		Message(PSTR("GetDevicePropDesc: Årror."), ptp_error);

	return ptp_error;
}

uint16_t PTP::GetDevicePropValue(const uint16_t pcode, PTPReadParser *parser)
{
	uint16_t	ptp_error	= PTP_RC_GeneralError;
	OperFlags	flags		= { 1, 0, 0, 1, 1, 0 };
	uint32_t	params[1];

	params[0] = (uint32_t)pcode;

	if ( (ptp_error = Transaction(PTP_OC_GetDevicePropValue, &flags, params, parser)) != PTP_RC_OK)
		Message(PSTR("GetDevicePropValue: Årror."), ptp_error);

	return ptp_error;
}

uint16_t PTP::ResetDevicePropValue(const uint16_t pcode)
{
	uint16_t	ptp_error	= PTP_RC_GeneralError;
	OperFlags	flags		= { 1, 0, 0, 0 };
	uint32_t	params[1];

	params[0] = (uint32_t)pcode;

	if ( (ptp_error = Transaction(PTP_OC_ResetDevicePropValue, &flags, params)) != PTP_RC_OK)
		Message(PSTR("ResetDevicePropValue: Årror."), ptp_error);

	return ptp_error;
}

//uint16_t PTP::EOSSetEventMask()
//{
//	uint16_t	ptp_error	= PTP_RC_GeneralError;
//	OperFlags	flags		= { 1, 0, 0, 0, 0, 0 };
//	uint32_t	params[3];
//
//	// 0x911a
//	//params[0] = 0x7fffffff;
//	//params[1] = 0x00001000;
//	//params[2] = 0x00000001;
//
//	//params[0] = 0x00000001;
//
//	//if ( (ptp_error = Transaction(0x9115, &flags, params)) != PTP_RC_OK)
//	//	Message(PSTR("EOSSetEventMask: 0x9115 Error."), ptp_error);
//	//else
//	//	Message(PSTR("EOSSetEventMask: 0x9115 Success."), ptp_error);
//
//	//delay(100);
//
//	// 0x9114 - device mode selection
//	params[0] = 0x00000001;
//
//	if ( (ptp_error = Transaction(0x9114, &flags, params)) != PTP_RC_OK)
//		Message(PSTR("EOSSetEventMask: 0x9114 Error."), ptp_error);
//	else
//		Message(PSTR("EOSSetEventMask: 0x9114 Success."), ptp_error);
//	return ptp_error;
//}
//
//uint16_t PTP::EOSSet()
//{
//	uint16_t	ptp_error	= PTP_RC_GeneralError;
//	OperFlags	flags		= { 0, 0, 1, 1, 3, 12 };
//	uint32_t	params[3];
//
//	//flags.opParams = 0;
//	//flags.rsParams = 1;
//	//flags.txOperation = 0;
//	//flags.dataStage = 0;
//
//	for (uint8_t i=0; i<0xFF; i+=1)
//	{
//		// ISO
//		params[0] = 0x0000000C;
//		params[1] = 0x0000D103;
//		params[2] = (uint32_t)i; //0x00000080;
//
//		// F
//		//params[0] = 0x0000000C;
//		//params[1] = 0x0000D101;
//		//params[2] = (uint32_t)i; //0x0000002D;
//
//		// T
//		//params[0] = 0x0000000C;
//		//params[1] = 0x0000D102;
//		//params[2] = (uint32_t)i; //0x0000002D;
//
//		if ( (ptp_error = Transaction(0x9110, &flags, NULL, (void*)params)) != PTP_RC_OK)
//			Message(PSTR("EOSSet: Error."), ptp_error);
//		else
//			Message(PSTR("EOSSet: Success."), ptp_error);
//
//		delay(2500);
//	}
//	return ptp_error;
//}
//
//uint16_t PTP::EOS9110(uint8_t size, void *params)
//{
//	uint16_t	ptp_error	= PTP_RC_GeneralError;
//	OperFlags	flags		= { 0, 0, 1, 1, 3, 12 };
//
//	flags.dataSize = size;
//
//	if ( (ptp_error = Transaction(0x9110, &flags, NULL, params)) != PTP_RC_OK)
//		Message(PSTR("EOSSet: Error."), ptp_error);
//	else
//		Message(PSTR("EOSSet: Success."), ptp_error);
//
//	return ptp_error;
//}

uint16_t PTP::Operation(uint16_t opcode, uint8_t nparams = 0, uint32_t *params = NULL)
{
	uint16_t	ptp_error	= PTP_RC_GeneralError;
	OperFlags	flags		= { 0, 0, 0, 0, 0, 0 };

	flags.opParams = nparams;

	if ( (ptp_error = Transaction(opcode, &flags, params)) != PTP_RC_OK)
		Message(PSTR("Operation: Error."), ptp_error);

	return ptp_error;
}

uint16_t PTP::GetStorageInfo(uint32_t storage_id, PTPReadParser *parser)
{
	uint16_t	ptp_error	= PTP_RC_GeneralError;
	OperFlags	flags		= { 1, 0, 0, 1, 1, 0 };

	uint32_t	params[1]; 
	params[0]	= storage_id;

	if ( (ptp_error = Transaction(PTP_OC_GetStorageInfo, &flags, params, parser)) != PTP_RC_OK)
		Message(PSTR("GetStorageInfo: Error."), ptp_error);

	return ptp_error;
}

uint16_t PTP::FormatStore(uint32_t storage_id, uint32_t fsformat)
{
	uint16_t	ptp_error	= PTP_RC_GeneralError;
	OperFlags	flags		= { 2, 0, 0, 0, 0, 0 };

	uint32_t	params[2]; 
	params[0]	= storage_id;
	params[1]	= fsformat;

	if ( (ptp_error = Transaction(PTP_OC_FormatStore, &flags, params)) != PTP_RC_OK)
		Message(PSTR("FormatStore: Error."), ptp_error);

	return ptp_error;
}

//uint16_t PTP::EOS911B()
//{
//	uint16_t	ptp_error	= PTP_RC_GeneralError;
//	OperFlags	flags		= { 0, 0, 0, 0, 0, 0 };
//	uint32_t	params[7];
//
//	if ( (ptp_error = Transaction(0x911B, &flags)) != PTP_RC_OK)
//		Message(PSTR("EOS911B: Error."), ptp_error);
//	else
//		Message(PSTR("EOS911B: Success."), ptp_error);
//
//	return ptp_error;
//}
//
//uint16_t PTP::EOS9125()
//{
//	uint16_t	ptp_error	= PTP_RC_GeneralError;
//	OperFlags	flags		= { 0, 0, 0, 0, 0, 0 };
//	uint32_t	params[7];
//
//	if ( (ptp_error = Transaction(0x9125, &flags)) != PTP_RC_OK)
//		Message(PSTR("EOS9125: Error."), ptp_error);
//	else
//		Message(PSTR("EOS9125: Success."), ptp_error);
//
//	return ptp_error;
//}
//
//uint16_t PTP::EOS9126()
//{
//	uint16_t	ptp_error	= PTP_RC_GeneralError;
//	OperFlags	flags		= { 0, 0, 0, 0, 0, 0 };
//	uint32_t	params[7];
//
//	if ( (ptp_error = Transaction(0x9126, &flags)) != PTP_RC_OK)
//		Message(PSTR("EOS9126: Error."), ptp_error);
//	else
//		Message(PSTR("EOS9126: Success."), ptp_error);
//
//	return ptp_error;
//}
//
//uint16_t PTP::EOS911A(uint32_t *p)
//{
//	uint16_t	ptp_error	= PTP_RC_GeneralError;
//	OperFlags	flags		= { 3, 0, 0, 0, 0, 0 };
//
//	if ( (ptp_error = Transaction(0x911A, &flags, p)) != PTP_RC_OK)
//		Message(PSTR("EOS911A: Error."), ptp_error);
//	else
//		Message(PSTR("EOS911A: Success."), ptp_error);
//
//	return ptp_error;
//}
//
//uint16_t PTP::EOS911C()
//{
//	uint16_t	ptp_error	= PTP_RC_GeneralError;
//	OperFlags	flags		= { 0, 0, 0, 0, 0, 0 };
//
//	if ( (ptp_error = Transaction(0x911C, &flags)) != PTP_RC_OK)
//		Message(PSTR("EOS911C: Error."), ptp_error);
//	else
//		Message(PSTR("EOS911C: Success."), ptp_error);
//
//	return ptp_error;
//}
//
//uint16_t PTP::EOS9155(uint32_t *p)
//{
//	uint16_t	ptp_error	= PTP_RC_GeneralError;
//	OperFlags	flags		= { 1, 0, 0, 0, 0, 0 };
//	uint32_t	params[1];
//
//	if ( (ptp_error = Transaction(0x9155, &flags, p)) != PTP_RC_OK)
//		Message(PSTR("EOS9155: Error."), ptp_error);
//	else
//		Message(PSTR("EOS9155: Success."), ptp_error);
//
//	return ptp_error;
//}
//
//uint16_t PTP::EOS9158(uint32_t *p)
//{
//	uint16_t	ptp_error	= PTP_RC_GeneralError;
//	OperFlags	flags		= { 1, 0, 0, 0, 0, 0 };
//	uint32_t	params[1];
//
//	if ( (ptp_error = Transaction(0x9158, &flags, p)) != PTP_RC_OK)
//		Message(PSTR("EOS9158: Error."), ptp_error);
//	else
//		Message(PSTR("EOS9158: Success."), ptp_error);
//
//	return ptp_error;
//}

uint16_t PTP::CaptureImage()
{
	uint16_t	ptp_error = PTP_RC_GeneralError;
	uint32_t	params[2] = {0, 0x00003801};

	Notify(PSTR("CaptureImage: Initiating capture...\n"));

	OperFlags	flags = { 2, 0, 0, 0 };

	if ( (ptp_error = Transaction(PTP_OC_InitiateCapture, &flags, params)) != PTP_RC_OK)
	{
		Message(PSTR("CaptureImage: Transaction error."), ptp_error);
		return ptp_error;
	}
	PTPUSBEventContainer	evnt;
	bool					occured;

	// multiple objects can be added depending on current camera shooting mode
	while ((occured = EventWait(sizeof(PTPUSBEventContainer), (uint8_t*)&evnt, 500)) && evnt.code == PTP_EC_ObjectAdded)
		Message(PSTR("CaptureImage: New object added."));
	
	if (!occured)
	{
		Notify(PSTR("CaptureImage: Timeout ellapsed."));
		return PTP_RC_Undefined;
	}
	switch (evnt.code)
	{
	case PTP_EC_CaptureComplete:
		Notify(PSTR("CaptureImage: Image captured."));
		return PTP_RC_OK;

	case PTP_EC_StoreFull:
		Notify(PSTR("CaptureImage: Storage is full."));
		return PTP_RC_StoreFull;

	default:
		Message(PSTR("CaptureImage: Unexpected event"), evnt.code);
		return PTP_RC_Undefined;
	}
}

//uint16_t PTP::GetDevicePropValue(uint16_t pcode, PTPDevicePropDesc *prop)
//{
//	uint16_t	ptp_error = PTP_RC_Undefined;
//
//	Message(PSTR("GetDevicePropValue: Getting dev. prop. val..."), ptp_error);
//
//	uint32_t	params[1];
//	params[0]	= (uint32_t)pcode;
//
//	OperFlags	flags = { 1, 0, 0, 1 };
//
//	if ( (ptp_error = Transaction(PTP_OC_GetDevicePropValue, &flags, params)) != PTP_RC_OK)
//		Message(PSTR("GetDevicePropValue: Unable to get dev. prop. val."), ptp_error);
//		
//	return ptp_error;
//}

uint16_t PTP::GetStorageIDs(PTPReadParser *parser)
{
	uint16_t	ptp_error = PTP_RC_Undefined;

	Notify(PSTR("GetStorageIDs: Getting storage IDs...\n"));

	OperFlags	flags = { 0, 0, 0, 1, 1, 0 };

	if ( (ptp_error = Transaction(PTP_OC_GetStorageIDs, &flags, NULL, parser)) != PTP_RC_OK)
		Message(PSTR("GetStorageIDs: Transaction error."), ptp_error);
	return ptp_error;
}

uint16_t PTP::GetStorageIDs(uint8_t bufsize, uint8_t *pbuf)
{
	uint16_t	ptp_error = PTP_RC_Undefined;

	//Notify(PSTR("GetStorageIDs: Getting storage IDs...\n"));

	OperFlags	flags = { 0, 0, 0, 1, 3, 0 };

	flags.dataSize = bufsize;

	if ( (ptp_error = Transaction(PTP_OC_GetStorageIDs, &flags, NULL, pbuf)) != PTP_RC_OK)
		Message(PSTR("GetStorageIDs: Transaction error."), ptp_error);

	return ptp_error;
}

uint16_t PTP::GetObjectHandles(uint32_t storage_id, uint16_t format, uint16_t assoc, PTPReadParser *parser)
{
	uint16_t	ptp_error = PTP_RC_Undefined;

	Notify(PSTR("GetObjectHandles: Getting object handles...\n"));

	OperFlags	flags = { 3, 0, 0, 1, 1, 0 };
	uint32_t	params[3];

	params[0] = storage_id;
	params[1] = (uint32_t)format;
	params[2] = (uint32_t)assoc;

	if ( (ptp_error = Transaction(PTP_OC_GetObjectHandles, &flags, params, parser)) != PTP_RC_OK)
		Message(PSTR("GetObjectHandles: Transaction error."), ptp_error);
	return ptp_error;
}

//void PTP::PrintDevicePropValue(PTPDevicePropDesc *prop)
//{
//	char c, *msg = NULL;
//	
//	if (prop && prop->DevicePropertyCode >= PTP_DPC_Undefined && prop->DevicePropertyCode <= PTP_DPC_CopyrightInfo )
//		msg = (char*)pgm_read_word(&dpValNames[(prop->DevicePropertyCode & 0xFF)]);
//	else
//		return;
//
//	if(!msg) return;
//
//	while((c = pgm_read_byte(msg++)))
//		Serial.print(c,BYTE);
//
//	Serial.println(0,BYTE);
//}

void PTP::Task()
{
    Max.Task();
    Usb.Task();

	//wait for addressing state
    if( Usb.getUsbTaskState() == USB_STATE_CONFIGURING ) 
	{  
		EP_RECORD			epRecord[ 4 ];

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
	    
//////////////////////////////////////////////////////
		GetConfDescr(devAddress, numConf);
//////////////////////////////////////////////////////

		uint8_t	rcode;

	    /* Configure device */
	    if ((rcode = Usb.setConf( devAddress, 0, numConf )))
	        HaltOnError(MsgErrDeviceConf, rcode);
		
	    Message(PSTR("Device configured."));

		if (pfRunning)
			pfRunning();

	    delay(20000);
        Usb.setUsbTaskState( USB_STATE_RUNNING );
    }
    if( Usb.getUsbTaskState() == USB_STATE_RUNNING ) 
	{  
        //kbd_poll();
		//CaptureImage();
		//while(1);
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

//uint8_t PTP::GetConfDescriptor(PTPReadParser *pf)
//{
//	uint8_t buf[ PTP_MAX_RX_BUFFER_LEN ];
//	byte rcode;
//	byte descr_length;
//	byte descr_type;
//	unsigned int total_length;
//	rcode = Usb.getConfDescr( addr, 0, 4, conf, buf );  //get total length
//	LOBYTE( total_length ) = buf[ 2 ];
//	HIBYTE( total_length ) = buf[ 3 ];
//	if( total_length > BUFSIZE ) {    //check if total length is larger than buffer
//	Serial.println("Total length truncated to 256 bytes");
//	total_length = BUFSIZE;
//	}
//	rcode = Usb.getConfDescr( addr, 0, total_length, conf, buf ); //get the whole descriptor
//}