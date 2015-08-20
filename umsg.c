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
#include "utils/uartstdio.h"
#include "umsg.h"

#define START_BYTE 0xFF

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

	// Print out the contents of the message that was received
	UARTCharPutNonBlocking(UART2_BASE, START_BYTE);
	UARTCharPutNonBlocking(UART2_BASE, START_BYTE);


	// Send message id
	UARTCharPutNonBlocking(UART2_BASE, psMsgObject->ui16MsgID & 0xFF);
	UARTCharPutNonBlocking(UART2_BASE, psMsgObject->ui16MsgID >> 8);

	// Send data
	unsigned int uIdx;

	for(uIdx = 0; uIdx < psMsgObject->ui32MsgLen; uIdx++)
	{
		UARTCharPutNonBlocking(UART2_BASE, psMsgObject->pui8MsgData[uIdx]);
	}

	// Checksum junk data
	UARTCharPutNonBlocking(UART2_BASE, 0xFE);
	UARTCharPutNonBlocking(UART2_BASE, 0xFE);
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
	// Code here
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
