// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file implements a loopback driver for use with 3GNIF test harness.
// 
//

/**
 @file
*/

#include <c32comm.h>
#include <c32comm_internal.h>

#include "Loopback.h"
#include "LoopbackConfig.h"
#include "LoopbackTimer.h"
#include "LoopbackQueue.h"

CHWPort::CHWPort(TUint aUnit) : iRole(ECommRoleDTE), iPortName(aUnit)
	{
	TName name;
	name.Format(_L("%d"),aUnit);
	SetName(&name);
	}

static void CloseObject(TAny* anObject)
/**
 * This method simply closes an object from the cleanup stack.  The object must contain
 * a Close method.
 *
 * @param	anObject - a TAny pointer to the object to close. 
 */
	{
	((CObject*)anObject)->Close();
	}

CHWPort* CHWPort::NewPacketLoopbackL(TUint aUnit, TUint aDelay, TUint aPacketLength, TUint aQueueLength)
/**
 * This method is used by the factory object to create the new CHWPort instances.
 * After newing the CHWPort object, the buffer is created, roles and signals are defaulted,
 * and names are initialized.
 *
 * @param	aUnit - the port number to create.
 * @param	aDelay - the delay for sending on the port
 * @param	aPacketLength - the size in bytes of a packet
 * @param	aQueueLength - the maximum number of packets to queue
 *
 * @return	A pointer to the created object
 */
	{
	LOGTEXT2(_L8("PKTLOOPBACK:NewL: Called for Unit %d"), aUnit);

	CHWPort *p = new(ELeave) CHWPort(aUnit);
	TCleanupItem closePort(CloseObject,p);
	CleanupStack::PushL(closePort);
	p->ConstructPacketLoopbackL(aDelay, aPacketLength, aQueueLength);
	CleanupStack::Pop(p);

	return p;
	}

CHWPort* CHWPort::NewSerialLoopbackL(TUint aUnit, TUint aDelay, TUint aBufferSize)
/**
 * This method is used by the factory object to create the new CHWPort instances.
 * After newing the CHWPort object, the buffer is created, roles and signals are defaulted,
 * and names are initialized.
 *
 * @param	aUnit - the port number to create.
 * @param	aDelay - the delay for sending on the port
 * @param	aBuffersize - the size of the buffer to allocate
 *
 * @return	A pointer to the created object
 */
	{
	LOGTEXT2(_L8("PKTLOOPBACK:NewL: Called for Unit %d"), aUnit);

	CHWPort *p = new(ELeave) CHWPort(aUnit);
	TCleanupItem closePort(CloseObject,p);
	CleanupStack::PushL(closePort);
	p->ConstructSerialLoopbackL(aDelay, aBufferSize);
	CleanupStack::Pop(p);

	return p;
	}

void CHWPort::ConstructSerialLoopbackL(TUint aDelay, TUint aBufferSize)
	{
	iWriteDelayTimer = CLoopbackTimer::NewL(aDelay, this);
	iFlowControlChange = CFlowControlChange::NewL(iPortName, this);
	
	iPortType = ESerialLoopbackPortType;
	TCommConfigV01 *config = &iConfig();
	iReadWriteQueue = CSerialBufferQueue::NewL(aBufferSize, config);
	}
	
void CHWPort::ConstructPacketLoopbackL(TUint aDelay, TUint aPacketLength, TUint aQueueLength)
	{
	iWriteDelayTimer = CLoopbackTimer::NewL(aDelay, this);
	iFlowControlChange = CFlowControlChange::NewL(iPortName, this);
	iReadWriteQueue = CPacketBufferQueue::NewL(aPacketLength, aQueueLength);
	}
	
TUint CHWPort::PortName() const
	{
	return iPortName;
	}

void CHWPort::SetLoopbackPort(CHWPort* aHWPort)
/**
 * This method sets up the loopback port member of the CHWPort.  It is used after
 * both ports have been created (NewL).  This allows each port to "know" to whom he is
 * connected.
 *
 * @param	aHWPort - the port which THIS port should be connected to.
 */
	{
	LOGTEXT2(_L8("PKTLOOPBACK:SetLoopbackPort:  Unit %d..."), iPortName);

	// Now set up the loopback
	iLoopbackPort = aHWPort;
	}

TBool CHWPort::GetFlowControl() const
/**
 * @return ETrue if flow control is on, EFalse if it is not
 */
	{
	return iFlowControlChange->FlowControlIsOn();
	}

