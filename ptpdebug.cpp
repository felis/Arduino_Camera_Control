#include "ptpdebug.h"

void HexDump::Parse(const uint16_t len, const uint8_t *pbuf, const uint32_t &offset)
{
	for (uint16_t j=0; j<len; j++, byteCount++, byteTotal++)
	{
                if (!byteCount)
                {
                        uint16_t    mask = 0x1000;
                        
                        while (mask > 1)
                        {
        		        if (byteTotal < mask)
        			      Serial.print("0");
                                mask >>= 4;
                        }
                        Serial.print(byteTotal, HEX);
                        Serial.print(":");
                }
		if (pbuf[j] < 0x10)
			Serial.print("0");

		Serial.print((unsigned char)pbuf[j], HEX);
		Serial.print(" ");

		if (byteCount == 15)
		{
			Serial.println("");
			byteCount = 0xFF;
		}
	}
	//Serial.println("");
}
