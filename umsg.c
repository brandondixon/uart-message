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
	psMsgObject->ui32Flags = MSG_OBJ_NO_FLAGS;

	uint16_t ui16CRCcalc = 0;		// Initilize CRC value

	// Print out the contents of the message that was received
	UARTCharPutNonBlocking(ui32Base, UMSG_START_BYTE);
	UARTCharPutNonBlocking(ui32Base, UMSG_START_BYTE);

	// Convert message ID to array
	uint8_t ui8MsgID[2] = {psMsgObject->ui16MsgID >> 8, psMsgObject->ui16MsgID & 0x00FF};
	ui16CRCcalc = Crc16(ui16CRCcalc, ui8MsgID, 2);

	// Send message id
	UARTCharPutNonBlocking(ui32Base, ui8MsgID[0]);
	UARTCharPutNonBlocking(ui32Base, ui8MsgID[1]);

	// Send data
	unsigned int uIdx;
	for(uIdx = 0; uIdx < psMsgObject->ui32MsgLen; uIdx++)
	{
		uint8_t ui8MsgByte[1] = { psMsgObject->pui8MsgData[uIdx] };
		UARTCharPutNonBlocking(ui32Base, ui8MsgByte[0]);
		ui16CRCcalc = Crc16(ui16CRCcalc, ui8MsgByte, 1);
	}

	// Calculate the checksum
	uint8_t ui8CRCcalc[2] = { ui16CRCcalc >> 8, ui16CRCcalc & 0x00FF };
	UARTCharPutNonBlocking(ui32Base, ui8CRCcalc[0]);
	UARTCharPutNonBlocking(ui32Base, ui8CRCcalc[1]);
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
	psMsgObject->ui32Flags = MSG_OBJ_NO_FLAGS;

	if ( UMSG_START_BYTE == UARTCharGetNonBlocking(ui32Base) )
	{
		if ( UMSG_START_BYTE == UARTCharGetNonBlocking(ui32Base) )
		{
			uint8_t ui8MsgID[2] = {0,0};	// Initilize Message ID array
			uint16_t ui16MsgID = 0x0000;	// Initilize Message ID

			uint8_t ui8CRCrx[2] = {0,0};	// Initilize CRC received array
			uint16_t ui16CRCrx = 0x0000;	// Initilize CRC received value
			uint16_t ui16CRCcalc = 0;		// Initilize CRC calculated value

			// Convert message ID to array
			ui8MsgID[0] = UARTCharGetNonBlocking(ui32Base);
			ui8MsgID[1] = UARTCharGetNonBlocking(ui32Base);
			ui16CRCcalc = Crc16(ui16CRCcalc, ui8MsgID, 2);
			ui16MsgID = ui8MsgID[0] << 8 | ui8MsgID[1];
			psMsgObject->ui16MsgID = ui16MsgID;

			// Receive data
			unsigned int uIdx;
			for(uIdx = 0; uIdx < psMsgObject->ui32MsgLen; uIdx++)
			{
				uint8_t ui8MsgByte[1] = { UARTCharGetNonBlocking(ui32Base) };
				psMsgObject->pui8MsgData[uIdx] = ui8MsgByte[0];
				ui16CRCcalc = Crc16(ui16CRCcalc, ui8MsgByte, 1);
			}

			// Convert message ID to array
			ui8CRCrx[0] = UARTCharGetNonBlocking(ui32Base);
			ui8CRCrx[1] = UARTCharGetNonBlocking(ui32Base);
			ui16CRCrx = Crc16(ui16CRCcalc, ui8CRCrx, 2);

			// Convert recived values to ui16
			ui16CRCrx = ui8CRCrx[0] << 8 | ui8CRCrx[1];

			if ( ui16CRCrx == ui16CRCcalc )
			{
				psMsgObject->ui32Flags |= MSG_OBJ_NEW_DATA;
			} else {
				psMsgObject->ui32Flags |= MSG_OBJ_DATA_LOST;
			}
		}
	}
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
