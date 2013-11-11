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
// Buffer Manager for Protocols (MBuf and Packet Queues)
// 
//

/**
 @file
 @internalComponent
*/

#include <es_mbman.h>

//
// MBUF QUEUE
//

__IMPLEMENT_CLEANUP(RMBufQ, Free)


EXPORT_C RMBufQ::RMBufQ(RMBuf* aChain)
/**
Constructor initializes the members
*/
 	{
	iNext = aChain;
	iLast = aChain->Last(); // Safe even if aChain==NULL
	}


EXPORT_C void RMBufQ::Init()
/**
initializes the members
*/
	{
	iNext = NULL;
	iLast = NULL;
	}


EXPORT_C void RMBufQ::Assign(RMBufQ& aQueue)
/**
Assign this queue to a MBuf queue
@param aQueue to queue
*/
	{
	*this = aQueue;
	aQueue.Init();
	}


EXPORT_C void RMBufQ::Assign(RMBufChain& aChain)
/**
Assign this a MBuf chain to this queue
@param aChain the chain
*/
 	{
	iNext = aChain.First();
	iLast = aChain.Last();
	aChain.Init();
	}


EXPORT_C void RMBufQ::Free()
/**
Frees the queue making it empty
*/
	{
	if(iNext)
		iNext->Free();
	Init();
	}


EXPORT_C void RMBufQ::Append(RMBuf* aBuf)
/**
Appends a MBuf to the queue
@param aBuf the buffer to be prepended
*/
 	{
	if (IsEmpty())
		iNext = iLast = aBuf;
	else
		{
		iLast->Link(aBuf);
		iLast = aBuf;
		}
	}


EXPORT_C void RMBufQ::Prepend(RMBuf* aBuf)
/**
Prepends one MBuf to this queue. aBuf must not point to any further Mbufs.
@param aBuf the buffer to be appended
*/
	{

    __ASSERT_DEBUG(aBuf->Next()==NULL, CMBufManager::Panic(EMBuf_CannotPrependChainedMBuf));
	if (IsEmpty())
		{
		iNext = aBuf;
		iLast = aBuf;
		aBuf->Link(NULL);	// break the mbuf chain (if any)
		}
	else
		{
		aBuf->Link(iNext);	// alter the mbuf chain (if any)
		iNext = aBuf;
		}
	}


EXPORT_C void RMBufQ::Append(RMBufQ& aQueue)
/**
Appends a MBuf queue to this queue
@param aQueue the queue to be appended
*/
 	{
	if (aQueue.IsEmpty())
		return;

	if (IsEmpty())
		{
		// src queue is reset, thus this operation is an append & move
		*this = aQueue;
		aQueue.Init();      // trs; why zero the src queue? implies a move instead of just append.  ideally this should be fixed, but kept as is to avoid a functional break
		}
	else
		{
		// src queue is not altered, thus this operation is only an append (ie. no move)
		iLast->Link(aQueue.iNext);
		iLast = aQueue.iLast;
		}
	}


EXPORT_C void RMBufQ::Append(RMBufChain& aChain)
/**
Appends a MBuf chain to this queue
@param aChain the chain to be appended
*/
 	{
	if (aChain.IsEmpty())
		return;

	if (IsEmpty())
		iNext = aChain.First();
	else
		iLast->Link(aChain.First());
	iLast = aChain.Last();
	aChain.Init();
	}


EXPORT_C void RMBufQ::Prepend(RMBufChain& aChain)
/**
Prepends a MBuf chain to this queue
@param aChain the chain to be prepended
*/
 	{
	if (aChain.IsEmpty())
		return;

	if (IsEmpty())
		iLast = aChain.Last();
	else
		aChain.Last()->Link(iNext);
	iNext = aChain.First();
	aChain.Init();
	}


EXPORT_C void RMBufQ::Prepend(RMBufQ& aQueue)
/**
Prepends a MBuf queue to this queue
@param aQueue the queue to be prepended
*/
 	{
	if (aQueue.IsEmpty())
		return;

	if (IsEmpty())
		Assign(aQueue);
	else
		{
		aQueue.iLast->Link(iNext);
		iNext = aQueue.iNext;
		}
	}


EXPORT_C RMBuf* RMBufQ::Remove()
/**
Removes the first MBuf from the queue
@return the MBuf
*/
	{
	RMBuf* m;

	if (IsEmpty())
		return NULL;

	m = iNext;

	if (iNext = m->Next(), iNext==NULL)
		iLast = NULL;

	m->Unlink();

	return m;
	}

