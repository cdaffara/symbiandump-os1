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
#include "commsbufpondintf.h"
#include "commsbufpool.h"
	
EXPORT_C RCommsBuf* RCommsBuf::Alloc(TUint aSize, TCommsBufAllocator& aAllocator)
/**
Allocates memory for a RCommsBuf object. 

@param aSize Requested total size
@param aAllocator A handle to the allocator

@return RCommsBuf	A pointer to the allocated RCommsBuf object if allocation is success
					or NULL if allocation is failed
*/
	{
	return aAllocator.iPond.Alloc(aSize, aSize, KMaxTInt);	
	}

EXPORT_C void RCommsBuf::Append(const TDesC8& aSrc)
/**
Append data onto the end of RCommsBuf data. The length of the
CommsBuf data is incremented to reflect the new content

@param aSrc Data to append
*/
	{
	__ASSERT_COMMSBUF();
	__ASSERT_ALWAYS(AppendLimit() >= aSrc.Length(), CommsBuf::Panic(EMBuf_IndexOutofRange));	
	Mem::Copy(const_cast<TUint8*>(Ptr()) + iCommsBufMetaData.iLength, aSrc.Ptr(), aSrc.Length());
	AdjustDataEnd(aSrc.Length());
	}

EXPORT_C void RCommsBuf::Prepend(const TDesC8& aSrc)
/**
Prepend data onto the beginning of RCommsBuf data. The length and offset of the
will be adjusted to reflect the new content

@param aSrc Data to append
*/
	{
	__ASSERT_COMMSBUF();
    __ASSERT_ALWAYS(PrependLimit() >= aSrc.Length(), CommsBuf::Panic(EMBuf_IndexOutofRange));	
	Mem::Copy(const_cast<TUint8*>(Ptr()) - aSrc.Length(), aSrc.Ptr(), aSrc.Length());
	SetDataRange(Offset() - aSrc.Length(), iCommsBufMetaData.iLength + aSrc.Length());
	}

EXPORT_C void RCommsBuf::Free()
/**
Frees the RCommsBuf
*/
	{
	Pool()->Pond().Free(this);
	}

EXPORT_C void RCommsBuf::Write(const TDesC8& aSrc, TInt aOffset /* =0 */)
/**
The supplied descriptor will be written to the specified offset within the RCommsBuf data

@param aSrc		Data to be written
@param aOffset	Offset within the CommsBuf

@panic MBuf::11 If the given offset is negative
*/
	{
	__ASSERT_COMMSBUF();
    __ASSERT_ALWAYS(aOffset>=0, CommsBuf::Panic(EMBuf_NegativeOffset));
    SetDataRange(aOffset, 0); // Set the write offset and make the length as 0
  	Append(aSrc);	// Append the data
	}

EXPORT_C void RCommsBuf::Read(TDes8& aDest, TInt aOffset /* =0 */) const	
/**
The supplied descriptor will be read from the specified offset within the RCommsBuf data

@param aDes		Descriptor to write
@param aOffset 	The offset

@panic MBuf::11 If the given offset is negative or greater than the length of the CommsBuf data
*/
	{
	__ASSERT_COMMSBUF();
    __ASSERT_ALWAYS(aOffset>=0 && aOffset<Length(), CommsBuf::Panic(EMBuf_BadOffset));
	TPtr8 src(const_cast<TUint8*>(Ptr()) + aOffset, Length() - aOffset, Length() - aOffset);
	aDest.Copy(src);
	}