void CHWPort::StartFlowControl()
/**
 * Performs any necessary actions to set flow control on
 */
	{
	iWriteDelayTimer->Cancel();
	}
	
void CHWPort::StopFlowControl()
/**
 * Performs any necessary actions to stop flow control
 */
	{
	if (!iLoopbackPort)
		{
		return;
		}
	
	if (iReadWriteQueue->IsWriteBufferEmpty())
		{
		return;
		}
	
	TInt nextWriteLen = iReadWriteQueue->PeekNextWriteBuffer().Length();
	if (!iLoopbackPort->iReadWriteQueue->IsReadBufferFull(nextWriteLen)
		&& !iWriteDelayTimer->IsActive())
		{
		iWriteDelayTimer->Start();
		}
	}
	
TInt CHWPort::GetReadRequestStatus()
/**
 * @return The status that Read() calls are to complete with
 */
	{
	TInt readRequestStatus = KErrNone;
	TInt ret = RProperty::Get(KUidPSCsyReadResultCategory, iPortName, readRequestStatus);
	if (ret == KErrNone)
		return readRequestStatus;
	// if we can't find this property, we make everything work
	return KErrNone;
	}
	
TInt CHWPort::GetWriteRequestStatus()
/**
 * @return The status that Write() calls are to complete with
 */
	{
	TInt writeRequestStatus = KErrNone;
	TInt ret = RProperty::Get(KUidPSCsyWriteResultCategory, iPortName, writeRequestStatus);
	if (ret == KErrNone)
		return writeRequestStatus;
	// if we can't find this property, we make everything work
	return KErrNone;
	}
	
void CHWPort::StartRead(const TAny* aClientBuffer, TInt aLength)
/**
 * This method queues a read operation to the driver.  If the read length is zero (which is a
 * special case used during initialization) the read completes immediately.
 * If the read buffer is insufficient in size, the read completes with error KErrOverflow.
 *
 * @param	aClientBuffer	- a TAny * to the buffer into which data is placed.
 * @param	aLength			- the length of the buffer supplied
 */
	{
	LOGTEXT2(_L8("PKTLOOPBACK:StartRead:  Unit %d..."), iPortName);

	// Because a length of zero is a special case (for initialization), we will complete this immediately
	if(aLength == 0)
		{
		ReadCompleted(KErrNone);
		return;
		}
	else if(aLength < 0)
		{
		// if the read is through the function ReadOneOrMore, the length will be negative
		// in this CSY, the behavior is always ReadOneOrMore
		aLength = -aLength;
		iReadOneOrMore = ETrue;
		}
	else // aLength > 0
		{
		iReadOneOrMore = EFalse;
		}
	
	// Save the client read buffer
	iClientReadBuffer = aClientBuffer;
	iClientReadBufferLength = aLength;
	iReadPending = ETrue;
	
	// if there is already an entry in the read buffer, we can complete the read now
	TryToCompleteRead();
	}

