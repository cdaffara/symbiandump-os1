// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @Internal
*/


#if !defined(__ES_PANIC_INTERNAL_H__)
#define __ES_PANIC_INTERNAL_H__

#include <e32base.h>

/** The panic category for panics caused by internal faults in ESOCK (was "eSock fault" in previous versions)
@internalComponent
*/
_LIT(KESockFaultPanic, "ESock_fault");

/**	The panic numbers for panics caused by clients
@internalComponent
*/

/** The panic numbers for panics caused by internal faults in ESOCK
@internalComponent */
enum TESockFault
	{
	EInitTwice = 0,              //< Twice initialization eSock Fault
	ECommServerNotRunning = 1,   //< Communication server not running eSock Fault
	ECreateTrapCleanup = 2,      //< Create Trap Cleanup eSock Fault
	EMainSchedulerError = 3,     //< Main Scheduler Error eSock Fault
	ESvrCreateServer = 4,        //< Svr Create Server eSock Fault
	ESvrStartServer = 5,         //< Svr Start Server eSock Fault
	ESocketCreate = 6,           //< Socket Create eSock Fault
	ENotImplementedYet = 7,      //< Not Implemented Yet eSock Fault
	EOddSock = 8,                //< Odd Socket eSock Fault
	EBadHostResolver = 9,        //< Bad Host Resolver eSock Fault
	EBadServiceResolver = 10,    //< Bad Service Resolver eSock Fault
	EBadNetDBRequest = 11,       //< Bad NetDB Request eSock Fault
	EBadBlocker = 12,            //< Bad Blocker eSock Fault
	ESocketCloseNoProtocol = 13, //< Socket CloseNo Protocol eSock Fault
	EProtocolCloseNoFamily = 14, //< Protocol Close No Family eSock Fault
	EBadState = 15,              //< Bad State eSock Fault
	ETwoReads = 16,              //< Two Reads eSock Fault
	ETwoWrites = 17,             //< Two Writes eSock Fault
	ETwoConnects = 18,           //< Two Close eSock Fault
	ETwoClose = 19,              //< eSock Fault
	ETwoIoctl = 20,              //< Two Ioctl eSock Fault
	ENoSockManData = 21,         //< No Socket ManData eSock Fault
	EBadAcceptQueue = 22,        //< Bad Accept Queue eSock Fault
	EStateNull = 23,             //< State Null eSock Fault
	EBadSocketRemove = 24,       //< Bad Socket Remove eSock Fault
	EBadMbufCheck = 25,          //< Bad Mbuf Check eSock Fault
	ENegativeSessionCount = 26,  //< Negative Session Count eSock Fault
	EMessageFifoNotDeleted = 27, //< Message Fifo Not Deleted eSock Fault
	EBadConnection = 28,         //< Bad Connection eSock Fault
	EWrongThread = 29,			 //< Code executed from incorrect thread
	EUnrecoverableError = 30,	 //< A usually recoverable error, such as KErrNoMemory, in a state far too rare and expensive to warrant attempting recovery
	ENoPrivatePath = 31,		 //< Could not create private path on C: drive
	EDebugSupport = 32,			 //< Debug support fault
	EDeadObjectReused = 33,		 //< Attempted re-use of an object in the process of being async deleted
	EBufsLeftInSendData = 34     //< Buffers left owned after send pathway completes request
	};

#endif // __ES_PANIC_INTERNAL_H__
