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

#include "es_commsbuf_internal.h"

__IMPLEMENT_CLEANUP(RCommsBufQ, Free)


EXPORT_C RCommsBufQ::RCommsBufQ(RCommsBuf* aChain)
 	{
	iNext = aChain;
	iLast = RCommsBufChain(aChain).Last(); // Safe even if aChain==NULL
	}


EXPORT_C void RCommsBufQ::Init()
/**
initializes the members
*/
	{
	iNext = NULL;
	iLast = NULL;
	}


EXPORT_C void RCommsBufQ::Assign(RCommsBufQ& aQueue)
/**
Assign this queue to a RCommsBuf queue
@param aQueue to queue
*/
	{
	*this = aQueue;
	aQueue.Init();
	}


EXPORT_C void RCommsBufQ::Assign(RCommsBufChain& aChain)
/**
Assign this a RCommsBuf chain to this queue
@param aChain the chain
*/
 	{
	iNext = aChain.First();
	iLast = aChain.Last();
	aChain.iNext = NULL;
	}


EXPORT_C void RCommsBufQ::Free()
/**
Frees the queue making it empty
*/
	{
	if(iNext)
		iNext->Free();
	Init();
	}


EXPORT_C void RCommsBufQ::Append(RCommsBuf* aBuf)
/**
Appends a RCommsBuf to the queue
@param aBuf the buffer to be prepended
*/
 	{
	if (IsEmpty())
		iNext = iLast = aBuf;
	else
		{
		iLast->SetNext(aBuf);
		iLast = aBuf;
		}
	}


EXPORT_C void RCommsBufQ::Prepend(RCommsBuf* aBuf)
/**
Prepends one RCommsBuf to this queue. aBuf must not point to any further CommsBufs.
@param aBuf the buffer to be appended
*/
	{

    __ASSERT_DEBUG(aBuf->Next()==NULL, CommsBuf::Panic(EMBuf_CannotPrependChainedMBuf));
	if (IsEmpty())
		{
		iNext = aBuf;
		iLast = aBuf;
		aBuf->SetNext(NULL);	// break the mbuf chain (if any)
		}
	else
		{
		aBuf->SetNext(iNext);	// alter the mbuf chain (if any)
		iNext = aBuf;
		}
	}


EXPORT_C void RCommsBufQ::Append(RCommsBufQ& aQueue)
/**
Appends a RCommsBuf queue to this queue
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
		iLast->SetNext(aQueue.iNext);
		iLast = aQueue.iLast;
		}
	}


EXPORT_C void RCommsBufQ::Append(RCommsBufChain& aChain)
/**
Appends a RCommsBuf chain to this queue
@param aChain the chain to be appended
*/
 	{
	if (aChain.IsEmpty())
		return;

	if (IsEmpty())
		iNext = aChain.First();
	else
		iLast->SetNext(aChain.First());
	iLast = aChain.Last();
	aChain.iNext = NULL;
	}


EXPORT_C void RCommsBufQ::Prepend(RCommsBufChain& aChain)
/**
Prepends a RCommsBuf chain to this queue
@param aChain the chain to be prepended
*/
 	{
	if (aChain.IsEmpty())
		return;

	if (IsEmpty())
		iLast = aChain.Last();
	else
		aChain.Last()->SetNext(iNext);
	iNext = aChain.First();
	aChain.iNext = NULL;
	}


EXPORT_C void RCommsBufQ::Prepend(RCommsBufQ& aQueue)
/**
Prepends a RCommsBuf queue to this queue
@param aQueue the queue to be prepended
*/
 	{
	if (aQueue.IsEmpty())
		return;

	if (IsEmpty())
		Assign(aQueue);
	else
		{
		aQueue.iLast->SetNext(iNext);
		iNext = aQueue.iNext;
		}
	}


EXPORT_C RCommsBuf* RCommsBufQ::Remove()
/**
Removes the first RCommsBuf from the queue
@return the MBuf
*/
	{
	RCommsBuf* m;

	if (IsEmpty())
		return NULL;

	m = iNext;

	if (iNext = m->Next(), iNext==NULL)
		iLast = NULL;

	m->SetNext(NULL);

	return m;
	}

EXPORT_C TInt RCommsBufQ::Transfer(RCommsBufQ& aQueue, TInt aSize, TInt aBufSize, TInt& aCount)
    {
    __ASSERT_DEBUG(aSize >= 0, CommsBuf::Panic(EMBuf_BadBufferSize));

    TInt transfered = 0;
    aCount = 0;
    
    if (!IsEmpty())
        {
        RCommsBuf* first;
        RCommsBuf* next;
        RCommsBuf* last;
    
        first = iNext;
        next = first;
        last = first;
    
        do 
            {
            ++aCount;
            transfered += aBufSize;
            aSize -= aBufSize;
            last = next;
            next = next->Next();
            } while (aSize > 0 && next != NULL);
            
        if (next)
            {
            last->SetNext(NULL);
            iNext = next;
            }
        else
            {
            iNext = NULL;
            iLast = NULL;
            }

        RCommsBufQ q(first, last);
        
        if (aSize < 0)
            {
            q.Last()->AdjustDataEnd(aSize);
            }
        aQueue.Append(q);
        }
    return transfered;
    }

EXPORT_C RCommsBuf* RCommsBufQ::RemoveLast()
    {
    RCommsBuf* current = iNext;
    if(iNext == iLast)
        {
        iNext = iLast = NULL;
        return current;
        }

    while(current->Next() != iLast)
        {
        current = current->Next();
        }
    RCommsBuf* last = iLast;
    current->SetNext(NULL);
    iLast = current;
    return last;
    }

