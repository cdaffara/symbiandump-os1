// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#include "hctlbcspreceiver.h"

#include "hctlbcsp.h"
#include "hctlbcspconsts.h"
#include "debug.h"


/**
Implementation of Class CHCTLBcspReceiver
*/
CHCTLBcspReceiver::CHCTLBcspReceiver(CHCTLBcsp& aHCTLBcsp, RBusDevComm& aPort) :
	CActive(EPriorityStandard), 
	iHctlBcsp(aHCTLBcsp),
	iReceiveBufPtr(NULL,0),
	iPort(aPort)
    {
	LOG_FUNC
	CActiveScheduler::Add(this);
    }

CHCTLBcspReceiver::~CHCTLBcspReceiver()
    {
	LOG_FUNC

	Cancel();
    iReceiveBuffer.Close();
    }

CHCTLBcspReceiver* CHCTLBcspReceiver::NewL(CHCTLBcsp& aHCTLBcsp, RBusDevComm& aPort) 
    {
	LOG_STATIC_FUNC

    CHCTLBcspReceiver* self=new(ELeave)CHCTLBcspReceiver(aHCTLBcsp, aPort);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

void CHCTLBcspReceiver::ProcessData()	
/**
	This method parses the byte stream for a pair of 0xC0 bytes denoting beginning and end of packets
	Then calls the HandleRx
	Then zeros the pointer to the receive buffer and queues another read for the next frame
*/
    {
	LOG_FUNC

#ifdef __DEBUG_FLOG_RAW_UART_	
	LOG(_L8("HCTLBCSP: UART RX..."));
	TPtrC8 ptr = iReceiveBufPtr.Ptr();
	TInt len = iReceiveBufPtr.Length();
	LOGHEXDESC(iReceiveBufPtr);
#endif
	
	if(iReceiveBufPtr.Length() > 1) // Processing the remainder of the SLIP encoded Frame
		{
		if(iHctlBcsp.HandleRx(iReceiveBufPtr) != KErrNone)	// SLIP Decode Frame
			{
			LOG(_L8("HCTLBCSP: SLIP Decoding failed discard packet requeue read..."))
			}
		}

	// Reset Buffers
	iReceiveBufPtr.Zero();
	QueueReadForNextFrame(); // Get more stuff until next slip byte
	}

void CHCTLBcspReceiver::QueueReadForNextFrame()
/**
	Method moved due to HCI refactoring to reduce code duplication
	This method should suffice for all RBusDevComm reads

	@param aBytesRequired
*/
	{
	LOG_FUNC

	if (IsActive())
        {
		LOG(_L8("Receiver already active - TRYING TO CANCEL AND REQUEUE"));
 		Cancel(); // consume signal then do re-queue
        }
 
    SetActive();

	iPort.Read(iStatus, iReceiveBufPtr);
	}

void CHCTLBcspReceiver::RunL()
/**
	If no error occurred, call ProcessData(). Otherwise, throw any data away 
	and requeue the Read. 	
*/
	{
	LOG_LINE
	LOG_FUNC
	LOG1(_L8("\tiStatus = %d"), iStatus.Int());

	switch(iStatus.Int())
		{
		case KErrNone:
			// Receiver has a framer state machine
			ProcessData();
			break;
		
		case KErrAbort:
		case KErrBadPower:
			iReceiveBufPtr.Zero();
			// Pretend nothing happened and requeue the read.
			// Indeed the driver gives us this notification, not on power-down
			// but on the next power-up, so we can requeue.
			QueueReadForNextFrame();
			break;

#ifdef __DEBUG_INIT_PROGRESS
		case KErrCommsOverrun:
		case KErrCommsFrame:
		case KErrCommsParity:
			break;

		default:
			PANIC(KHCTLBcspPanicCat, EHCIHCTLReceiverFramerBadState);
			break;
#else
		default:
			// Ignore error condition.
			iReceiveBufPtr.Zero();
			QueueReadForNextFrame();
			break;
#endif
		};
	}

void CHCTLBcspReceiver::ConstructL()
    {
	LOG_FUNC
	
	//Create a HBufC based RBuf Object. 
    HBufC8* buf = HBufC8::NewMaxL(KHCTLRecvBufSize);
    iReceiveBuffer.Assign(buf);
    iReceiveBufPtr.Set(buf->Des());
    }

void CHCTLBcspReceiver::DoCancel()
	{
	LOG_FUNC
	
	iPort.ReadCancel();
	}

TInt CHCTLBcspReceiver::RunError(TInt aError)
	{
	LOG_LINE
	LOG_FUNC
	LOG1(_L8("\taError = %d"), aError);
	static_cast<void>(aError); // prevent unused variable warning
	return KErrNone;
	}
