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
// This file implements a loopback driver for use with the ETel Regression test harness.  This
// driver supports up to 100 virtual port connections, from port 501 to 600
// directory exists.  The log file is named loopback.
// 
//

/**
 @file
 @note	This driver will create a log file in the Logs\ETel directory if the Logs\ETel
*/

#include "LoopbackQueue.h"
#include "LoopbackConfig.h"

CTPtr8Queue* CTPtr8Queue::NewL(TInt aBufferLength, TInt aQueueLength)
	{
	CTPtr8Queue* me = new(ELeave) CTPtr8Queue(aQueueLength);
	CleanupStack::PushL(me);
	me->ConstructL(aBufferLength, aQueueLength);
	CleanupStack::Pop(me);
	return me;
	}
	
CTPtr8Queue::CTPtr8Queue(TInt aQueueLength) : iFirstBuffer(0), iNextBuffer(0), iQueueFull(EFalse), 
	iDescriptors(aQueueLength)
	{}
	
void CTPtr8Queue::ConstructL(TInt aBufferLength, TInt aQueueLength)
	{
	HBufC8* buffer = HBufC8::NewL(aBufferLength*aQueueLength);
	// don't push onto the cleanup stack, as we are assigning to member variable here
	iBuffer = buffer;
	TPtr8 tmp = buffer->Des();
	tmp.AppendFill('C', aBufferLength*aQueueLength);
	for (TInt i = 0; i < aQueueLength; i++)
		{
		TUint8& startChar = tmp[i*aBufferLength];
		TPtr8 nextElement = TPtr8(&startChar, aBufferLength);
		iDescriptors.AppendL(nextElement);
		}
	}
	
CTPtr8Queue::~CTPtr8Queue()
	{
	delete iBuffer;
	}
	
void CTPtr8Queue::Clear()
	{
	iFirstBuffer = iNextBuffer = 0;
	iQueueFull = EFalse;
	}
	
TBool CTPtr8Queue::IsEmpty() const
	{
	if (iQueueFull)
		return EFalse;
	else
		return iFirstBuffer == iNextBuffer;
	}
	
TBool CTPtr8Queue::QueueLength() const
	{
	if (iQueueFull)
		return iDescriptors.Count();
	else
		return Abs(iFirstBuffer - iNextBuffer) % iDescriptors.Count();
	}
	
TBool CTPtr8Queue::IsFull() const
	{
	if (iQueueFull)
		{
		ASSERT(iFirstBuffer == iNextBuffer);
		}
	return iQueueFull;
	}
	
TPtr8& CTPtr8Queue::GetNewBuffer()
	{
	ASSERT(!IsFull());
	TPtr8& ret = iDescriptors[iNextBuffer];
	iNextBuffer++;
	if (iNextBuffer >= iDescriptors.Count())
		iNextBuffer = 0;
	if (iNextBuffer == iFirstBuffer)
		iQueueFull = ETrue;
	return ret;
	}
	
const TPtr8& CTPtr8Queue::PeekFirstBuffer() const
	{
	ASSERT(!IsEmpty());
	return iDescriptors[iFirstBuffer];
	}

TPtr8& CTPtr8Queue::DequeueFirstBuffer()
	{
	ASSERT(!IsEmpty());
	TPtr8& ret = iDescriptors[iFirstBuffer];
	iFirstBuffer++;
	if (iFirstBuffer >= iDescriptors.Count())
		iFirstBuffer = 0;
	iQueueFull = EFalse;
	return ret;
	}

MLoopbackQueue::~MLoopbackQueue()
	{}
	
CPacketBufferQueue* CPacketBufferQueue::NewL(TInt aBufferLength, TInt aQueueLength)
	{
	CPacketBufferQueue* me = new (ELeave)CPacketBufferQueue(aBufferLength);
	CleanupStack::PushL(me);
	me->ConstructL(aBufferLength, aQueueLength);
	CleanupStack::Pop(me);
	return me;
	}
	
CPacketBufferQueue::CPacketBufferQueue(TInt aBufferLength) : iBufferLength(aBufferLength)
	{}
	
void CPacketBufferQueue::ConstructL(TInt aBufferLength, TInt aQueueLength)
	{
	iReadQueue = CTPtr8Queue::NewL(aBufferLength, aQueueLength);
	iWriteQueue = CTPtr8Queue::NewL(aBufferLength, aQueueLength);
	}
	
CPacketBufferQueue::~CPacketBufferQueue()
	{
	delete iReadQueue;
	delete iWriteQueue;
	}
	
TPtrC8 CPacketBufferQueue::PeekNextReadBuffer(TBool& aCompleteIfBufferNotFull) const
	{
	aCompleteIfBufferNotFull = ETrue;
	return iReadQueue->PeekFirstBuffer();
	}

void CPacketBufferQueue::DiscardNextReadBuffer()
	{
	iReadQueue->DequeueFirstBuffer();
	}

void CPacketBufferQueue::AppendToReadBuffer(TDesC8& aSrcData)
	{
	TPtr8& destBuf = iReadQueue->GetNewBuffer();
	destBuf.Copy(aSrcData);
	}

TPtrC8 CPacketBufferQueue::PeekNextWriteBuffer() const
	{
	return iWriteQueue->PeekFirstBuffer();
	}

void CPacketBufferQueue::DiscardNextWriteBuffer()
	{
	iWriteQueue->DequeueFirstBuffer();
	}

TPtr8 CPacketBufferQueue::AppendToWriteBuffer(TInt aLength)
	{
	TPtr8& newBuf = iWriteQueue->GetNewBuffer();
	newBuf.FillZ(aLength);
	newBuf.LeftTPtr(aLength);
	return newBuf;
	}

