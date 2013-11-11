// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//



/**
 @file
 @publishedAll
 @released
*/

#ifndef __ETELEXT_H__
#define __ETELEXT_H__

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif // __E32BASE_H__

//
// TEtelDataPackageType
// Data package description that passing between client to Server
// specified in p[1]
//
enum TEtelDataPackageType
	{
	EIsaNull =0,
	EIsaDesTobeSet =1,
	EIsaDesTobeRead =2,
	EIsaDoubleDesTobeSet =3,
	EIsaDoubleDesTobeRead =4,
	EIsaCancelMessage =5,
	EIsaCancelSubSession =6,
	EIsaNarrowAndUnicodeDoubleDesTobeSet = 7,	// narrow comes first then unicode
	EIsaNarrowAndUnicodeDoubleDesTobeRead = 8,
	EIsaNarrowDesToSetAndGet = 9,
	EIsaNarrowDesToGetUnicodeDesToSet = 10,
	EIsaUnicodeDesToSetAndGet = 11,
	EIsaUnicodeDesToGetNarrowDesToSet = 12,

	EIsaUnicodeDesTobeSet =0x0101,
	EIsaUnicodeDesTobeRead = 0x0102,
	EIsaUnicodeDoubleDesTobeSet =0x0103,
	EIsaUnicodeDoubleDesTobeRead =0x0104,

	EIsPriorityClientReqWithNull =0x1000,
	EIsPriorityClientReqWithaDesTobeSet =0x1001,
	EIsPriorityClientReqWithaDesTobeRead =0x1002,
	EIsPriorityClientReqWithaDoubleDesTobeSet =0x1003,
	EIsPriorityClientReqWithaDoubleDesTobeRead =0x1004,
	EIsPriorityClientReqWithaNarrowAndUnicodeDoubleDesTobeSet = 0x1007,
	EIsPriorityClientReqWithaNarrowAndUnicodeDoubleDesTobeRead = 0x1008,
	EIsPriorityClientReqWithaNarrowDesToSetAndGet = 0x1009,
	EIsPriorityClientReqWithaNarrowDesToGetUnicodeDesToSet  = 0x1010,
	EIsPriorityClientReqWithaUnicodeDesToSetAndGet = 0x1011,
	EIsPriorityClientReqWithaDesToGetNarrowDesToSet = 0x1012,

	EIsPriorityClientReqWithaUnicodeDesTobeSet =0x1101,
	EIsPriorityClientReqWithaUnicodeDesTobeRead = 0x1102,
	EIsPriorityClientReqWithaUnicodeDoubleDesTobeSet = 0x1103,
	EIsPriorityClientReqWithaUnicodeDoubleDesTobeRead = 0x1104
	};

const TInt KUnicodeReq			=0x0100;
const TInt KPriorityClientReq	=0x1000;
const TInt KEmergencyClientReq	=0x1000;

/**
ETel Panics
*/
enum TEtelPanic
	{
	EEtelPanicBadPhoneHandle,      // 0
	EEtelPanicBadDescriptor,       // 1
	EEtelPanicInvalidRequestType,  // 2
	EEtelPanicRequestAsyncTwice,   // 3
	EEtelPanicBadSubSessionHandle, // 4
	EEtelPanicZeroLengthDes,       // 5
	EEtelPanicDesLengthNegative,   // 6
	EEtelPanicNullPtrArray,        // 7
	EEtelPanicNullHandle,          // 8
	EEtelPanicHandleNotOpen,       // 9
	EEtelPanicIndexOutOfRange,     // 10
	EEtelPanicHandleNotClosed,     // 11
	EEtelPanicBadCallGroupType,    // 12
	EEtelPanicBadName,             // 13
	EEtelPanicExceededAllowedNoOfEmergencyClientSessions //14
	};

/**
ETel Faults
*/
enum TEtelFault
	{
	EEtelFaultMainSchedulerError,                 // 0
	EEtelFaultSvrCreateServer,                    // 1
	EEtelFaultSvrStartServer,                     // 2
	EEtelFaultCommServerNotRunning,               // 3
	EEtelFaultCreateTrapCleanup,                  // 4
	EEtelFaultCompletionReceivedWithNoReqPackage, // 5
	EEtelFaultWaitingReqLostCObject,              // 6
	EEtelFaultCompleteReqWithoutReqEntry,         // 7
	EETelFaultRequestWithoutBuffer,               // 8
	EEtelFaultCallTsyServiceWithoutReqPackage,    // 9
	EEtelFaultMessageTypeCorrupted,               // 10
	EEtelFaultNotRecognisedTsyHandle,             // 11
	EEtelFaultInvalidIpcForCancel,                // 12
	EEtelFaultCanNotRelinquishOwnership,          // 13
	EEtelFaultBadTelSessionPointer,               // 14
	EEtelFaultBadSubSessionHandle,                // 15
	EEtelFaultBadPhoneFactoryPointer,             // 16
	EEtelFaultBadPhonePointer,                    // 17
	EEtelFaultBadTelObjectPointer,                // 18
	EEtelFaultDes1DoesNotExist,                   // 19
	EEtelFaultDes2DoesNotExist,                   // 20
	EEtelFaultNegativeFlowcontrolCount,           // 21
	EEtelFaultNegativeSessionCount,               // 22
	EEtelFaultNegativeActiveReqCount,             // 23
	EEtelFaultBadMessageType,                     // 24
	EEtelFaultClientNotPostedNotification,        // 25
	EEtelFaultUpdatingBufferOfDifferentIpc,       // 26
	EEtelFaultUpdatingIdenticalBuffer,            // 27
	EEtelFaultMaxDesLengthNegative,               // 28
	EEtelFaultPreviousReqNonNull,                 // 29
	EEtelFaultNullBooleanArrayPointer,            // 30
	EEtelFaultCancelErrorWithoutCancelled,        // 31
	};

#define ETEL_SERVER_NAME	  _L("!EtelServer")  // EKA2 Server names need a "!" prepended

#define ETEL_SERVER_START_SEQUENCE 5

#endif // __ETELEXT_H__
