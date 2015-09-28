//*****************************************************************************
//
// umsg.c - Driver for the UMSG module.
//
// Copyright (c) 2015 Sevun Scientific, Inc..  All rights reserved.
// Software License Agreement
// 
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions
//   are met:
// 
//   Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the  
//   distribution.
// 
//   Neither the name of Sevun Scientific, Inc. nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup umsg_api
//! @{
//
//*****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "inc/hw_uart.h"
#include "driverlib/uart.h"
#include "driverlib/sw_crc.h"
#include "umsg.h"

#include "utils/uartstdio.h"

#define UMSG_START_BYTE 0xFF

//*****************************************************************************
//
//! Configures a message object in the UMSG controller.
//!
//! \return None.
//
//*****************************************************************************
void
UARTMessageSet(uint32_t ui32Base, tUARTMsgObject *psMsgObject)
{
    // Calculate checksum
	// Send start bytes, 2 bytes
	// Send message id, 2 bytes
	// Send data
	// Send checksum, 2 bytes

	uint16_t ui16CRCValue = 0;		// Initilize CRC value
	uint32_t ui32MsgLength = psMsgObject->ui32MsgLen;  // Get message length


	// Print out the contents of the message that was received
	UARTCharPutNonBlocking(ui32Base, UMSG_START_BYTE);
	UARTCharPutNonBlocking(ui32Base, UMSG_START_BYTE);

	// Convert message ID to array
	uint8_t ui8MsgID[2] = {psMsgObject->ui16MsgID >> 8, psMsgObject->ui16MsgID & 0x00FF};
	ui16CRCValue = Crc16(ui16CRCValue, ui8MsgID, 2);

	// Send message id
	UARTCharPutNonBlocking(ui32Base, ui8MsgID[0]);
	UARTCharPutNonBlocking(ui32Base, ui8MsgID[1]);

	// Send data
	unsigned int uIdx;
	for(uIdx = 0; uIdx < ui32MsgLength; uIdx++)
	{
		uint8_t ui8MsgByte[1] = { psMsgObject->pui8MsgData[uIdx] };
		UARTCharPutNonBlocking(ui32Base, ui8MsgByte[0]);
		ui16CRCValue = Crc16(ui16CRCValue, ui8MsgByte, 1);
	}

	// Calculate the checksum
	uint8_t ui8CRC[2] = {ui16CRCValue >> 8, ui16CRCValue & 0x00FF};
	UARTCharPutNonBlocking(ui32Base, ui8CRC[0] >> 8);
	UARTCharPutNonBlocking(ui32Base, ui8CRC[1] & 0x00FF);

	UARTprintf("\033[1;40f");	// Set cursor to R4C1
	UARTprintf( "%04x",ui16CRCValue);
}

//*****************************************************************************
//
//! Reads a UMSG message from one of the message object buffers.
//!
//! \return None.
//
//*****************************************************************************
void
UARTMessageGet(uint32_t ui32Base, tUARTMsgObject *psMsgObject)
{
	if ( UMSG_START_BYTE == UARTCharGetNonBlocking(ui32Base) )
	{
		if ( UMSG_START_BYTE == UARTCharGetNonBlocking(ui32Base) )
		{
			// ?FIXME� Probably better way to get message ID
			uint16_t ui16MessageID = 0x0000;
			ui16MessageID = UARTCharGetNonBlocking(ui32Base) << 8 | ui16MessageID;
			ui16MessageID = UARTCharGetNonBlocking(ui32Base) | ui16MessageID;

			psMsgObject->ui16MsgID = ui16MessageID;

			// Receive data
			unsigned int uIdx;

			for(uIdx = 0; uIdx < psMsgObject->ui32MsgLen; uIdx++)
			{
				psMsgObject->pui8MsgData[uIdx] = UARTCharGetNonBlocking(ui32Base);
			}

			uint16_t ui16Checksum = 0x0000;
			ui16Checksum = UARTCharGetNonBlocking(ui32Base) << 8 | ui16Checksum;
			ui16Checksum = UARTCharGetNonBlocking(ui32Base) | ui16Checksum;

			// ?FIXME� Checksum validation
		}
	}
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
