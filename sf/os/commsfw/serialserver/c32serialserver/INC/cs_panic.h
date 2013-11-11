// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// top-level header file for C32 that defines client and server panics.
// 
//

/**
 @file
 @publishedPartner 
 @released
*/

#ifndef CS_PANIC_H
#define CS_PANIC_H

/**
 * C32 client panic
 *
 * @note Don't change the order of these enums!
 * @note These panics will appear with a category of "!CommServer".
 */
enum TECommPanic
	{
	EWriteTwice                                 = 0,  //< 0 cannot have two outstanding writes from the same client
	EReadTwice                                  = 1,  //< 1 cannot have two outstanding reads from the same client
	EBreakTwice                                 = 2,  //< 2 cannot have two outstanding breaks from the same client
	EBadCommHandle                              = 3,  //< 3 the message handle from the client was no good
	EBadDescriptor                              = 4,  //< 4 client tried to use a bad descriptor
	ESetConfigWhilePendingRequests              = 5,  //< 5 someone tried to config the port when a request was outstanding
	ESetSignalsWhilePendingRequests             = 6,  //< 6 someone tried to set the signals when a request was outstanding
	EResetBuffersWhilePendingRequests           = 7,  //< 7 someone tried to reset the buffers when a request was outstanding
	ESetReceiveBufferLengthWhilePendingRequests = 8,  //< 8 someone tried to set buffer size when a request was outstanding
	ENotifySignalTwice                          = 9,  //< 9 cannot have two outstanding notify signals from the same client
	ENotifyFlowControlTwice                     = 10, //< 10 cannot have two outstanding notify flow from the same client
	ENotifyConfigTwice                          = 11, //< 11 cannot have two outstanding notify config from the same client
	ENotifyBreakTwice                           = 12, //< 12 cannot have two outstanding notify break from the same client
	ENotifyDataAvailableTwice                   = 13, //< 13 cannot have two outstanding notify data from the same client
	ENotifyOutputTwice                          = 14, //< 14 cannot have two outstanding notify empty from the same client
	EThreadSizeStackNegative                    = 15, //< 15 thread size is negative
	EThreadHeapMinTooSmall                      = 16, //< 16 minimum heap size is too small
	EThreadHeapMaxLessThanMin                   = 17, //< 17 maximum heap size is less than minimum heap size
	EWrongClientForAccessRequest                = 18, //< 18 wrong client for access request
	EMustCloseNotChangeAccessMode               = 19  //< 19 client must close object before doing set access
	};

/**
 * Internal C32 server fault
 *
 * @note Don't change the order of these enums!
 * @note These panics will appear with a category of "C32-fault".
 */
enum TECommFault
	{
	EMainSchedulerError           = 0, //< 0 serious problem with CCommScheduler
	ESvrCreateServer              = 1, //< 1 could not create the CC32Server object
	ESvrStartServer               = 2, //< 2 could not start the Comms server
	ECreateTrapCleanup            = 3, //< 3 no traphandler available
	ENotImplementedYet            = 4, //< 4 not implemented yet (not used)
	ECPortEObjNegativeAccessCount = 5, //< 5 access count should be a positive number
	ECommNullPointer              = 6,  //< 6 illegal NULL pointer
	EDTimerAllocFailure           = 7,  //< 7 failed to create a global timer
	EBadState					  = 8,	//< 8 internal consistency error
	EInitTwice					  = 9,	//< InitC32GlobalsL called twice
	EBadIniData                   = 10, //< Found a problem with the [IniData] section of a cmi file
	EFindSubSessionFailed		  = 11,	//< Failed to find subsession in CC32SubSessionIx
	EMemoryLeakDetected           = 12, //< Dealer detected a memory leak in a player thread
	EDbgHandleLeak                = 13, //< Leaked a handle - should only occur in debug builds
	};

GLDEF_C void Fault(TECommFault aFault, TRefByValue<const TDesC8> aFmt=KNullDesC8(), ...);
GLDEF_C void Fault(TECommFault aFault, TRefByValue<const TDesC16> aFmt, ...);


#endif // CS_PANIC_H

