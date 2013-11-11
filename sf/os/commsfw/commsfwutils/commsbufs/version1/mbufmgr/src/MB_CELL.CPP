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
// C++ storage support
// 
//

#include <nifmbuf.h>

//
// MBuf Packet Information Base
//

EXPORT_C RMBufCell* RMBufCell::CopyL()
/**
Pseudo Copy Constructor	
- refer RMBufChain::AllocL notes regarding the deliberate decision not to provide an overloaded min/max mbuf size variant
*/
	{
	RMBuf* mbuf = RMBuf::AllocL(iThisMBuf->Size());

	Mem::FillZ(mbuf->Buffer() + iThisMBuf->Length(), mbuf->Size() - iThisMBuf->Length());	// trs; wasteful to fill memory, but removing this constitutes a functional break, hence it has been kept as is
	mbuf->SetType(EMBufHeader);			// trs; internally only used for UDEB so could in theory be compiled out for UREL, but left as is to avoid a functional break
	
	// memory is copied to the length specified by the destination (instead of the source) because;
	// a. avoid overrun if source is larger than the destination
	// b. avoid FC break whereby legacy code derives from an RMBufCell object, but does not request the correct size during allocation
	//    eg. tcpip6 wrongly typecasts RMBufPktInfo object to RMBufRecvInfo & assumes the mbuf is large enough for any additional members
	//        - this works (but is still a very poor assumption) if mbufs are of K_MBufSmallSize, but fails when copying between 
	//			RMBufCell's unless the entire mbuf is copied (ie. not just the length specified iLength)
	Mem::Copy(mbuf->Buffer(), iThisMBuf->Buffer(), mbuf->Size());	// trs; ideally length should length min(iThisMBuf->Length(), mbuf->Size())
	
	RMBufCell* hdr = (RMBufCell*)mbuf->Ptr();
	hdr->iThisMBuf = mbuf;
	return hdr;
	}


EXPORT_C TAny* RMBufCell::operator new(TUint aSize, TLeave /*aLeave*/, TUint aExtra)
/**
new operator.
- refer RMBufChain::AllocL notes regarding the deliberate decision not to provide an overloaded min/max mbuf size variant
@param aSize A size of the cell
@param aExtra Extra size
*/
	{
	aSize += aExtra;

	// for backward compatibility (refer es_mbuf.h notes) cells are assumed to always be allocated within an K_MBufSmallSize sized mbuf
	// - removing this assumption requires a SC break on consumers that (wrongly) assume that cells are always of this size
	// - this assumption is unforunate as it restricts cells to no larger (or smaller) than this size
	__ASSERT_ALWAYS(aSize <= (TUint)KMBufSmallSize, MBufExtPanic(EMbExtPanic_MBufAllocTooBig));

	RMBuf* mbuf = RMBuf::AllocL(aSize); 
	mbuf->SetType(EMBufHeader);			// trs; internally only used for UDEB so could in theory be compiled out for UREL, but left as is to avoid a functional break
	mbuf->FillZ();						// trs; wasteful to fill memory, but removing this constitutes a functional break, hence it has been kept as is

	RMBufCell* hdr = (RMBufCell*)mbuf->Ptr();
	hdr->iThisMBuf = mbuf;
	return hdr;
	}


EXPORT_C TAny* RMBufCell::operator new(TUint aSize, TUint aExtra)  __NO_THROW
/**
new operator.
- refer RMBufChain::AllocL notes regarding the deliberate decision not to provide an overloaded min/max mbuf size variant
@param aSize A size of the cell
@param aExtra Extra size
*/
	{
	aSize += aExtra;

	// cells are assumed to always be allocated within an K_MBufSmallSize sized mbuf, refer ::new() throw notes
	__ASSERT_ALWAYS(aSize <= (TUint)KMBufSmallSize, MBufExtPanic(EMbExtPanic_MBufAllocTooBig));
	
	RMBuf* mbuf = RMBuf::Alloc(aSize);
	if(!mbuf)
		{
		return mbuf;
		}
	mbuf->SetType(EMBufHeader);	// trs; internally only used for UDEB so could in theory be compiled out for UREL, but left as is to avoid a functional break
	mbuf->FillZ();				// trs; wasteful to fill memory, but removing this constitutes a functional break, hence it has been kept as is

	RMBufCell* hdr = (RMBufCell*)mbuf->Ptr();
	hdr->iThisMBuf = mbuf;
	return hdr;
	}


EXPORT_C void RMBufCell::operator delete(TAny* aPtr)
/**
delete operator
deletes the cell
*/
 	{
	((RMBufCell*)aPtr)->iThisMBuf->Free();
	}


void RMBufCell::operator delete(TAny* aPtr, TLeave, TUint)
/**
extra delete operator for exception unwinding in Code Warrior
*/
	{
	RMBufCell::operator delete(aPtr);
	}

void RMBufCell::operator delete(TAny* aPtr, TUint)
/**
extra delete operator for exception unwinding in Code Warrior
*/
	{
	RMBufCell::operator delete(aPtr);
	}




