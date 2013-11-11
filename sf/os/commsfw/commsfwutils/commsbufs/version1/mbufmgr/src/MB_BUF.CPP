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
// Buffer Manager for Protocols(Buffer)
// 
//

/**
 @file
 @internalComponent
*/

#include <es_mbman.h>

#include "MBufPoolChain.h"
#include "MBufPoolManager.h"

//
// MBUF
//


TAny *RMBuf::operator new(TUint /*aSize*/, RMBuf* aPtr)
/**
Provide MBuf with a new in-place operator
*/
	{
	__ASSERT_DEBUG(aPtr, CMBufManager::Panic(EMBuf_BadNewInPlace));
	return aPtr;
	}


void RMBuf::operator delete(TAny* /*aPtr*/)
/**
In place delete
*/
	{
	}


void RMBuf::operator delete(TAny* /*aPtr*/, RMBuf*)
/**
extra delete operator for exception unwinding in Code Warrior
*/
	{
	}


RMBuf::RMBuf(TUint8* aBuffer, RMBufPoolChain* aPoolChain, TInt aSize)
	: iBuffer(aBuffer), iSize(aSize), iPoolChain(aPoolChain)
/**
MBuf constructor initialiser
@param aBuffer A buffer
@param aPoolChain The poolchain that contains the MBuf
@param aSize Size
*/
	{
	Init(EMBufFree);
	}


EXPORT_C void RMBuf::Init(TMBufType aType)
/**
Initialises MBuf's members
@param aType A MBuf type
*/
	{
	iType = aType;
	iLength = iSize;
	iOffset = 0;
	iNext = NULL;
	iRefCount = 0;
	iNextPkt.Init();
	}


// allocates memory for a RMBuf object, return a pointer to the RMBuf object memory
// - the size of the mbuf to be allocatd will depend on the available pool chains, eg. 128, 1514, 9000, etc
EXPORT_C RMBuf* RMBuf::AllocL()
 	{
	return CMBufManager::Context()->AllocL(KMBufSmallSize);		// default size is specified within CMufManager implementation, guaranteed to be only 1 mbuf
	}
EXPORT_C RMBuf* RMBuf::AllocL(TUint aSize)
 	{
	RMBuf* mBuf = CMBufManager::Context()->Alloc(aSize, aSize);	// min. size constraint to ensure only 1 mbuf is allocated
	if (!mBuf)
		{
		User::Leave(KErrNoMBufs);
		}
	return mBuf;
	}
EXPORT_C RMBuf* RMBuf::Alloc()
 	{
	return CMBufManager::Context()->Alloc(KMBufSmallSize);		// default size is specified within CMufManager implementation, guaranteed to be only 1 mbuf
	}
EXPORT_C RMBuf* RMBuf::Alloc(TUint aSize)
 	{
	return CMBufManager::Context()->Alloc(aSize, aSize);	// min. size constraint to ensure only 1 mbuf is allocated
	}

// overloading for TLS
EXPORT_C RMBuf* RMBuf::AllocL(RMBufAllocator& aRMBufAllocator)
 	{
	return aRMBufAllocator.MBufManager().AllocL(KMBufSmallSize);		// default size is specified within CMufManager implementation, guaranteed to be only 1 mbuf
	}
EXPORT_C RMBuf* RMBuf::AllocL(TUint aSize, RMBufAllocator& aRMBufAllocator)
 	{
	RMBuf *mBuf = aRMBufAllocator.MBufManager().Alloc(aSize, aSize);	// min. size constraint to ensure only 1 mbuf is allocated
	if (!mBuf)
		{
		User::Leave(KErrNoMBufs);
		}
	return mBuf;
	}
EXPORT_C RMBuf* RMBuf::Alloc(RMBufAllocator& aRMBufAllocator)
	{
	return aRMBufAllocator.MBufManager().Alloc(KMBufSmallSize);		// default size is specified within CMufManager implementation, guaranteed to be only 1 mbuf
	}
EXPORT_C RMBuf* RMBuf::Alloc(TUint aSize, RMBufAllocator& aRMBufAllocator)
	{
	return aRMBufAllocator.MBufManager().Alloc(aSize, aSize);	// min. size constraint to ensure only 1 mbuf is allocated
	}

EXPORT_C void RMBuf::Free()
/**
Frees the allocated memory for the RMBuf object
*/
	{

	// There is currently no support for reference counting, this check is to
	// ensure that someone hasn't mistakenly tried to use the iRefCount member.
	// The iRefCount member was introduced to allow reference counting to be
	// implemented at a later without another BC break having to be raised.
	__ASSERT_DEBUG(iRefCount==0, CMBufManager::Panic(EMBuf_InvalidRefCount));
	__ASSERT_DEBUG(iPoolChain != NULL, CMBufManager::Panic(EMBuf_NoPoolChain));
	((RMBufPoolChain*)iPoolChain)->MBufPoolManager().Free(this, EFalse);
	}


EXPORT_C RMBuf* RMBuf::Last()
/**
Last MBuf in a chain.
Not really supposed to be in RMBuf,
but putting it here allows the code to
be shared by RMBufChain and RMBufQ
@return A pointer to the last MBuf in the chain
*/
{
	RMBuf* p = NULL;
	TMBufIter m(this);

	while (m.More())
		{
		p = m;
		m++;
		}
	return p;
	}

EXPORT_C void RMBuf::__DbgPut(TUint8 aVal, TInt aOffset)
{
	__ASSERT_ALWAYS(aOffset<(iOffset+iLength), CMBufManager::Panic(EMBuf_BadOffset));
	*(Ptr()+aOffset) = aVal;
}

EXPORT_C TUint8 RMBuf::__DbgGet(TInt aOffset) const
{
	__ASSERT_ALWAYS(aOffset<(iOffset+iLength), CMBufManager::Panic(EMBuf_BadOffset));
	return *(Ptr()+aOffset);
}