void CHWPort::TryToCompleteRead()
/**
 * This method attempts to complete reads - it is either called as they are initially issued (and is
 * completed if there is data already in the read buffer), or is is called later, when data is supplied 
 * to the read buffer (and is completed if there is a read waiting)
 * Data is moved from the buffer associated with this port to a buffer that was supplied by the client 
 * when the read was issued.
 */

	{
	LOGTEXT2(_L8("PKTLOOPBACK:TryToCompleteRead: Unit %d ..."), iPortName);

	// If there is not read pending or no loopback port, we can't complete a read
	if (!iReadPending)
		return;
	
	if (!iLoopbackPort)
		return;
	
	// Determine if there an entry available to be read
	if (iReadWriteQueue->IsReadBufferEmpty())
		return;
	
	TInt res = KErrNone;
	TBool completeIfBufferNotFull;
	TPtrC8 nextReadBuffer = iReadWriteQueue->PeekNextReadBuffer(completeIfBufferNotFull);
	if ( iClientReadBufferLength > nextReadBuffer.Length() )
		{
		if (iPortType == ESerialLoopbackPortType) 
			{
		 	if (iReadOneOrMore == EFalse) 
				{
		 		if (!completeIfBufferNotFull)
		 			{
					return;
		 			}
				}
			}
		}
		
	TInt readRequestStatus = GetReadRequestStatus();
	if ( (iClientReadBufferLength < nextReadBuffer.Length()) && (iPortType == EPacketLoopbackPortType) )
		{
		// The client buffer is not large enough
		res = KErrOverflow;
		}
	else if (readRequestStatus != KErrNone)
		{
		// We are configured to complete with an error
		res = readRequestStatus;
		iReadWriteQueue->DiscardNextReadBuffer();
		}
	else
		{
		// Write the data to the client's supplied buffer
		res = IPCWrite(iClientReadBuffer, nextReadBuffer, 0);
		if(res == KErrNone)
			{
			// It is normally the writing buffer's responsibility to start the timer, but in the case where the 
			// reading port's buffer is full, now that the read buffer is no longer full, we have to signal to the 
			// writing port to start sending again
			if (!iLoopbackPort->iReadWriteQueue->IsWriteBufferEmpty())
				{
				TInt nextWriteLen = iLoopbackPort->iReadWriteQueue->PeekNextWriteBuffer().Length();
				if (iReadWriteQueue->IsReadBufferFull(nextWriteLen)
					&& !iLoopbackPort->iWriteDelayTimer->IsActive() )
					{
					iLoopbackPort->iWriteDelayTimer->Start();
					}
				}
			// Read has been completed successfully, so dequeue
			iReadWriteQueue->DiscardNextReadBuffer();
			}
		}
	// complete the read
	iReadPending = EFalse;
	LOGTEXT3(_L8("PKTLOOPBACK:TryToCompleteRead: Unit %d completing read with error %d"), iPortName, res);
	ReadCompleted(res);
	}

void CHWPort::ReadCancel()
/**
 * Cancel a pending read and complete it with KErrCancel. The handling of the CActive class
 * will by default complete any outstanding read with KErrCancel, but it is cleaner to handle
 * that processing here.
 */
	{
	LOGTEXT2(_L8("PKTLOOPBACK:ReadCancel:  Unit %d..."), iPortName);

	iReadPending = EFalse;

	ReadCompleted(KErrCancel);
	}

TInt CHWPort::WriteBuf(const TAny* aClientBuffer, TInt aLength)
/**
 * This method queues the client buffer to the loopback port.
 * A delay in the port is simulated, so the read corresponding to this buffer will not complete
 * until after this delay. The buffers are queued up and sent one per delay period.
 *
 * @param	aClientBuffer	- a TAny * to the buffer which contains the data to write
 * @param	aLength			- the length of the buffer.
 *
 * @return	KErrNone if everything is okay, KErrOverflow if the write queue is full and no more entries can be written until some have been sent, 
 * KErrNotReady	if the loopback port has not yet been opened or there is no loopback port configured for this port, or KErrArgument	if the supplied buffer is too large
 */
	{
	TInt res = KErrNone;

	LOGTEXT2(_L8("PKTLOOPBACK:WriteBuf:  Unit %d..."), iPortName);

	TInt writeRequestStatus = GetWriteRequestStatus();
	if (writeRequestStatus != KErrNone)
		return writeRequestStatus;
	
	// Fill the receiving buffer
	if (aLength != 0)
		{
		if (iReadWriteQueue->IsWriteBufferFull(aLength))
			{
			return KErrOverflow;
			}
		if (aLength > iReadWriteQueue->BufferSize())
			{
			return KErrArgument;
			}
		
		TPtr8 currentPacket = iReadWriteQueue->AppendToWriteBuffer(aLength);
		
		res = IPCRead(aClientBuffer, currentPacket);
		if(res != KErrNone)
			return res;
		}
	// Queue up the request and start the timer to simulate the delay in sending the data across the port
	if (!iWriteDelayTimer->IsActive())
		{
		iWriteDelayTimer->Start();
		}

	return KErrNone;
	}

void CHWPort::TimerCallBack()
/**
 * This method is called after the simulated delay for sending data across the port
 */
	{
	if (!iLoopbackPort)
		{
		return;
		}
	
	if (iReadWriteQueue->IsWriteBufferEmpty())
		{
		return;
		}
	
	TPtrC8 writeBuffer = iReadWriteQueue->PeekNextWriteBuffer();
	if (iLoopbackPort->iReadWriteQueue->IsReadBufferFull(writeBuffer.Length()))
		{
		return;
		}
	
	// Move the buffer from the write queue to the read queue
	iLoopbackPort->iReadWriteQueue->AppendToReadBuffer(writeBuffer);
	iReadWriteQueue->DiscardNextWriteBuffer();

	// If the loopback port has a read outstanding it can complete now
	iLoopbackPort->TryToCompleteRead();
	// If there is another write outstanding we have to simulate a delay for that entry
	if (!iReadWriteQueue->IsWriteBufferEmpty() && !iWriteDelayTimer->IsActive())
		{
		iWriteDelayTimer->Start();
		}
	}

