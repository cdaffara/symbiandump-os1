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
// Packet Info Headers
// 
//

#include <nifmbuf.h>

EXPORT_C TMBufMark::TMBufMark()
	{
	iMBuf = NULL;
	iPtr = 0;
	iOffset = -1;
	}

EXPORT_C TMBufMark::TMBufMark(const TMBufMark& aParser)
	{
	iMBuf = aParser.iMBuf;
	iPtr = aParser.iPtr;
	iOffset = aParser.iOffset;
	}

EXPORT_C TMBufMark::TMBufMark(const RMBufChain& aChain, TInt aOffset/*=0*/)
	{
	Set(aChain, aOffset);
	}

EXPORT_C void TMBufMark::Set(const RMBufChain& aChain, TInt aOffset)
	{
	iOffset = aOffset;
	if (aOffset!=0)
		{
		TInt n, o;
		aChain.Goto(aOffset, iMBuf, o, n);
		iPtr = o-iMBuf->Offset();
		}
	else
		{
		iMBuf = (RMBuf*)aChain.First();
		iPtr = 0;
		}
	}

EXPORT_C void TMBufMark::Skip(TInt aLength)
//
// Skip over data
//
	{
	while (aLength>0 && iMBuf!=NULL)
		{
		TInt n = Min(iMBuf->Length()-iPtr, aLength);
		aLength -= n;		
		iOffset += n;
		iPtr += n;
		if (iPtr>iMBuf->Length())
			{
			iPtr = 0;
			iMBuf = iMBuf->Next();
			}
		}
	}

EXPORT_C void TMBufMark::Get(TDes8& aBuf)
//
// Get data into a descriptor
//
	{
	TInt len = aBuf.Length();
	TUint8* ptr = (TUint8*)aBuf.Ptr();
	
	while (len>0 && iMBuf!=NULL)
		{
		TInt n = Min(iMBuf->Length()-iPtr, len);
		Mem::Copy(ptr, iMBuf->Ptr()+iPtr, n);
		len -= n;		
		ptr += n;
		iOffset += n;
		iPtr += n;
		if (iPtr>iMBuf->Length())
			{
			iPtr = 0;
			iMBuf = iMBuf->Next();
			}
		}
	if (len>0)
		aBuf.SetLength(aBuf.Length()-len);
	}

EXPORT_C void TMBufMark::Get(TUint8* aPtr, TInt aLen)
//
// Get data into a buffer
//
	{
	while (aLen>0 && iMBuf!=NULL)
		{
		TInt n = Min(iMBuf->Length()-iPtr, aLen);
		Mem::Copy(aPtr, iMBuf->Ptr()+iPtr, n);
		aLen -= n;		
		aPtr += n;
		iOffset += n;
		iPtr += n;
		if (iPtr>iMBuf->Length())
			{
			iPtr = 0;
			iMBuf = iMBuf->Next();
			}
		}
	}
