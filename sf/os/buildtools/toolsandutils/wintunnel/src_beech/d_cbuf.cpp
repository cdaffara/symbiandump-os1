// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "d_comm.h"

CCommTxBuf::CCommTxBuf()
	: CCirBuffer()
	{}


TInt CCommTxBuf::ClientRead(DComm* aDriver, DThread* aThread, const TAny* aPtr, TInt& aTxLength, TInt& aTxOffset, TInt aMode)
//
// Copy from client to buffer.
//
	{
	TInt offset = aTxOffset;
	TInt length = aTxLength;
	if (length==0)
		return KErrNone;

	TPtr8 des(NULL,0);

	aDriver->DisableInterrupts();
	TInt count = iCount;
	TInt space = iLength-count;
	if (space==0)
		{
		aDriver->EnableInterrupts();
		return KErrOverflow;
		}
	TInt maxcopy = Min(space,length);
	aDriver->EnableInterrupts();
	
	TInt copylen = Min(maxcopy, iPtrE-iHead);
	des.Set(iHead, copylen, copylen);
	aThread->Read(aPtr, &des, offset, aMode);
	length -= copylen;
	offset += copylen;

	copylen = maxcopy-copylen;
	if (copylen>0)
		{
		des.Set(iPtr, copylen, copylen);
		aThread->Read(aPtr, &des, offset, aMode);
		length -= copylen;
		offset += copylen;
		}

	iHead += maxcopy;
	if (iHead>=iPtrE)
		iHead -= iLength;

	aDriver->DisableInterrupts();
	iCount += maxcopy;
	aDriver->EnableInterrupts();

	aTxLength = length;
	aTxOffset = offset;
	return KErrNone;
	}


CCommRxBuf::CCommRxBuf()
//
//
//
	{
	__DECLARE_NAME(_S("CCommRxBuf"));
	}

CCommRxBuf::~CCommRxBuf()
//
//
//
	{
	User::Free(iCharPtr);
	}

void CCommRxBuf::SetLengthL(TInt aLength)
//
// ReAlloc - Resets all the buffer pointers
//
	{
	__ASSERT_ALWAYS(aLength>0,User::Panic(_L("DCOMBUF"),0/*ECircSetLengthNegative*/));
	aLength = Align4(aLength);
	iCharPtr=(TUint8 *)User::ReAllocL(iCharPtr, aLength<<1);
	iLength = aLength;
	iInfoPtr = iCharPtr+aLength;
	Reset();
	}

void CCommRxBuf::Reset()
//
// Reset the buffer pointers
//
	{
	iCount = 0;
	iInsP = 0;
	iRemP = 0;
	iInsSeqNum = 0;
	iRemSeqNum = iInsSeqNum;
	iLastSeqNum = iInsSeqNum;
	}


