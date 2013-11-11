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

Buffer Manager for Protocols(Buffer)

@internalComponent
*/
#include <es_mbuf.h>
#include <comms-infras/commsbufpanic.h>

//
// MBUF
//

EXPORT_C void RMBuf::Init(TMBufType aType)
/**
Initialises MBuf's members
@param aType A MBuf type
*/
	{
	iCommsBufMetaData.iType = aType;
	iCommsBufMetaData.iLength = iCommsBufMetaData.iRawSize;
	iCommsBufMetaData.iOffset = iCommsBufMetaData.iRawDataOffset;
	iCommsBufMetaData.iNext = NULL;
	(static_cast<RMBufChain&>(iCommsBufMetaData.iNextPkt)).Init();
	}

EXPORT_C void RMBuf::Free()
	{
	RCommsBuf::Free();		
	}

EXPORT_C RMBuf* RMBuf::AllocL()
/**
Allocates memory for a RMBuf object - the size of the mbuf to be allocate will depend on the available pool 
chains, eg. 128, 256 etc...

@return A pointer to the RMBuf 
@leave KErrNoMemory On allocation failure
*/
 	{
	RMBufAllocator allocator;
	return AllocL(allocator);
	}

EXPORT_C RMBuf* RMBuf::AllocL(TUint aSize)
/**
Allocates memory for a RMBuf object. Multiple RMBuf's maybe allocated internally if the
requested size is greater than the maximum size of the RMBuf on the available pool chains.

@param aSize The requested size for the RMBuf

@return A pointer to the RMBuf 
@leave KErrNoMemory On allocation failure
*/
 	{
 	RMBufAllocator allocator;
 	return AllocL(aSize, allocator);
	}
	
EXPORT_C RMBuf* RMBuf::Alloc()
/**
Allocates memory for a RMBuf object - the size of the mbuf to be allocate will depend on the available pool 
chains, eg. 128, 256 etc...

@return A pointer to the RMBuf or NULL on allocation failure
*/
 	{
 	RMBufAllocator allocator;
	return Alloc(allocator);
	}
	
EXPORT_C RMBuf* RMBuf::Alloc(TUint aSize)
/**
Allocates memory for a RMBuf object. Multiple RMBuf's maybe allocated internally if the
requested size is greater than the maximum size of the RMBuf on the available pool chains.

@param aSize The requested size for the RMBuf

@return A pointer to the RMBuf or NULL on allocation failure
*/
 	{
 	RMBufAllocator allocator;
	return Alloc(aSize, allocator);
	}

EXPORT_C RMBuf* RMBuf::AllocL(RMBufAllocator& aRMBufAllocator)
/**
Allocates memory for a RMBuf object using a given RMBufAllocator. Multiple RMBuf's maybe allocated internally if the
requested size is greater than the maximum size of the RMBuf on the available pool chains.

@see RMBufAllocator

@param aRMBufAllocator RMBufAllocator object

@return A pointer to the RMBuf
@leave KErrNoMemory On allocation failure
*/
 	{
 	return AllocL(KMBufSmallSize, aRMBufAllocator);
	}
	
EXPORT_C RMBuf* RMBuf::AllocL(TUint aSize, RMBufAllocator& aRMBufAllocator)
 /**
Allocates memory for a RMBuf object using a given RMBufAllocator and a given size. Multiple RMBuf's maybe allocated internally if the
requested size is greater than the maximum size of the RMBuf on the available pool chains.

@see RMBufAllocator

@param aSize The requested size for the RMBuf
@param aRMBufAllocator RMBufAllocator object

@return A pointer to the RMBuf
@leave KErrNoMemory On allocation failure
*/
 	{
 	RMBuf* buf = static_cast<RMBuf*>(RCommsBuf::Alloc(aSize, aRMBufAllocator));

	if (!buf)
	 	{
		User::Leave(KErrNoMBufs);	 		
	 	}
	return buf;		 	
	}
	
EXPORT_C RMBuf* RMBuf::Alloc(RMBufAllocator& aRMBufAllocator)
/**
Allocates memory for a RMBuf object using a given RMBufAllocator. Multiple RMBuf's maybe allocated internally if the
requested size is greater than the maximum size of the RMBuf on the available pool chains.

@see RMBufAllocator

@param aRMBufAllocator RMBufAllocator object

@return A pointer to the RMBuf or NULL on allocation failure
*/
	{
	return static_cast<RMBuf*>(RCommsBuf::Alloc(KMBufSmallSize, aRMBufAllocator));
	}

EXPORT_C RMBuf* RMBuf::Alloc(TUint aSize, RMBufAllocator& aRMBufAllocator)
 /**
Allocates memory for a RMBuf object using a given RMBufAllocator and a given size. Multiple RMBuf's maybe allocated internally if the
requested size is greater than the maximum size of the RMBuf on the available pool chains.

@see RMBufAllocator

@param aSize The requested size for the RMBuf
@param aRMBufAllocator RMBufAllocator object

@return A pointer to the RMBuf or NULL on allocation failure
*/
	{
	return static_cast<RMBuf*>(RCommsBuf::Alloc(aSize, aRMBufAllocator));
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
	__ASSERT_ALWAYS((aOffset >= 0) && (aOffset <= iCommsBufMetaData.iLength), CommsBuf::Panic(EMBuf_BadOffset));
	*(Ptr()+aOffset) = aVal;
	}

EXPORT_C TUint8 RMBuf::__DbgGet(TInt aOffset) const
	{
	__ASSERT_ALWAYS((aOffset >= 0) && (aOffset <= iCommsBufMetaData.iLength), CommsBuf::Panic(EMBuf_BadOffset));
	return *(Ptr()+aOffset);
	}