TInt CHWPort::QueryReceiveBuffer(TInt& aLength) const
/**
 * This method returns the length of the buffer associated with this instance of the
 * port.
 *
 * @param	aLength			- a reference to return the length of the buffer.
 */
	{
	LOGTEXT2(_L8("PKTLOOPBACK:QueryReceiveBuffer:  Unit %d..."), iPortName);

	TBool dummy;
	const TPtrC8& iReadBuf = iReadWriteQueue->PeekNextReadBuffer(dummy);
	aLength = iReadBuf.Length();
	return KErrNone;
	}

void CHWPort::ResetBuffers(TUint)
/**
 * This method resets the buffer used by this loopback port 
 *
 * @note Note that most ResetBuffers methods derived from CPort allow a parameter for flags.
 * This ResetBuffers method does not.
 *
 * @param	Not Used
 */
	{
	LOGTEXT2(_L8("PKTLOOPBACK:ResetBuffers:  Unit %d..."), iPortName);
	
	iReadWriteQueue->Clear();
	}

void CHWPort::StartWrite(const TAny* aClientBuffer,TInt aLength)
/**
 * This method queues a write operation to the driver.
 *
 * @param	aClientBuffer	- a TAny * to the buffer into which data should be read.
 * @param	aLength			- the length of the data to be written.
 */
	{

	LOGTEXT2(_L8("PKTLOOPBACK:StartWrite:  Unit %d..."), iPortName);

	TInt res = WriteBuf(aClientBuffer, aLength);
	
	LOGTEXT3(_L8("PKTLOOPBACK:StartWrite:  Completing Write for Unit %d with error %d"), iPortName, res);
	WriteCompleted(res);
	}

void CHWPort::WriteCancel()
/**
 * This method cancels a pending write and issues a WriteCompleted with the result
 * KErrCancel.
 */

	{
	LOGTEXT2(_L8("PKTLOOPBACK:WriteCancel:  Unit %d..."), iPortName);

	WriteCompleted(KErrCancel);
	}

void CHWPort::Break(TInt /*aTime*/)
/**
 * This method is currently not implemented in the loopback driver as breaks are
 * not supported.
 */
	{
	LOGTEXT2(_L8("PKTLOOPBACK:Break is not supported:  Unit %d..."), iPortName);
	}

void CHWPort::BreakCancel()
/**
 * This method is currently not implemented in the loopback driver as breaks are
 * not supported.
 */
	{
	LOGTEXT2(_L8("PKTLOOPBACK:BreakCancel is not supported:  Unit %d..."), iPortName);
	}

TInt CHWPort::GetConfig(TDes8& aDes) const
/**
 * This gets the current configuration from the loopback driver.
 * 
 * @return KErrNone if the size of the output descriptor is valid
 */
	{
	LOGTEXT2(_L8("PKTLOOPBACK:GetConfig: Unit %d..."), iPortName);

	TInt length = aDes.Length();
	__ASSERT_DEBUG( ((length==sizeof(TCommConfigV01)) ||
	                 (length==sizeof(TCommConfigV02)) ),
	                 User::Panic(KPortLoopBackCsyPanic, EPLBArgument));

	if (length == sizeof(TCommConfigV01))
	    {
            aDes.Copy(iConfig);
            return KErrNone;
	    }
        
	return KErrNotSupported;
	}

TInt CHWPort::SetConfig(const TDesC8& aDes)
/**
 * This sets the current configuration for the loopback driver.  Note that
 * no error checking is done when setting the configuration.
 *
 * @return KErrNone if the size of the output descriptor is valid
 */
	{
    
	LOGTEXT2(_L8("PKTLOOPBACK:SetConfig:  Unit %d..."), iPortName);

	TInt length = aDes.Length();
	
	__ASSERT_DEBUG( ((length==sizeof(TCommConfigV01)) ||
	                  (length==sizeof(TCommConfigV02))),
	                  User::Panic(KPortLoopBackCsyPanic, EPLBArgument));

	if (length==sizeof(TCommConfigV01))
	    {
            iConfig.Copy(aDes);
	    }
	else if (length==sizeof(TCommConfigV02))
	    {
            TBuf8<sizeof(TCommConfigV02)> buff(aDes);
            buff.SetLength(sizeof(TCommConfigV01));        
            iConfig.Copy(buff);	
	    }

	return KErrNone;
	}

