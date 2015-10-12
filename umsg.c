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

#include "driverlib/sw_crc.h"
#include "driverlib/uart.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "inc/hw_uart.h"
#include "umsg.h"

//*****************************************************************************
//
//! Write U8 value to UART and update running CRC from message stream
//!
//! \return None.
//
//*****************************************************************************
void UARTWriteU8(uint32_t ui32Base, const uint8_t writeData, uint16_t *runningCrc)
{
	UARTCharPutNonBlocking(ui32Base, writeData);
	*runningCrc = Crc16(*runningCrc, &writeData, 1);

	return;
}

//*****************************************************************************
//
//! Write U16 value to UART and update running CRC from message stream
//!
//! \return None.
//
//*****************************************************************************
void UARTWriteU16(uint32_t ui32Base, const uint16_t writeData, uint16_t *runningCrc)
{
	UARTWriteU8(ui32Base, (uint8_t)(0xFF & (writeData >> 8)), runningCrc);
	UARTWriteU8(ui32Base, (uint8_t)(0xFF & writeData), runningCrc);

	return;
}

//*****************************************************************************
//
//! Read U8 value from UART and update running CRC from message stream
//!
//! \return U8 data.
//
//*****************************************************************************
uint8_t
UARTReadU8(uint32_t ui32Base, uint16_t *runningCrc)
{
	uint8_t localReadData = UARTCharGetNonBlocking(ui32Base);
	*runningCrc = Crc16(*runningCrc, &localReadData, 1);

	return localReadData;
}

//*****************************************************************************
//
//! Read U16 value from UART and update running CRC from message stream
//!
//! \return U16 data.
//
//*****************************************************************************
uint16_t
UARTReadU16(uint32_t ui32Base, uint16_t *runningCrc)
{
	uint16_t localReadData =
			(((uint16_t)(UARTReadU8(ui32Base, runningCrc)) << 8) & 0xFF00) |
			(((uint16_t)(UARTReadU8(ui32Base, runningCrc))) & 0xFF);

	return localReadData;
}

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

	// dummy CRC value
	uint16_t tempCrc = 0;
	// initilize CRC value
	uint16_t ui16CRCcalc = 0;

	const uint16_t startWord = ((((uint16_t)UMSG_START_BYTE) << 8) & 0xFF00) | ((uint16_t)UMSG_START_BYTE);

	// start new message with 0xFFFF
	UARTWriteU16(ui32Base, startWord, &tempCrc);

	// send message id
	UARTWriteU16(ui32Base, psMsgObject->ui16MsgID, &ui16CRCcalc);

	// send data
	unsigned int uIdx;
	for(uIdx = 0; uIdx < psMsgObject->ui32MsgLen; uIdx++)
	{
		UARTWriteU8(ui32Base, psMsgObject->pui8MsgData[uIdx], &ui16CRCcalc);
	}

	// send CRC
	UARTWriteU16(ui32Base, ui16CRCcalc, &tempCrc);
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

	// dummy CRC value
	uint16_t tempCrc = 0;

	// check if next byte could be start of 0xFFFF word
	if(UMSG_START_BYTE != UARTReadU8(ui32Base, &tempCrc))
	{
		return;
	}

	// if so, check next byte and GO if it matches
	if(UMSG_START_BYTE == UARTReadU8(ui32Base, &tempCrc))
	{
		// initilize CRC received value
		uint16_t ui16CRCrx = 0;
		// initilize CRC calculated value
		uint16_t ui16CRCcalc = 0;

		// read message ID
		psMsgObject->ui16MsgID = UARTReadU16(ui32Base, &ui16CRCcalc);

		// receive data
		unsigned int uIdx;
		for(uIdx = 0; uIdx < psMsgObject->ui32MsgLen; uIdx++)
		{
			psMsgObject->pui8MsgData[uIdx] = UARTReadU8(ui32Base, &ui16CRCcalc);
		}

		// read CRC...
		ui16CRCrx = UARTReadU16(ui32Base, &tempCrc);

		// ...then check for data integrity against calculated value
		if(ui16CRCrx == ui16CRCcalc)
		{
			psMsgObject->ui32Flags |= MSG_OBJ_NEW_DATA;
		}
		else
		{
			psMsgObject->ui32Flags |= MSG_OBJ_DATA_LOST;
		}
	}
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