TBool CPacketBufferQueue::IsReadBufferEmpty() const
	{
	return iReadQueue->IsEmpty();
	}

TBool CPacketBufferQueue::IsWriteBufferEmpty() const
	{
	return iWriteQueue->IsEmpty();
	}

TBool CPacketBufferQueue::IsReadBufferFull(TInt /*aLength*/) const
	{
	return iReadQueue->IsFull();
	}

TBool CPacketBufferQueue::IsWriteBufferFull(TInt /*aLength*/) const
	{
	return iWriteQueue->IsFull();
	}

TInt CPacketBufferQueue::BufferSize() const
	{
	return iBufferLength;
	}

void CPacketBufferQueue::Clear()
	{
	iWriteQueue->Clear();
	iReadQueue->Clear();
	}

CSerialBufferQueue* CSerialBufferQueue::NewL(TInt aBufferLength, TCommConfigV01* aConfig)
	{
	CSerialBufferQueue* me = new (ELeave)CSerialBufferQueue(aBufferLength, aConfig);
	CleanupStack::PushL(me);
	me->ConstructL(aBufferLength);
	CleanupStack::Pop(me);
	return me;
	}
	
CSerialBufferQueue::CSerialBufferQueue(TInt aBufferLength, TCommConfigV01* aConfig) : 
	iBufferLength(aBufferLength), iConfig(aConfig), iWriteData(NULL, 0), iReadData(NULL, 0)
	{}
	
void CSerialBufferQueue::ConstructL(TInt aBufferLength)
	{
	iReadBuffer = HBufC8::NewL(aBufferLength);
	iWriteBuffer = HBufC8::NewL(aBufferLength);
	iReadData.Set(iReadBuffer->Des());
	iWriteData.Set(iWriteBuffer->Des());
	}
	
CSerialBufferQueue::~CSerialBufferQueue()
	{
	delete iReadBuffer;
	delete iWriteBuffer;
	}
	
TPtrC8 CSerialBufferQueue::NextBuffer(const TPtrC8& aBuffer, TBool& aTerminatorFound) const
	{
	// Search for terminator characters
	TInt len = 0;
	aTerminatorFound = EFalse;
	const TText8* terminators = iConfig->iTerminator;
	TPtrC8 dataToTerminator(aBuffer);
	for (TInt termNum = 0; termNum < iConfig->iTerminatorCount; ++termNum)
		{
		TInt termIndex = dataToTerminator.Locate(terminators[termNum]);
		if (termIndex >= 0)
			{
			// Found the terminator; reduce the length of ptr and search for the next one
			len = termIndex + 1;
			dataToTerminator.Set(dataToTerminator.Ptr(), len);
			aTerminatorFound = ETrue;
			}
		}
	return dataToTerminator;
	}

TPtrC8 CSerialBufferQueue::PeekNextReadBuffer(TBool& aCompleteIfBufferNotFull) const
	{
	// if the terminator is found, we want the requrest completed even if the buffer is not full
	return NextBuffer(iReadData, aCompleteIfBufferNotFull);
	}

void CSerialBufferQueue::DiscardNextReadBuffer()
	{
	TBool terminatorFound;
	TPtrC8 dataToTerminator = NextBuffer(iReadData, terminatorFound);

	if (terminatorFound)
		{
		TPtrC8 source = iReadData.Right(iReadData.Length()-dataToTerminator.Length());
		iReadData.Copy(source);
		}
	else
		{
		iReadData.SetLength(0);
		}
	}

void CSerialBufferQueue::AppendToReadBuffer(TDesC8& aSrcData)
	{
	ASSERT(aSrcData.Length() < iReadData.MaxLength()-iReadData.Length());
	iReadData.Append(aSrcData);
	}

TPtrC8 CSerialBufferQueue::PeekNextWriteBuffer() const
	{
	TBool dummy;
	return NextBuffer(iWriteData, dummy);
	}

void CSerialBufferQueue::DiscardNextWriteBuffer()
	{
	TBool terminatorFound;
	TPtrC8 dataToTerminator = NextBuffer(iWriteData, terminatorFound);
	
	// Delete the write data from the buffer
	if (terminatorFound)
		{
		TPtrC8 source = iWriteData.Right(iWriteData.Length()-dataToTerminator.Length());
		iWriteData.Copy(source);
		}
	else
		{
		iWriteData.SetLength(0);
		}
	}

TPtr8 CSerialBufferQueue::AppendToWriteBuffer(TInt aLength)
	{
	TInt length = iWriteData.Length();
	ASSERT(aLength < iWriteData.MaxLength()-length);
	iWriteData.AppendFill('C', aLength);
	TUint8 *start = &iWriteData[length];
	return TPtr8(start, aLength, aLength);
	}
	
TBool CSerialBufferQueue::IsReadBufferEmpty() const
	{
	return iReadData.Length() == 0;
	}

TBool CSerialBufferQueue::IsWriteBufferEmpty() const
	{
	return iWriteData.Length() == 0;
	}

TBool CSerialBufferQueue::IsReadBufferFull(TInt aLength) const
	{
	return ( aLength > (iReadData.MaxLength()-iReadData.Length()) );
	}

TBool CSerialBufferQueue::IsWriteBufferFull(TInt aLength) const
	{
	return ( aLength > (iWriteData.MaxLength()-iWriteData.Length()) );
	}

TInt CSerialBufferQueue::BufferSize() const
	{
	return iWriteData.MaxLength()-iWriteData.Length();
	}

void CSerialBufferQueue::Clear()
	{
	}

