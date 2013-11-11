// Copyright (c) 1999-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "hostmbufpool.h"

#include <bluetooth/hcicommandqueue.h>
#include <bluetooth/hci/hostnumberofcompletedpacketscommand.h>
#include <bluetooth/hci/hcievents.h>
#include <bluetooth/hci/commandcompleteevent.h>
#include <bluetooth/hci/hciutil.h>
#include <bluetooth/hci/hciconsts.h>

#include "linkconsts.h"
#include "linkutil.h"

#include <bluetooth/logger.h>

#ifdef HOSTCONTROLLER_TO_HOST_FLOW_CONTROL

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_LINKMGR);
#endif

__DEBUG_ONLY(PANICCATEGORY("mbufpool");)


CHostMBufPool* CHostMBufPool::NewL(MHCICommandQueue& aCommandQueue)
	{
	LOG_STATIC_FUNC
	CHostMBufPool* self = new (ELeave) CHostMBufPool(aCommandQueue);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CHostMBufPool::DeletePool(TSglQue<TPoolBuffer>& aQueue)
	{
	LOG_FUNC
	TPoolBuffer* buffer = NULL;
	TSglQueIter<TPoolBuffer> iter(aQueue);
	while(buffer=iter++, buffer)
		{
		aQueue.Remove(*buffer);
		DeleteBuffer(buffer);
		}
	}

void CHostMBufPool::DeleteBuffer(TPoolBuffer*& aBuffer)
	{
	LOG_FUNC
	if(aBuffer)
		{
		aBuffer->iMBufChain.Free();
		delete aBuffer;
		aBuffer = NULL;
		}
	}

CHostMBufPool::~CHostMBufPool()
	{
	LOG_FUNC
	Cancel();
	// iMBufRequester is cleaned in it's destructor (not much of an R-class...but it is what it is)
	DeletePool(iBufferPool);
	DeletePool(iWaitingAllocPool);
	DeleteBuffer(iBufferBeingAllocd);
	}
	
CHostMBufPool::CHostMBufPool(MHCICommandQueue& aCommandQueue)
	: CActive(EPriorityHigh) 
	// High priority so that buffers are allocated occur before more data is read, this prevents
	// the cases the data floods the device and exhausts the buffers before any more can be allocated.
	// This maximises throughput since we will ensure we send packet completion notifications in a
	// timely manner.
	, iCmdQ(aCommandQueue)
	, iBufferPool(_FOFF(TPoolBuffer,iLink))
	, iWaitingAllocPool(_FOFF(TPoolBuffer,iLink))
	, iCurrAckHandle(KErrNotFound)
	{
	LOG_FUNC
	}
	
void CHostMBufPool::ConstructL()
/**
2nd phase constructor for the Host MBuf Pool.

This method will attempt to reserve enough MBufs from the global pool
for bluetooth use.
@leave KErrNoMemory If the required number of MBufs couldn't be reserved
*/
	{
	LOG_FUNC
	LOG2(_L("CHostMBufPool: now reserving %d size %d MBufChains"), KStackACLBuffersNum, KLinkMgrIncomingBufferSize);
	
	for (TInt i=0; i<=KStackACLBuffersNum-1; i++)
		{
		TPoolBuffer* thisBuffer = CreatePoolBufferL();
		AddToBufferPool(*thisBuffer);
		}
		
	CActiveScheduler::Add(this);
	}
	
CHostMBufPool::TPoolBuffer* CHostMBufPool::CreatePoolBufferL()
	{
	LOG_FUNC
	TPoolBuffer* newBuffer = new(ELeave) TPoolBuffer();
	CleanupStack::PushL(newBuffer);
	newBuffer->iCurrentHandle = KInvalidConnectionHandle; // we assert on this later
	newBuffer->iMBufChain.AllocL(KLinkMgrIncomingBufferSize);
	CleanupStack::Pop(newBuffer);
	return newBuffer;
	}
	
void CHostMBufPool::DoCancel()
	{
	LOG_FUNC
	iMBufRequester.Cancel();
	}
	
RMBufChain CHostMBufPool::TakeBufferL(THCIConnHandle aConnHandle)
/**
Takes a buffer from the pool and schedules an asynchronous allocation
of the next buffer.	 Only when that allocation has succeeded will the host
controller be signalled with a host_number_of_completed_packets.  Hence,
if we cannot allocate a buffer from the global MBuf pool, the host controller
will be flowed off and no data will be lost.
*/
	{
	LOG_FUNC
	ASSERT_DEBUG(aConnHandle != KInvalidConnectionHandle);
	
	// Speculatively attempt to allocate any queued allocations that may have previously failed.
	TryToAllocQueuedBuffer();
	
	TPoolBuffer* ready = iBufferPool.First();
	
	if(!ready)
		{
		// Whoops run out of buffers - even though we were trying to prevent this with
		// flow control, in the case of disconnection the controller will assume all the
		// data for a connection handle will be flushed and therefore the buffers associated
		// with that connection handle will be free.  Unfortunately for us we don't have
		// that much control with the MBuf pool (since flow control is for asynchronous
		// buffer allocation rather than waiting for the given MBufs to be relinquished
		// by a higher layer).
		// So the controller could think we have more buffers than we actually have...
		LOG(_L8("CHostMBufPool: Ran out of buffers!!!!"));
		LEAVEL(KErrOverflow);
		}
	
	// If here then we should have a valid pool buffer to use
	__ASSERT_DEBUG(!ready->iMBufChain.IsEmpty(), Panic(ELinkMgrHostControllerHasOverflowedHost));
	__ASSERT_DEBUG(ready->iCurrentHandle == KInvalidConnectionHandle, Panic(ELinkMgrHostControllerHasOverflowedHost));
	
	RemoveFromBufferPool(*ready);
	ready->iCurrentHandle = aConnHandle;
	
	RMBufChain retChain;
	retChain.Assign(ready->iMBufChain);
	
	if (IsActive())
		{
		//This buffer will be reclaimed from the global pool
		//after the one(s) we're currently trying to reclaim
		LOG(_L8("CHostMBufPool: TakeBuffer, buffer taken while alloc outstanding: queued alloc"));
		iWaitingAllocPool.AddLast(*ready);
		}
	else
		{
		LOG(_L8("CHostMBufPool: TakeBuffer, buffer taken"));
		AllocNewBuffer(*ready);
		}
		
	return retChain;
	}

void CHostMBufPool::InvalidateByConnH(THCIConnHandle aConnHandle)
	{
	LOG_FUNC
	ASSERT_DEBUG(aConnHandle != KInvalidConnectionHandle);
	
	// We need to scan through the two pending "lists" to remove the
	// connection handle from record so we don't try to provide a 
	// packet completion notification (the controller already assumes
	// the buffers are free as they are entitled to by the spec).
	
	// The current buffer being allocated
	if(iBufferBeingAllocd && iBufferBeingAllocd->iCurrentHandle == aConnHandle)
		{
		iBufferBeingAllocd->iCurrentHandle = KInvalidConnectionHandle;
		}
	
	// The list of buffers waiting to be allocted
	TPoolBuffer* buffer = NULL;
	TSglQueIter<TPoolBuffer> iter(iWaitingAllocPool);
	while(buffer=iter++, buffer)
		{
		if(buffer->iCurrentHandle == aConnHandle)
			{
			buffer->iCurrentHandle = KInvalidConnectionHandle;
			}
		}
	
	// Finally we need to purge any batched up completions if they
	// are for this connection handle
	if(iCurrAckHandle == aConnHandle)
		{
		iCurrAckHandle = KErrNotFound;
		iCurrCompletedPackets = 0;
		}
	}

void CHostMBufPool::RunL()
	{
	LOG_FUNC
	LEAVEIFERRORL(iStatus.Int());
	
	// We've successfully allocated a new MBufChain
	TPoolBuffer* justAllocd = iBufferBeingAllocd;
	iBufferBeingAllocd = NULL;
	THCIConnHandle justAllocdHandle = justAllocd->iCurrentHandle;
	
	// Return buffer to pool for re-use
	AddToBufferPool(*justAllocd);
	justAllocd->iCurrentHandle = KInvalidConnectionHandle;
	
	// If connection handle is still valid then we need to provide a completion
	// notification for the packet to the connection handle it was from.
	if(justAllocdHandle != KInvalidConnectionHandle)
		{
		if (iCurrAckHandle == KErrNotFound)
			{
			// This is the first completion we have seen
			iCurrAckHandle = justAllocdHandle;
			}
		ASSERT_DEBUG(iCurrAckHandle != KInvalidConnectionHandle); // just to be sure
		
		TBool ackNow = (justAllocdHandle != iCurrAckHandle);
		
		if (!ackNow)
			{
			iCurrCompletedPackets++;
			LOG2(_L8("CHostMBufPool: CompletedPackets++ for conn: %d [->%d]"), justAllocdHandle, iCurrCompletedPackets);
			ackNow = (iCurrCompletedPackets >= KStackACLBuffersTideMarkNum);
			}
			
		if (ackNow)
			{
			TInt err = KErrNone;
			
			if (iCurrCompletedPackets > 0)
				{
				LOG2(_L8("CHostMBufPool: Sending HostNumberOfCompletedPackets for conn: %d [%d completed]"), iCurrAckHandle, iCurrCompletedPackets);
				//Acknowledge the completed packets
				TRAP(err, HostNumberOfCompletedPacketsL(iCurrAckHandle, iCurrCompletedPackets));
				//if this failed we probably couldn't alloc the memory for the command frame,
				//the HC is still flowed off.
				__ASSERT_DEBUG(err == KErrNone, Panic(ELinkMgrCouldNotSendHostNumberOfCompletedPackets));
				LEAVEIFERRORL(err);
				}
			
			iCurrCompletedPackets = (justAllocdHandle != iCurrAckHandle) ? 1 : 0;
			iCurrAckHandle = justAllocdHandle;
			}
		}
	
	TryToAllocQueuedBuffer();
	}
	
void CHostMBufPool::TryToAllocQueuedBuffer()
	{
	LOG_FUNC
	if (!iWaitingAllocPool.IsEmpty() && !IsActive())
		{
		TPoolBuffer* needsAlloc = iWaitingAllocPool.First();
		iWaitingAllocPool.Remove(*needsAlloc);
		AllocNewBuffer(*needsAlloc);
		}
	}
	
void CHostMBufPool::AllocNewBuffer(TPoolBuffer& aBuffer)
	{
	LOG_FUNC
	ASSERT_DEBUG(!iBufferBeingAllocd);
	iBufferBeingAllocd = &aBuffer;
	iMBufRequester.Alloc(aBuffer.iMBufChain, KLinkMgrIncomingBufferSize, iStatus);
	SetActive();
	}
	
void CHostMBufPool::HostNumberOfCompletedPacketsL(THCIConnHandle aConnH, TUint16 aNumPackets)
	{
	RArray<THCIConnectionHandle> connHandles;
	connHandles.AppendL(aConnH);
	CleanupClosePushL(connHandles);

	RArray<THCINumOfCompletedPackets> numPackets;
	numPackets.AppendL(aNumPackets);
	CleanupClosePushL(numPackets);
	
	CHostNumberOfCompletedPacketsCommand* cmd = CHostNumberOfCompletedPacketsCommand::NewL(1, connHandles, numPackets);
	// Ownership of the arrays is taken by the command object.
	CleanupStack::Pop(2, &connHandles); // &numPackets, &connHandles

	// This is a priority command as we want to try to get this out as soon as possible (and not wait
	// for all normal control aspects to be processed).  This command shouldn't normally consume any credits
	// so as a priority command it has little impact.
	// Ownership of cmd transfered even if MhcqAddPriorityCommandL leaves
	iCmdQ.MhcqAddPriorityCommandL(cmd, *this);
	}
	
void CHostMBufPool::MhcqcCommandEventReceived(const THCIEventBase& aEvent, const CHCICommandBase* /*aRelatedCommand*/)
	{
	LOG_FUNC
	// We don't expect a non-error event back because we're only sending Host_Number_of_Completed_Packet commands
	if(aEvent.EventCode() == ECommandCompleteEvent)
		{
		const THCICommandCompleteEvent& completeEvent = THCICommandCompleteEvent::Cast(aEvent);
		if(completeEvent.CommandOpcode() == KHostNumberOfCompletedPacketsOpcode)
			{
			// a regular error for a Host_Number_Of_Completed_Packets command
			TInt err = CHciUtil::SymbianErrorCode(completeEvent.ErrorCode());
			if(err != KErrNone) // we shouldn't get a non-erroring event back, but just in case
				{
				Error(err);
				}
			}
		else // an unexpected command complete event
			{
			LOG1(_L8("CHostMBufPool: Unexpected HCI command complete event; opcode = 0x%04x"), completeEvent.CommandOpcode());
			DEBUG_PANIC_LINENUM;
			}
		}
	else // some unexpected event
		{
		LOG1(_L8("CHostMBufPool: Unexpected HCI event received; code = 0x%02x"), aEvent.EventCode());
		DEBUG_PANIC_LINENUM;
		}
	}

void CHostMBufPool::MhcqcCommandErrored(TInt aErrorCode, const CHCICommandBase* /*aCommand*/)
	{
	LOG_FUNC
	Error(aErrorCode);
	}
	
TInt CHostMBufPool::RunError(TInt aError)
	{
	LOG_FUNC
	if(iBufferBeingAllocd)
		{
		TPoolBuffer* justFailedToAlloc = iBufferBeingAllocd;
		iBufferBeingAllocd = NULL;
		// Add to wait for alloc queue - we may get another chance
		iWaitingAllocPool.AddFirst(*justFailedToAlloc);
		}
	Error(aError);
	return KErrNone;
	}
	
void CHostMBufPool::Error(TInt IF_FLOGGING(aError))
	{
	LOG_FUNC
	// So there has been some internal error when handling controller to host
	// flow control.  Tough, we've done our best for now - the only real thing
	// that might be worth doing is a hard reset to start-up clean.
	LOG1(_L8("CHostMBufPool: ERROR (%d) - inbound data to the stack may stall soon"), aError);
	}
	
#endif
