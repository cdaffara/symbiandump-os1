// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __COMMSBUFS_KERNEL_LIBRARY_INL__
#define __COMMSBUFS_KERNEL_LIBRARY_INL__


TInt DCommsBuf::Length() const
	{
	return iLength;
	}

void DCommsBuf::SetLength(TInt aLength)
	{
	__ASSERT_DEBUG((aLength >= 0) && ((aLength + iOffset) <= 0), KernCommsBuf::Fault(ECommsBufDimensionsOutOfBounds));
	iLength = aLength;
	}

TInt DCommsBuf::Offset() const
	{
	return iOffset - iRawDataOffset;
	}

void DCommsBuf::SetOffset(TInt aOffset)
	{
	__ASSERT_DEBUG((aOffset >= 0) && ((aOffset + iLength) <= iRawSize), KernCommsBuf::Fault(ECommsBufDimensionsOutOfBounds));
	iOffset = iRawDataOffset + aOffset;
	}
	
TUint8* DCommsBuf::Ptr() const
	{
	return iCommsBufPtr + iOffset;
	}

TAny* DCommsBuf::operator new(TUint /* aSize */, const TUint8* aPtr)
	{
	return (TUint8*)aPtr;
	}

void DCommsBuf::operator delete(TAny* /* aPtr */, const TUint8*)
	{
	}


#endif // __COMMSBUFS_KERNEL_LIBRARY_INL__

