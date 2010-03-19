#ifndef __PTP_H__
#define __PTP_H__

#include <inttypes.h>
#include <avr/pgmspace.h>
#include <Max3421e.h>
#include <Usb.h>
#include "ptpconst.h"
#include "ptpmsgstr.h"

// Buffer size should NEVER be less than USB packet size!!!!!!!!!!!!!!!!!!!!!
#define PTP_MAX_RX_BUFFER_LEN	64
//#define PTP_MAX_TX_BUFFER_LEN	128
#define PTP_MAX_EV_BUFFER_LEN	 8

//#define PTP_MAX_CMD_RSP_BUFFER_LEN	 28

void Notify(const char* msg);
void Message(const char* msg, uint16_t rcode);

// Base class for incomming data parser
class PTPReadParser
{
public:
	virtual void Parse(const uint16_t len, const uint8_t *pbuf, const uint32_t &offset) = 0;
};

// Template for multibyte value parsing. 
// T   - type of value being parsed
template <class T> 
class MultiByteValueParser
{
	uint8_t	valBuf[sizeof(T)];
	uint8_t countDown;

public:
	MultiByteValueParser() : countDown(sizeof(T)) {};
	void Reset() { countDown = sizeof(T); };
	bool Parse(uint8_t **pp, uint16_t *pcntdn, T *pret);
};

template <class T> 
bool MultiByteValueParser<T>::Parse(uint8_t **pp, uint16_t *pcntdn, T *pret)
{
	for (; countDown && (*pcntdn); countDown--, (*pcntdn)--, (*pp)++)
		valBuf[sizeof(T)-countDown] = (**pp);
	
	if (countDown)
		return false;

	//(*pret) = *((T*)valBuf);

	for (uint8_t i=0; i<sizeof(T); i++)
		((uint8_t*)pret)[i] = valBuf[i];

	countDown = sizeof(T);
	return true;
}

// Pointer to a callback function triggered for each element of PTP array when used with PTPArrayParser
typedef void (*PTP_ARRAY_EL_FUNC)(uint32_t);

// Template for PTP array parsing
// ARR_EL_TYPE	- array element type
template <class ARR_EL_TYPE> 
class PTPArrayParser
{
	uint8_t		nStage;
	uint32_t	arLen;
	uint32_t	arLenCntdn;

	// Array size parser
	MultiByteValueParser<uint32_t>				sizeParser;

	// Array element parser
	MultiByteValueParser<ARR_EL_TYPE>			elemParser;

public:
	PTPArrayParser() : nStage(0), arLenCntdn(0), arLen(0) {};
	bool Parse(uint8_t **pp, uint16_t *pcntdn, PTP_ARRAY_EL_FUNC pf);
};

// pp		- data buffer current position pointer
// cntdn	- parsed data countdown
// pf		- callback function pointer
template <class ARR_EL_TYPE> 
bool PTPArrayParser<ARR_EL_TYPE>::Parse(uint8_t **pp, uint16_t *pcntdn, PTP_ARRAY_EL_FUNC pf)
{
	switch (nStage)
	{
	case 0:
		(*pp) += 12;
		(*pcntdn) -= 12;
		nStage ++;

	case 1:
		if (!sizeParser.Parse(pp, pcntdn, &arLen))
			return false;

		Serial.println(arLen, DEC);

		arLenCntdn = arLen;
		nStage ++;

	case 2:
		for ( ;arLenCntdn; arLenCntdn--)
		{
			ARR_EL_TYPE		element;

			if (!elemParser.Parse(pp, pcntdn, &element))
				return false;

			//Serial.print((uint16_t)(*pp), HEX);
			//Serial.print("\t");
			//Serial.print((*pcntdn), DEC);
			//Serial.print("\t");
			//Serial.println(element, HEX);

			if (pf)
				pf(element);
			//elemParser.Reset();
		}
		//nStage ++;
		nStage = 0;
	}
}

class PTPObjHandleParser : public PTPReadParser
{
	uint8_t							nStage;
	PTPArrayParser<uint32_t>		arrayParser;

	static void PrintHandle(uint32_t element) { static uint32_t count = 0; Serial.print(count++, DEC); Serial.print("\t"); Serial.println(element, HEX); };

public:
	PTPObjHandleParser() : nStage(0) {};
	virtual void Parse(const uint16_t len, const uint8_t *pbuf, const uint32_t &offset);
};