TInt RMBufQ::Transfer(RMBufQ& aQueue, TInt aSize)
/**
For the benefit of the MBuf allocator
Grab as many buffer as possible, as quickly as possible.
@param aQueue the queue
@param aSize the size
@return aSize - buffer_space_transfered
*/
	{
	__ASSERT_DEBUG(aSize >= 0, CMBufManager::Panic(EMBuf_BadBufferSize));

	TInt transfered = 0;
	
	if (!IsEmpty())
		{
		RMBuf* first;
		RMBuf* next;
		RMBuf* last;
	
		first = iNext;
		next = first;
		last = first;
	
		while (aSize >= 0 && next != NULL)
			{
			transfered += next->Size();
			aSize -= next->Size();
			last = next;
			next = next->Next();
			if (aSize == 0)
				{
				break;
				}
			}
			
		if (next)
			{
			last->Unlink();
			iNext = next;
			}
		else
			{
			iNext = NULL;
			iLast = NULL;
			}

		RMBufQ q(first, last);
		
		if (aSize < 0)
			{
			q.First()->AdjustEnd(aSize);
			}
		aQueue.Prepend(q);
		}
	return transfered;
	}


//
// MBUF PACKET QUEUE
//

__IMPLEMENT_CLEANUP(RMBufPktQ, Free)


EXPORT_C void RMBufPktQ::Init()
/**
Initializes the the members
*/
	{
	iNext.Init();
	iLast.Init();
	}


EXPORT_C void RMBufPktQ::Free()
/**
Frees the RMBuf Paket Queue, delets all objects
*/
	{
	RMBufChain chain;
	while (Remove(chain))
		chain.Free();
	}


EXPORT_C void RMBufPktQ::Append(RMBufChain& aChain)
/**
Appends a MBuf chain to the queue
@param aCHain A MBuf Chain
*/
 	{
	if (IsEmpty())
		{
		iNext = aChain;
		iLast = aChain;
		}
	else
		{
		iLast.Link(aChain);
		iLast = aChain;
		}
	aChain.Init();
	}


EXPORT_C void RMBufPktQ::Append(RMBufPktQ& aQueue)
/**
Appends a queue to the queue
@param aQueue a BMuf Paket Queue
*/
 	{
	if (aQueue.IsEmpty())
		return;

	if (IsEmpty())
		{
		iNext = aQueue.iNext;
		iLast = aQueue.iLast;
		}
	else
		{
		iLast.Link(aQueue.iNext);
		iLast = aQueue.iLast;
		}
	aQueue.Init();
	}


EXPORT_C void RMBufPktQ::Prepend(RMBufChain& aChain)
/**
Prepends a MBuf chain to the queue
@param aCHain A MBuf Chain
*/
 	{
	if (IsEmpty())
		{
		iNext = aChain;
		iLast = aChain;
		}
	else
		{
		aChain.Link(iNext);
		iNext = aChain;
		}
	aChain.Init();
	}


EXPORT_C TBool RMBufPktQ::Remove(RMBufChain& aChain)
/**
Prepends a queue to the queue
@param aQueue a BMuf Packet Queue
*/
	{
	if (IsEmpty())
		return EFalse;

	aChain = iNext;

	if (iNext = iNext.Next(), iNext.IsEmpty())
		iLast.Init();

	aChain.Unlink();
	return ETrue;
	}


void RMBufPktQ::Insert(RMBufChain& aNew, RMBufChain& aPrev)
/**
Inserts a MBuf chain next to a member in the queue
@param aNew A MBuf chain to be inserted
@param aPrev the member (a RMBufChain object) of the chain next to which the chain in inserted into
*/
	{
	if (aPrev.IsEmpty())
		Prepend(aNew);
	else if (aPrev.Next().IsEmpty())
 		Append(aNew);
	else
		{
		RMBufChain tmp;
		tmp.Assign(aNew);
		tmp.Link(aPrev.Next());
		aPrev.Link(tmp);
		}
	}

void RMBufPktQ::Remove(RMBufChain& aNew, RMBufChain& aPrev)
/**
Removes the part after a member from the queue
@param aNew the resulted removed part
@param aPrevious the memeber (a RMBufChain object) of the chain the part of which is removed from the queue
*/
	{
	if (aPrev.IsEmpty())
		Remove(aNew);
	else
		{
		aNew.Assign(aPrev.Next());
		aPrev.Link(aNew.Next());
		aNew.Unlink();
		}
	}

EXPORT_C void TMBufPktQIter::Insert(RMBufChain& aNewChain)
/**
Inserts a chain into the queue to the part after current position
@param aNewChain the chain to be inserted
*/
	{
	iQueue->Insert(aNewChain, iPrev);
	TidyAfterUpdate();
	}

EXPORT_C void TMBufPktQIter::Remove(RMBufChain& aNewChain)
/**
Removes the part after the current position
@param aNewChain the resulted removed queue
*/
	{
	iQueue->Remove(aNewChain, iPrev);
	TidyAfterUpdate();
	}

EXPORT_C const RMBufChain& TMBufPktQIter::operator ++(TInt)
/**
Operator ++
move to the next position in the queue
*/
	{
	if (iPrev = iCurrent, !iPrev.IsEmpty())
		iCurrent = iPrev.Next();
	return iPrev;
	}

EXPORT_C void TMBufPktQIter::TidyAfterUpdate()
/**
Update the current position, after update
if the current position becomes empty after update - the current position will
be the first in the queue, else the next in the queue
*/
	{
	if (iPrev.IsEmpty())
		SetToFirst();
	else
		iCurrent = iPrev.Next();
	}