TInt CHWPort::GetCaps(TDes8& aDes)
/**
 * This gets the supported capabilities from the loopback driver.  The actual capabilities of
 * the driver will vary based on the role the port is playing (DCE or DTE).  The loopback driver
 * supports capabilities via TCommCapsV01 and TCommCapsV02.
 *
 * @param	aDes				- a TDes8 reference to copy the capabilities into.
 *
 * @return	KErrNone			- Everything is okay, KErrNotSupported it the length of the descriptor passed to this method indicates a
 *								  capabilities structure which we don't support.
 */
	{
	LOGTEXT2(_L8("PKTLOOPBACK:GetCaps:  Unit %d..."), iPortName);

	if(aDes.Length()==sizeof(TCommCapsV01))
		{
		TCommCapsV01* commcaps=(TCommCapsV01*)(aDes.Ptr());
		// We've got all of these
		commcaps->iRate=0x3fffff;
		commcaps->iDataBits=0xf;
		commcaps->iStopBits=0x3;
		commcaps->iParity=0x1f;
		commcaps->iFifo=0x1;
		commcaps->iHandshake=0;
		commcaps->iSignals=0x3f;
		commcaps->iSIR=0x0;
		return KErrNone;
		}
	else if(aDes.Length()==sizeof(TCommCapsV02))
		{
		TCommCapsV02* commcaps=(TCommCapsV02*)(aDes.Ptr());
		commcaps->iRate=0x3fffff;
		commcaps->iDataBits=0xf;
		commcaps->iStopBits=0x3;
		commcaps->iParity=0x1f;
		commcaps->iFifo=0x1;
		commcaps->iHandshake=0;
		commcaps->iSignals=0x3f;
		commcaps->iSIR=0x0;
		commcaps->iNotificationCaps=0;
		commcaps->iRoleCaps=0x0;
		return KErrNone;
		}
	else
		return KErrNotSupported;
	}

TInt CHWPort::SetServerConfig(const TDesC8& /*aDes*/)
/**
 * This sets the current server configuration for the loopback driver.  The loopback driver
 * stores this information but does nothing with it.
 *
 * @param	aDes	- a TDes8 reference to copy the configuration from.
 *
 * @return	KErrNotSupported
 */
	{
	LOGTEXT2(_L8("PKTLOOPBACK:SetServerConfig is not supported:  Unit %d..."), iPortName);

	return KErrNotSupported;
	}

TInt CHWPort::GetServerConfig(TDes8& /*aDes*/)
/**
 * This gets the current server configuration for the loopback driver.  The loopback driver
 * stores this information but does nothing with it other than return it here.
 *
 * @param	aDes	- a TDes8 reference to copy the configuration to.
 *
 * @return	KErrNotSupported
 */
	{
	LOGTEXT2(_L8("PKTLOOPBACK:GetServerConfig is not supported:  Unit %d..."), iPortName);

	return KErrNotSupported;
	}

TInt CHWPort::GetSignals(TUint& aSignals)
/**
 * This method retrieves the current setting of the signals for THIS port.
 *
 * @param	aSignals	- A reference to a TUint to return the signal settings.
 *
 * @return	KErrNotSupported
 */
	{
	LOGTEXT2(_L8("PKTLOOPBACK:GetSignals is not supported:  Unit %d..."), iPortName);

	aSignals = 0;
	return KErrNotSupported;
	}

TInt CHWPort::SetSignalsToMark(TUint /*aSignals*/)
/**
 * This method asserts the signals specified by the parameter aSignals.
 *
 * @param	aSignals	- a bitmask specifying which signals to assert (See definition
 *                        of KSignalDCD, KSignalCTS, etc. for bit values).
 *
 * @return	KErrNotSupported
 */
	{
	LOGTEXT2(_L8("PKTLOOPBACK:SetSignalsToMark is not supported:  Unit %d..."), iPortName);

	return KErrNotSupported;
	}



TInt CHWPort::SetSignalsToSpace(TUint /*aSignals*/)
/**
 * This method de-asserts the signals specified by the parameter aSignals.
 *
 * @return KErrNotSupported
 */
	{
	LOGTEXT2(_L8("PKTLOOPBACK:SetSignalsToSpace is not supported:  Unit %d..."), iPortName);

	return KErrNotSupported;
	}