TInt CCommRxBuf::ClientWrite(TUint& aStatus, DComm* aDriver, DThread* aThread, const TAny* aPtr, TInt& aRxLength, TInt& aRxOffset, TInt aMode)
//
// Copy to client's buffer
// if aCount==0 then do as much as possible, and only return completed
// if the clients buffer is full. Normally, aCount would be the count
// returned by GetInfo().
//
	{
	if (iCount==0)
		return KErrUnderflow;

	TInt blocksz;
	TUint state = iInfoPtr[iRemP];

	TInt p;
	if ((state & 0x0f)==0x0f)
		{
		p = (iRemP+0x4)&~0x3;
		if (p>=iLength)
			p = 0;
		blocksz = *((TInt*)(iInfoPtr+p));
		}
	else
		blocksz = state & 0x0f;

	p = iRemP+blocksz;
	if (blocksz>0)
		p -= 1;
	if (p>=iLength)
		p-=iLength;
	aStatus = (iInfoPtr[p] << KReceiveIsrShift) & KReceiveIsrMaskComplete;

	TInt length = aRxLength;
	TInt offset = aRxOffset;

	if (blocksz>0 && blocksz<length)
		length = blocksz;

	aDriver->DisableInterrupts();
	TInt count = iCount;
	// The number of bytes will will extract in total
	TInt maxcopy = Min(length, count);
	// Pre-empt the recv int handler so that the sequence
	// spaces dont clash
	iRemSeqNum += maxcopy;
	if (iRemSeqNum > iLastSeqNum)
		iLastSeqNum = iRemSeqNum;
	aDriver->EnableInterrupts();
	
	TInt copylen;
	TPtrC8 des;
	copylen = Min(maxcopy, iLength-iRemP);
	des.Set(iCharPtr+iRemP, copylen);
	aThread->Write(aPtr, &des, offset, aMode);
	offset += copylen;
	length -= copylen;
	count -= copylen;	
	
	copylen = Min(length, count);
	if (copylen>0)
		{
		des.Set(iCharPtr, copylen);
		aThread->Write(aPtr, &des, offset, aMode);
		length -= copylen;
		count -= copylen;
		offset += copylen;
		}

	aDriver->DisableInterrupts();
	iRemP += maxcopy;

	if (iRemP >= iLength)
		iRemP -= iLength;

	iCount -= maxcopy;

	aDriver->EnableInterrupts();

	if (blocksz>maxcopy)
		{
		TInt ptr = iRemP;
		TInt count = blocksz-maxcopy;
		if (count<0x0f)
			iInfoPtr[ptr] = (TUint8)((iInfoPtr[ptr] & 0xf0) | count);
		else
			{
			iInfoPtr[ptr] = (TUint8)((iInfoPtr[ptr] & 0xf0) | 0x0f);
			// Word align buffer offset and wrap if needed
			// so a full count can be written
			ptr = (ptr+0x4)&~0x3;
			if (ptr>=iLength)
				ptr = 0;
			*((TInt*)(iInfoPtr+ptr)) = count;
			}
		}
		
	aRxOffset = offset;
	aRxLength = length;
	
	if (blocksz>0)
		return (blocksz-maxcopy)==0 || aRxLength==0;

	return aRxLength==0;
	}


TInt CCommRxBuf::PutChar(TUint aChar)
//
// Add char to receive buffer
// Return true if this character should complete
// a Read().
//
	{
	if (iCount>=iLength)
		return KErrOverflow;

	iCharPtr[iInsP] = (TUint8)(aChar & 0xff);
	iInfoPtr[iInsP] = (TUint8)(aChar >> KReceiveIsrShift);
	if (++iInsP >= iLength)
		iInsP = 0;
	++iCount;
	++iInsSeqNum; // Pre-Inc is more efficient for a C++ object

	// Check if we have to write a new event offset into
	// the info buffer
	if (aChar & KReceiveIsrMaskComplete)
		{
		TInt count = iInsSeqNum - iLastSeqNum;
		TInt ptr = iInsP-count;
		if (ptr<0)
			ptr += iLength;
		
		if (count<0x0f)
			iInfoPtr[ptr] = (TUint8)((iInfoPtr[ptr] & 0xf0) | count);
		else
			{
			iInfoPtr[ptr] = (TUint8)((iInfoPtr[ptr] & 0xf0) | 0x0f);
			// Word align buffer offset and wrap if needed
			// so a full count can be written
			ptr = (ptr+0x4)&~0x3;
			if (ptr>=iLength)
				ptr = 0;
			*((TInt*)(iInfoPtr+ptr)) = count;
			}
		iLastSeqNum = iInsSeqNum;
		return 1;
		}
		
	return 0;
	}


