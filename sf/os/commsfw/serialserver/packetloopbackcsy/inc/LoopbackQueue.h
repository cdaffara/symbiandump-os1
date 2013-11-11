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
// This file implements a queue for the loopback driver
// 
//

/**
 @file
*/

#ifndef __LOOPBACK_QUEUE_H__
#define __LOOPBACK_QUEUE_H__

#include <cs_port.h>
#include "SLOGGER.H"
#include <d32comm.h>
#include <c32comm.h>
#include <e32hal.h>
#include <e32base.h>

#include "LoopbackConfig.h"
#include "LoopbackTimer.h"

/**
 A queue class of descriptors - all of the data for the desctiptors is contained in this class
 */
NONSHARABLE_CLASS(CTPtr8Queue) : CBase
	{
public:
	static CTPtr8Queue* NewL(TInt aBufferLength, TInt aQueueLength);
	~CTPtr8Queue();
	TPtr8& GetNewBuffer();
	TPtr8& DequeueFirstBuffer();
	const TPtr8& PeekFirstBuffer() const;
	TInt QueueLength() const;
	TBool IsEmpty() const;
	TBool IsFull() const;
	void Clear();
private:
	CTPtr8Queue(TInt aQueueLength);
	void ConstructL(TInt aBufferLength, TInt aQueueLength);

	/** Index of the first buffer in the queue */
	TInt iFirstBuffer;
	/** Index of the next buffer in the queue */
	TInt iNextBuffer;
	/** True if the buffer is full (iFirstBuffer should also equal iNextBuffer */
	TBool iQueueFull;
	
	/** Pointer to the descriptors, each of which represents an element in the queue */
	CArrayFixFlat<TPtr8> iDescriptors;
	/** Buffer for all of the data in all of the elements of the queue */
	TDesC8 *iBuffer;
	};

/**
 Contains the read and write queues for a given port
 */
NONSHARABLE_CLASS(MLoopbackQueue)
	{
public:
	virtual ~MLoopbackQueue();
	virtual TPtrC8 PeekNextReadBuffer(TBool& aCompleteIfBufferNotFull) const = 0;
	virtual void DiscardNextReadBuffer() = 0;
	virtual void AppendToReadBuffer(TDesC8& aSrcData) = 0;
	virtual TPtrC8 PeekNextWriteBuffer() const = 0;
	virtual void DiscardNextWriteBuffer() = 0;
	virtual TPtr8 AppendToWriteBuffer(TInt aLength) = 0;
	virtual TBool IsWriteBufferEmpty() const = 0;
	virtual TBool IsWriteBufferFull(TInt aLength) const = 0;
	virtual TBool IsReadBufferEmpty() const = 0;
	virtual TBool IsReadBufferFull(TInt aLength) const = 0;
	virtual TInt BufferSize() const = 0;
	virtual void Clear() = 0;
	};

/**
 Contains the read and write queues for a given port
 */
NONSHARABLE_CLASS(CPacketBufferQueue) : public CBase, public MLoopbackQueue
	{
public:
	static CPacketBufferQueue* NewL(TInt aBufferLength, TInt aQueueLength);
	~CPacketBufferQueue();
	void DiscardNextReadBuffer();
	TPtrC8 PeekNextReadBuffer(TBool& aCompleteIfBufferNotFull) const;
	TPtr8 AppendToWriteBuffer(TInt aLength);
	void DiscardNextWriteBuffer();
	TPtrC8 PeekNextWriteBuffer() const;
	void AppendToReadBuffer(TDesC8& aSrcData);
	TBool IsWriteBufferEmpty() const;
	TBool IsWriteBufferFull(TInt aLength) const;
	TBool IsReadBufferEmpty() const;
	TBool IsReadBufferFull(TInt aLength) const;
	TInt BufferSize() const;
	void Clear();
private:
	CPacketBufferQueue(TInt aBufferLength);
	void ConstructL(TInt aBufferLength, TInt aQueueLength);

	/** The length of each element in each queue */
	TInt iBufferLength;

	/** The read queue for a port */
	CTPtr8Queue *iReadQueue;
	/** The write queue for a port */
	CTPtr8Queue *iWriteQueue;
	};

/**
 Contains the read and write queues for a given port
 */
NONSHARABLE_CLASS(CSerialBufferQueue) : public CBase, public MLoopbackQueue
	{
public:
	static CSerialBufferQueue* NewL(TInt aBufferLength, TCommConfigV01* aConfig);
	~CSerialBufferQueue();
	void DiscardNextReadBuffer();
	TPtrC8 PeekNextReadBuffer(TBool& aCompleteIfBufferNotFull) const;
	void AppendToReadBuffer(TDesC8& aSrcData);
	TPtrC8 PeekNextWriteBuffer() const;
	void DiscardNextWriteBuffer();
	TPtr8 AppendToWriteBuffer(TInt aLength);
	TBool IsWriteBufferEmpty() const;
	TBool IsWriteBufferFull(TInt aLength) const;
	TBool IsReadBufferEmpty() const;
	TBool IsReadBufferFull(TInt aLength) const;
	TInt BufferSize() const;
	void Clear();
private:
	CSerialBufferQueue(TInt aBufferLength, TCommConfigV01* aConfig);
	void ConstructL(TInt aBufferLength);
	TPtrC8 NextBuffer(const TPtrC8& aBuffer, TBool& aTerminatorFound) const;
	

	/** The length of each element in each queue */
	TInt iBufferLength;
	
	TCommConfigV01 *iConfig;

	HBufC8* iWriteBuffer;
	TPtr8 iWriteData;
	HBufC8* iReadBuffer;
	TPtr8 iReadData;
	};

#endif