TInt CHWPort::GetReceiveBufferLength(TInt& /*aLength*/) const
/**
 * This method is currently not implemented in the loopback driver.  Calling this
 * method will return an error
 *
 * @return	KErrNotSupported
 */
	{
	LOGTEXT2(_L8("PKTLOOPBACK:GetReceiveBufferLength is not supported:  Unit %d..."), iPortName);

	return KErrNotSupported;
	}

TInt CHWPort::SetReceiveBufferLength(TInt /*aLength*/)
/**
 * This method is currently not implemented in the loopback driver.  Calling this
 * method will return an error
 *
 * @return	KErrNotSupported
 */
	{
	LOGTEXT2(_L8("PKTLOOPBACK:SetReceiveBufferLength is not supported:  Unit %d..."), iPortName);

	return KErrNotSupported;
	}

void CHWPort::Destruct()
/**
 * This method is simply deletes this instance of the port, comitting sucide.
 */
	{
	delete this;
	}



void CHWPort::NotifySignalChange(TUint /*aSignalMask*/)
/**
 * This method sets up a request to be notified when a signal change occurs on the specified
 * signals.  Later operations will send a message to the requestor indicating the signal
 * change.
 *
 * @param	aSignalMask		-	the signals that the caller is interested in monitoring.
 */
	{
	LOGTEXT2(_L8("PKTLOOPBACK:NotifySignalChange is not supported:  Unit %d..."), iPortName);
	}


void CHWPort::NotifySignalChangeCancel()
/**
 * This method cancels an outstanding request to be notified when a signal change occurs.  Any
 * outstanding signal change request will be completed with KErrCancel.
 */
	{
	LOGTEXT2(_L8("PKTLOOPBACK:NotifySignalChangeCancel is not supported:  Unit %d..."), iPortName);
	}


void CHWPort::NotifyConfigChange()
/**
 * This method is currently not implemented in the loopback driver.
 */
	{
	LOGTEXT2(_L8("PKTLOOPBACK:NotifyConfigChange is not supported:  Unit %d..."), iPortName);
	}

void CHWPort::NotifyConfigChangeCancel()
/**
 * This method is currently not implemented in the loopback driver.
 */
	{
	LOGTEXT2(_L8("PKTLOOPBACK:NotifyConfigChangeCancel is not supported:  Unit %d..."), iPortName);
	}

void CHWPort::NotifyFlowControlChange()
/**
 * This method is currently not implemented in the loopback driver.
 */
	{
	LOGTEXT2(_L8("PKTLOOPBACK:NotifyFlowControlChange is not supported:  Unit %d..."), iPortName);
	}

void CHWPort::NotifyFlowControlChangeCancel()
/**
 * This method is currently not implemented in the loopback driver.
 */
	{
	LOGTEXT2(_L8("PKTLOOPBACK:NotifyFlowControlChangeCancel is not supported:  Unit %d..."), iPortName);
	}


void CHWPort::NotifyBreak()
/**
 * This method is currently not implemented in the loopback driver.
 */
	{
	LOGTEXT2(_L8("PKTLOOPBACK:NotifyBreak is not supported:  Unit %d..."), iPortName);
	}

void CHWPort::NotifyBreakCancel()
/**
 * This method is currently not implemented in the loopback driver.
 */
	{
	LOGTEXT2(_L8("PKTLOOPBACK:NotifyBreakCancel is not supported:  Unit %d..."), iPortName);
	}

void CHWPort::NotifyDataAvailable()
/**
 * This method is currently not implemented in the loopback driver.
 */
	{
	LOGTEXT2(_L8("PKTLOOPBACK:NotifyDataAvailable is not supported:  Unit %d..."), iPortName);
	}

void CHWPort::NotifyDataAvailableCancel()
/**
 * This method is currently not implemented in the loopback driver.
 */
	{
	LOGTEXT2(_L8("PKTLOOPBACK:NotifyDataAvailableCancel is not supported:  Unit %d..."), iPortName);
	}

void CHWPort::NotifyOutputEmpty()
/**
 * This method is currently not implemented in the loopback driver.
 */
	{
	LOGTEXT2(_L8("PKTLOOPBACK:NotifyOutputEmpty is not supported:  Unit %d..."), iPortName);
	}