TBool CCommRxBuf::RescanTerminators(DComm* aDriver, TInt aTermCount, TText8* aTermChars)
	{
	aDriver->DisableInterrupts();

	TCommSeqNum	_rem = iRemSeqNum;
	
	// 1. Move last event point to the current insert pointer
	// This protects from interrupts
	iLastSeqNum = iInsSeqNum;
	TCommSeqNum endseq = iLastSeqNum;
	
	// 2. Remove all count information, and terminator flags

	TInt ptr = iRemP;	
	TCommSeqNum seq = iRemSeqNum;
	
	while (seq<endseq)
		{
		TInt p = ptr;
		TInt len;
		TUint s = iInfoPtr[p];

		if (s==0)
			len = endseq-seq;
		else if ((s & 0x0f)==0x0f)
			{
			p = (ptr+0x4)&~0x3;
			if (p>=iLength)
				p = 0;
			len = *((TInt*)(iInfoPtr+p));
			*((TInt*)(iInfoPtr+p)) = 0;
			}
		else
			{
			iInfoPtr[p] &= 0xf0;
			len = s & 0x0f;
			}
		
		ptr += len;
		p = ptr;

		if (len>0)
			p -= 1;
		if (p>=iLength)
			p-=iLength;

		iInfoPtr[p] &= ~(KReceiveIsrTermChar>>KReceiveIsrShift);
		
		if (ptr>=iLength)
			ptr -= iLength;
		seq += len;				
		}

	// 3. Now create a new set of counts and events exactly as if inserting the
	// characters for the first time.

	TBool complete = EFalse;
	TInt lstptr;
	TCommSeqNum lstseq;

	ptr = iRemP;	
	seq = iRemSeqNum;

	lstseq = seq;
	lstptr = ptr;

	while (seq<endseq)
		{
		++seq;
		TText8 c = iCharPtr[ptr];
		for (TInt i=0; i<aTermCount; i++)
			{
			if (c==aTermChars[i])
				{
				iInfoPtr[ptr] |= (KReceiveIsrTermChar>>KReceiveIsrShift);
				break;
				}
			}

		if (iInfoPtr[ptr] & (KReceiveIsrMaskComplete>>KReceiveIsrShift))
			{
			TInt len = seq-lstseq;
			if (len<0x0f)
				iInfoPtr[lstptr] = (TUint8)((iInfoPtr[lstptr] & 0xf0) | len);
			else
				{
				iInfoPtr[lstptr] = (TUint8)((iInfoPtr[lstptr] & 0xf0) | 0x0f);
				// Word align buffer offset and wrap if needed
				// so a full count can be written
				TInt p = (lstptr+0x4)&~0x3;
				if (p>=iLength)
					p = 0;
				*((TInt*)(iInfoPtr+p)) = len;
				}
			lstptr = ptr+1;
			if (lstptr>=iLength)
				lstptr -= iLength;
			lstseq = seq;
			complete = ETrue;
			}
		if (++ptr>=iLength)
			ptr -= iLength;
		}

	// 4. Tie into new location of event seq pointer that may have been moved
	//    as a result of receive interrupts inserting new events
	if (iLastSeqNum==endseq)
		{
		// No interrupts have occurred that resulted in a new event.
		iLastSeqNum = lstseq;
		}
	else
		{
		// New event was generated by the interrupt handler
		// So extract and zero the inserted count
		TInt p = ptr;
		TInt len;
		TUint s = iInfoPtr[p];

		if (s==0)
			len = endseq-seq;
		else if ((s & 0x0f)==0x0f)
			{
			p = (ptr+0x4)&~0x3;
			if (p>=iLength)
				p = 0;
			len = *((TInt*)(iInfoPtr+p));
			*((TInt*)(iInfoPtr+p)) = 0;
			}
		else
			{
			iInfoPtr[p] &= 0xf0;
			len = s & 0x0f;
			}
		// And in the offset for the last event in the re-scan
		len += seq-lstseq;
		// Insert the count into the re-scanned section
		if (len<0x0f)
			iInfoPtr[lstptr] = (TUint8)((iInfoPtr[lstptr] & 0xf0) | len);
		else
			{
			iInfoPtr[lstptr] = (TUint8)((iInfoPtr[lstptr] & 0xf0) | 0x0f);
			// Word align buffer offset and wrap if needed
			// so a full count can be written
			p = (lstptr+0x4)&~0x3;
			if (p>=iLength)
				p = 0;
			*((TInt*)(iInfoPtr+p)) = len;
			}
		complete = ETrue;
		}

	aDriver->EnableInterrupts();
	return complete;
	}
