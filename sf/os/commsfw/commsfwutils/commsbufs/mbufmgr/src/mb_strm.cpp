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
// MBuf Stream Queue

//
// MBuf Stream Queue
//

#include <nifmbuf.h>

//
// MBuf Stream Queues
//

EXPORT_C RMBufStreamQ::RMBufStreamQ()
/**
Constructor
*/
 	{
	iPosition = -1; iMBuf = NULL;
	}


EXPORT_C void RMBufStreamQ::CopyOut(TDes8& aDes)
/**
Copies a data from the chain to a TDes8 buffer
@param aDes the buffer where the data is copied to
*/
 	{
	AsRMBufChain().CopyOut(aDes);
	}

EXPORT_C void RMBufStreamQ::TrimStart(TInt aOffset)
/**
Trims the data upto offset 
@param aOffset the offset
*/
	{
	AsRMBufChain().TrimStart(aOffset);
	if (iNext==NULL)
		{
		iLast = NULL;
		iPosition = -1;
		}
	else
		{
		iPosition -= aOffset;
		}
	}

EXPORT_C void RMBufStreamQ::Append(RMBufChain& aPacket)
/**
Appends a MBuf chain to this stream
@param aPacket the MBuf chain
*/
 	{
	if (iPosition>=0 && iMBuf==NULL)
		{
		RMBuf* m = static_cast<RMBuf*>(iLast);
		RMBufQ::Append(aPacket);
		if (m!=NULL && (m = m->Next(), m!=NULL))
			{
			iMBuf = m;
			iOffset = m->Offset();
			iLength = m->Length();
			}
		}
	else
		RMBufQ::Append(aPacket);
	}

EXPORT_C void RMBufStreamQ::CopySegmentL(RMBufChain& aNewChain, TInt aOffset, TInt aLen/*, const TDesC8* aDes*/)
/**
Copies a segment to a MBuf chain
Functionaly the similar to RMBufChain::CopyL, except that cache pointer
information is updated to optimise the common case of each extraction
following on from the previous call.
@param aNewChain the chain to be created
@param aOffset the offset
@param aLen the length
*/
	{
	TInt n, n1, n2;
	TUint8* p1, *p2;
	RMBuf* m1, *m2;

	// Locate offset into stream queue
	if (aOffset>0)
		{
		if (aOffset!=iPosition || iMBuf==NULL)
			{
			TInt o = aOffset;
			RMBuf* m = static_cast<RMBuf*>(iNext);
			while (m!=NULL && o>=m->Length())
				{
				o -= m->Length();
				m = m->Next();
				}
			
			if (!m)
				{
				User::Leave(KErrOverflow);
				}
			iPosition = aOffset;
			iMBuf = m;
			iOffset = o + m->Offset();
			iLength = m->Length() - o;
			}
		
		m1 = iMBuf;		
		n = iOffset;		
		n1 = iLength;
		p1 = m1->Buffer()+n;
		}
	else
		{
		m1 = static_cast<RMBuf*>(iNext);
		p1 = m1->Ptr();
		n1 = m1->Length();
		}

	TInt len = aLen;
	aNewChain.AllocL(len);
	m2 = aNewChain.First();

	p2 = m2->Ptr();
	n2 = m2->Length();
	
	while (len>0)
		{
		n = n1 < n2 ? n1 : n2;
		Mem::Copy(p2, p1, n);
		if (n1 -= n, n1 == 0)
			{
			if (m1 = m1->Next(), m1==NULL)
				break;
			p1 = m1->Ptr();
			n1 = m1->Length();
			}
		else
			p1 += n;

		if (n2 -= n, n2 == 0)
			{
			if (m2 = m2->Next(), m2==NULL)
				break;
			p2 = m2->Ptr();
			n2 = m2->Length();
			}
		else
			p2 += n;

		len -= n;
		}

	if (m1)
		{
		iPosition = aOffset+aLen;	
		iMBuf = m1;
		iOffset = p1-m1->Buffer();
		iLength = n1;
		}
	else
		{
		iPosition = aOffset+aLen;	
		iMBuf = NULL;	
		}
	}