void CHWPort::NotifyOutputEmptyCancel()
/**
 * This method is currently not implemented in the loopback driver.
 */
	{
	LOGTEXT2(_L8("PKTLOOPBACK:NotifyOutputEmptyCancel is not supported:  Unit %d..."), iPortName);
	}

TInt CHWPort::GetFlowControlStatus(TFlowControl& /* aFlowControl */)
/**
 * This method is currently not implemented in the loopback driver.
 *
 * @return	KErrNotSupported
 */
	{
	LOGTEXT2(_L8("PKTLOOPBACK:GetFlowControlStatus is not supported:  Unit %d..."), iPortName);

	return KErrNotSupported;
	}

TInt CHWPort::GetRole(TCommRole& aRole)
/**
 * This method returns the current Role that this port is playing (ECommRoleDCE or ECommRoleDTE)
 *
 * @param	aRole	-	a reference to a TCommRole to return the role value in.
 *
 * @return	KErrNone
 */
	{
	LOGTEXT3(_L8("PKTLOOPBACK:GetRole:  Unit %d, aRole %d..."), iPortName, aRole);

	aRole = iRole;
	return KErrNone;
	}

TInt CHWPort::SetRole(TCommRole aRole)
/**
 * This method sets the role of the port.  Additionally, it sets the default state of the
 * signals for this type of port.  This is the first place where signals are set as a port
 * is opening.  The ports will assert their output signals with the exception of DCD (which
 * is an output signal from the DCE).  DCD is left to be driven by the test harness.
 *
 * A test could be put into place to insure that each port is in a different role.  This was
 * not done at this time for backwards compatibility reasons.
 *
 * @param	aRole	-	the TCommRole value that this port should be set to.
 *
 * @return	KErrNone
 */
	{
	LOGTEXT3(_L8("PKTLOOPBACK:SetRole:  Unit %d, aRole %d..."), iPortName, aRole);

	if (ECommRoleDTE == aRole)
		{
		SetSignalsToMark(KSignalDTR|KSignalRTS);
		}
	else // DCE
		{
		SetSignalsToMark(KSignalDSR|KSignalCTS);
		}


	// Informational test only.  This will produce output to the log file if both sides have the
	// same role set.  With both sides having the same role, none of the signal handling will
	// function properly.
#if defined (_DEBUG)
	if (iLoopbackPort)
		{
		TCommRole otherSide;
		iLoopbackPort->GetRole(otherSide);
		if (otherSide == aRole)
			LOGTEXT3(_L8("PKTLOOPBACK:SetRole:  Unit %d...Both sides same role %d"), iPortName, aRole);
		}
#endif

	iRole = aRole;
	return KErrNone;
	}

CHWPort::~CHWPort()
/**
 * This method is the standard destructor for the port.  It deletes the buffer
 * which was allocated to the port and resets the loopback port pointer to NULL.
 */
	{
	delete iFlowControlChange;
	iFlowControlChange = NULL;
	
	delete iReadWriteQueue;
	iReadWriteQueue = NULL;
	
	delete iWriteDelayTimer;
	iWriteDelayTimer = NULL;

	if(iLoopbackPort)
		{
		iLoopbackPort->SetLoopbackPort(NULL);
		iLoopbackPort->iWriteDelayTimer->Cancel();
		}
	
	((CHWPortFactory*)Owner())->Remove(this);
	}

void CHWPort::FreeMemory()
/**
 * This method is currently not implemented in the loopback driver.
 */
	{}

CHWPort* CHWPortFactory::FindPort(TUint aUnit)
/**
 * Returns the port corresponding to the unit aUnit
 *
 * @param aUnit The port number for the port object to find
 *
 * @return The port object corresponding to unit aUnit
 */
	{
	for (TInt i = 0; i < iPorts.Count(); i++)
		{
		if (iPorts[i]->PortName() == aUnit)
			return iPorts[i];
		}
	return NULL;
	}
	
