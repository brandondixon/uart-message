//*****************************************************************************
//
// umsg.h - Defines and Macros for the UMSG controller.
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

#ifndef _UMSG_H__
#define _UMSG_H__

//*****************************************************************************
//
//! \addtogroup umsg_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#define UMSG_START_BYTE 0xFF

//*****************************************************************************
//
// Miscellaneous defines for Message ID Types
//
//*****************************************************************************

//*****************************************************************************
//
// These are the flags used by the tUARTMsgObject.ui32Flags value when calling
// the UARTMessageSet() and UARTMessageGet() functions.
//
//*****************************************************************************

//
//! This indicates that new data was available in the message object.
//
#define MSG_OBJ_NEW_DATA        0x00000080

//
//! This indicates that data was lost since this message object was last
//! read.
//
#define MSG_OBJ_DATA_LOST       0x00000100

//
//! This indicates that a message object has no flags set.
//
#define MSG_OBJ_NO_FLAGS        0x00000000

//*****************************************************************************
//
//! The structure used for encapsulating all the items associated with a UART
//! message object
//
//*****************************************************************************
typedef struct
{
    //
    //! The UART message identifier
    //
    uint16_t ui16MsgID;

    //
    //! This value holds various status flags and settings specified by
    //! tUARTObjFlags.
    //
    uint32_t ui32Flags;

    //
    //! This value is the number of bytes of data in the message object.
    //
    uint32_t ui32MsgLen;

    //
    //! This is a pointer to the message object's data.
    //
    uint8_t *pui8MsgData;
}
tUARTMsgObject;

//*****************************************************************************
//
// API Function prototypes
//
//*****************************************************************************
extern void UARTMessageSet(uint32_t ui32Base, tUARTMsgObject *psMsgObject);
extern void UARTMessageGet(uint32_t ui32Base, tUARTMsgObject *psMsgObject);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // _UMSG_H__