template <class T> 
void PrintHex(T val)
{
	//Serial.println ((((sizeof(T) << 1) - 1) << 2), HEX);

    T    mask = (((T)1) << (((sizeof(T) << 1) - 1) << 2));
    
	//Serial.println (mask, HEX);

    while (mask > 1)
    {
		if (val < mask)
		  Serial.print("0");

		mask >>= 4;
    }
    Serial.print(val, HEX);
}


// Base class for outgoing data supplier
class PTPDataSupplier
{
public:
	virtual uint32_t GetDataSize() = 0;
	virtual void GetData(const uint16_t len, uint8_t *pbuf) = 0;
};

typedef void (*PTPMAIN)();

class PTP
{
#ifdef PTP_HANDLE_RESPONSES
	static const char* stdResponse[];
#endif

	//static const char* dpValNames[];
	static const char* dtNames1[];
	static const char* dtNames2[];

	uint8_t				devAddress;
	uint8_t				epDataIn;
	uint8_t				epDataOut;
	uint8_t				epInterrupt;
	uint8_t				numConf;

	MAX3421E			Max;
	USB					Usb;

	uint16_t			idTransaction;
	uint16_t			idSession;

	PTPMAIN				pfRunning;

protected:
	struct OperFlags
	{
		uint16_t	opParams	:	3;			// 7	- maximum number of operation parameters
		uint16_t	rsParams	:	3;			// 7	- maximum number of response parameters
		uint16_t	txOperation	:	1;			// I->R operation if the flag is set
		uint16_t	dataStage	:	1;			// operation has data stage if the flag is set
		uint16_t	typeOfVoid	:	2;			// 0 - NULL, 1 - READPARSER, 2 - WRITEPARSER, 3 - buffer pointer
		uint16_t	dataSize	:	6;			// size of data buffer (64 bytes maximum)		
	};
	typedef void (*READPARSER)(const uint16_t len, const uint8_t *pbuf, const uint32_t &offset);

	void ZerroMemory(uint8_t size, uint8_t *mem) { for (uint8_t i=0; i<size; i++) mem[i] = 0; };

	// waits for any event to occure
	// returns event on success or error code if timeout elapsed
	bool EventWait(uint8_t size, uint8_t *event_buf, uint16_t timeout);

	// returns true if event occured
	// the actual data is stored in a buffer pointed by buf
	bool CheckEvent(uint8_t size, uint8_t *buf);

	uint16_t Transaction(uint16_t opcode, OperFlags *flags, uint32_t *params, void *pVoid);

	void HaltOnError(const char* msg, uint16_t rcode) { Message(msg, rcode); while(1); };
	//void PrintDevicePropValue(PTPDevicePropDesc *prop);

#ifdef PTP_HANDLE_RESPONSES
	// outputs human readable response messages
	void HandleResponse(uint16_t rc);
#endif

public:
	PTP(uint8_t addr, uint8_t epin, uint8_t epout, uint8_t epint, uint8_t nconf, PTPMAIN pfunc);

	virtual uint16_t EventCheck(PTPReadParser *parser);

	void	Setup()	{ Max.powerOn(); };
	void	Task();

	//uint8_t GetUsbState()	{ return Usb.GetUsbState()	};

	// Simple PTP operation which has no data stage. Any number of uint32_t params can be supplied.
	uint16_t Operation(uint16_t opcode, uint8_t nparams, uint32_t *params);

	uint16_t CaptureImage();

	uint16_t OpenSession();
	uint16_t CloseSession();
	uint16_t ResetDevice();
	uint16_t PowerDown();
	uint16_t SelfTest(uint16_t type);
	uint16_t GetDeviceInfo(PTPReadParser *parser);
	uint16_t GetDevicePropDesc(const uint16_t pcode, PTPReadParser *parser);
	uint16_t GetDevicePropValue(const uint16_t pcode, PTPReadParser *parser);
	uint16_t ResetDevicePropValue(const uint16_t pcode);
	uint16_t GetStorageIDs(PTPReadParser *parser);
	uint16_t GetStorageIDs(uint8_t bufsize, uint8_t *pbuf);
	uint16_t GetStorageInfo(uint32_t storage_id, PTPReadParser *parser);
	uint16_t GetObjectHandles(uint32_t storage_id, uint16_t format, uint16_t assoc, PTPReadParser *parser);
	uint16_t FormatStore(uint32_t storage_id, uint32_t fsformat);

	//uint16_t GetDevicePropValue(uint16_t pcode, PTPDevicePropDesc *prop);

private:
	uint8_t GetConfDescr( byte addr, byte conf );
};

#endif // __PTP_H__