CPort* CHWPortFactory::NewPortL(const TUint aUnit)
/**
 * This method creates a new port object.  It identifies the new object with the unit number that
 * is supplied.  If both ports that are supported by the CHWPortFactory object have been created,
 * then the loopback ports are initialized.
 *
 * @param	aUnit		-	The unit number to create.
 *
 * @return	CPort *		-	A pointer to the newly created object.
 */
	{
	LOGTEXT2(_L8("PKTLOOPBACK:NewPortL:  Unit %d"), aUnit);
	
	// Get the settings for the given port. The iLoopbackConfig will look at the test's ini file for this information
	TLoopbackConfigItem portSettings;
	if (iLoopbackConfig)
		{
		if (iLoopbackConfig->GetPortSettings(aUnit, portSettings) != KErrNone)
			User::Leave(KErrNotFound);
		}
	else
		{
		User::Leave(KErrNotFound);
		}
	
	// If the port already exists, just return it
	CHWPort *existingPort = FindPort(aUnit);
	if (existingPort)
		{
		LOGTEXT2(_L8("Loopback:NewPortL:  Unit %d already exists! @%x"), aUnit);
		return existingPort;
		}
	// Create the port and add it to the list of ports
	CHWPort *newPort;
	if (iLoopbackConfig->PortType() == ESerialLoopbackPortType)
		{
		newPort = CHWPort::NewSerialLoopbackL(aUnit, portSettings.iDelay, portSettings.iBufferSize);
		}
	else
		{
		newPort = CHWPort::NewPacketLoopbackL(aUnit, portSettings.iDelay, portSettings.iPacketLength, portSettings.iQueueLength);
		}
	CleanupStack::PushL(newPort);
	iPorts.AppendL(newPort);
	CleanupStack::Pop(newPort);
	
	// Get the name of the opposite loopback port
	TUint oppositePortName;
	if (portSettings.iPortA == aUnit)
		oppositePortName = portSettings.iPortB;
	else
		oppositePortName = portSettings.iPortA;
	
	// If the opposite loopback port is open, set it as the current port's loopback
	CHWPort *oppositePort = FindPort(oppositePortName);
	if (oppositePort)
		{
		oppositePort->SetLoopbackPort(newPort);
		newPort->SetLoopbackPort(oppositePort);
		}
	return newPort;
	}

void CHWPortFactory::Info(TSerialInfo &aSerialInfo)
/**
 * This method fills information into the passed structure.  It is required for factory objects.
 *
 * @param	aSerialInfo		-	a reference to the structure to fill in.
 */
	{
	aSerialInfo.iDescription = KSerialDescription;
	aSerialInfo.iName = KSerialName;
	aSerialInfo.iLowUnit = KCommLowUnit;
	aSerialInfo.iHighUnit = KCommHighUnit;
	}

CHWPortFactory::CHWPortFactory()
/**
 * This method is the constructor for the factory object.
 * We initialize iPorts to contain the number of ports from KMinLoopBackPort to KMaxLoopBackPort
 */
	{
	TName name(KSerialName);
	SetName(&name);
	iVersion = TVersion(KEC32MajorVersionNumber,KEC32MinorVersionNumber,KEC32BuildVersionNumber);
	
	// Create the object to read the loopback port settings for this test
	TRAP_IGNORE(iLoopbackConfig = CLoopbackConfig::NewL());
	}

void CHWPortFactory::Remove(CHWPort* aPort)
/**
 * This method removes an instance of the CHWPort from the factory package CHWPortFactory.
 * The object is not deleted.
 *
 * @param	aPort	-	The pointer to the CHWPort pointer to be removed from the factory object.
 *
 * @note	If the passed in value does not match a current port, this method will panic.
 */
	{
	LOGTEXT2(_L8("PKTLOOPBACK:Remove:  Port %d"), aPort->PortName());
	for (TInt i = 0; i < iPorts.Count(); i++)
		{
		if(iPorts[i] == aPort)
			{
			iPorts.Remove(i);
			return;
			}
		}
	User::Panic(_L("CHWPortFactory: Port not found"),0);
	}

CHWPortFactory::~CHWPortFactory()
/**
 * This method is the destructor for the factory object.
 */
	{
    ASSERT(iPorts.Count()==0);
    iPorts.Close();
    
	delete iLoopbackConfig;
	iLoopbackConfig = NULL;
	LOGDESTROY();
	}
	
/**
Returns capabilities for requested port
*/
TSecurityPolicy CHWPortFactory::PortPlatSecCapability(TUint /*aPort*/) const
	{
	return TSecurityPolicy(TSecurityPolicy::EAlwaysPass);	
	}

extern "C"
	{
	IMPORT_C CSerial * LibEntry(void);	// Force export 

	EXPORT_C CSerial * LibEntry(void)
	/**
	 * This method is the library's main entry point.  It simply new's the factory object.
	 */
		{
		return new CHWPortFactory;
		}

	} // extern "C"


