// Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements the active object that controls the Write() requests. 
// 
//

/**
 @file
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "SenderTraces.h"
#endif

#include "Sender.h"
#include "Constants.h"
#include <es_ini.h>


CSender::CSender(CBcaIoController& aObserver, TInt aMaxPacketSise)
/**
 * Constructor. Performs standard active object initialisation.
 * @param aObserver Reference to the observer of this state machine
 */
	: CActive(EPriorityUserInput), 
	  iObserver(aObserver),
	  iMaxPacketSize(aMaxPacketSise)
	{
    // EPriorityUserInput is higher than the default priority but lower than
    // EPriorityHigh which is used on Receiving (DL having priority), however,
    // we want this to be handled in an expedited manner compared to other
    // active objects in the thread.
	CActiveScheduler::Add(this);
	}

CSender* CSender::NewL(CBcaIoController& aObserver, TInt aMaxPacketSise)
/**
 * Two-phase constructor. Creates a new CBcaIoController object, performs 
 * second-phase construction, then returns it.
 *
 * @param aObserver The observer, to which events will be reported
 * @return A newly constructed CBcaIoController object
 */
	{
	CSender* self = new (ELeave) CSender(aObserver, aMaxPacketSise);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CSender::ConstructL()
/**
 * Second-phase constructor. Creates all the state objects it owns.
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSENDER_CONSTRUCTL_1, "CSender::ConstructL");
	iSendBuffer.CreateL(iMaxPacketSize);
	}

CSender::~CSender()
/**
 * Destructor.
 */
    {
    Cancel();
    // iSendBuffer is a shared bit of memory between raw ip and bca
    // you cannot delete it while bca might be using it otherwise
    // bad things may happen.
    iSendBuffer.Close();
    }

void CSender::RunL()
/**
 * This method checks if any error occured in the write operation.  
 */
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSENDER_RUNL_1, "CSender::RunL [iStatus=%d]", iStatus.Int());

	if (iStatus!=KErrNone)
		{
		if(iStatus == KErrNoMemory)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSENDER_RUNL_2, "WARNING! CSender: Write failed with KErrNoMemory");
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSENDER_RUNL_3, "WARNING! CSender: Ignoring packet!!!!");
			// Write operation failed!! Nif will ignore this packet.
			iObserver.SendComplete();
			}
		else if (iStatus == KErrNotReady)
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSENDER_RUNL_4, "WARNING! CSender: Write failed with KErrNotReady");
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSENDER_RUNL_5, "WARNING! CSender: Ignoring packet!!!!");
			// Write operation failed!! Nif will ignore this packet.
			iObserver.SendComplete();
			}
		else
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSENDER_RUNL_6, "ERROR! CSender: Write failed!!!!");
			// Nif will shut down
			iObserver.Stop(iStatus.Int());
			}
		return;
		}

	else
		{
		// The Ip packet was sent successfuly
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSENDER_RUNL_7, "***** CSender: Packet Sent.");
		iObserver.SendComplete();
		}
	}

void CSender::DoCancel()
/**
 *	Cancel active request
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSENDER_DOCANCEL_1, "CSender::DoCancel");

	(iObserver.Bca())->CancelWrite(); 
	}

void CSender::Send(RMBufChain& aPdu)
/**
 * Copies the specified RMBufChain into a descriptor and sends it.
 *
 * @param aPdu The IP packet to be sent.
 * @return KStopSending, or KErrArgument if the packet is too large.
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSENDER_SEND_1, "CSender::Send");

	// Copy the IP portion of the RMBufChain to the buffer.
	iSendBuffer.SetMax();
	aPdu.CopyOut(iSendBuffer, aPdu.First()->Length());

#ifdef RAWIP_HEADER_APPENDED_TO_PACKETS
	iObserver.AddHeader(iSendBuffer);
#endif // RAWIP_HEADER_APPENDED_TO_PACKETS

	aPdu.Free();

	(iObserver.Bca())->Write(iStatus, iSendBuffer);
	SetActive();
	}
