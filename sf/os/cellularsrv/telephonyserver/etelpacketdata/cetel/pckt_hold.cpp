// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Packet_HOLD.CPP
// ETel Multimode Packet Data API 
// GPRS Rel97/98, CDMAOne, CDMA2000, Rel99 and Rel4.
// 
//

#include <etelext.h>

// ETel Packet data header files
#include "pcktptr.h"

CEtelPacketPtrHolder::CEtelPacketPtrHolder()
	{}

CEtelPacketPtrHolder::~CEtelPacketPtrHolder()
/**
 * Destructor
 */
	{
	iPtrArray.Close();
	iPtrCArray.Close();
	}

void CEtelPacketPtrHolder::ConstructL(const TInt aSizeOfPtrArray,TInt aSizeOfPtrCArray)
/**
 * ConstructL() method - Initializes the array of pointers.
 */
	{
	TPtr8 ptr(NULL,0);
	TInt i;
	for (i=0;i<aSizeOfPtrArray;i++)
		User::LeaveIfError(iPtrArray.Append(ptr));

	TPtrC8 ptrC(NULL,0);
	for (i = 0; i < aSizeOfPtrCArray; i++)
		{
		User::LeaveIfError(iPtrCArray.Append(ptrC));
		}		
	}

TPtr8& CEtelPacketPtrHolder::Ptr(const TInt aIndex)
/**
 * This method checks that the array index is valid (i.e. within range). 
 * If the index is invalid, the client is panic'ed, otherwise the location of the array
 * element is returned.
 */
	{
	__ASSERT_ALWAYS(aIndex<iPtrArray.Count(),PanicClient(EEtelPanicIndexOutOfRange));
	return iPtrArray[aIndex];
	}
	
TPtrC8& CEtelPacketPtrHolder::PtrC(TInt aIndex)
/**
 * This method returns a reference to the TPtrC8 at aIndex within the array
 *
 * @param aIndex The array location of the desired TPtrC8
 * @return TPtrC8&
 */
	{
	__ASSERT_ALWAYS(aIndex<iPtrCArray.Count(),PanicClient(EEtelPanicIndexOutOfRange));
	return iPtrCArray[aIndex];
	}

CPacketPtrHolder* CPacketPtrHolder::NewL(const TInt aSizeOfPtrArray, TInt aSizeOfPtrCArray)
/**
 * Two phase constructor.
 *
 * @param aSizeOfPtrArray Size of the array containing the pointers.
 */
	{
	CPacketPtrHolder* p = new (ELeave) CPacketPtrHolder();
	CleanupStack::PushL(p);
	p->ConstructL(aSizeOfPtrArray,aSizeOfPtrCArray);
	CleanupStack::Pop();
	return p;
	}

CPacketPtrHolder::CPacketPtrHolder()
	:CEtelPacketPtrHolder(),
	iStoreMbmsPtr(NULL,0,0)
	{}

CPacketPtrHolder::~CPacketPtrHolder()
	{
	delete iStoreMbmsBuf;
	}

CPacketContextPtrHolder* CPacketContextPtrHolder::NewL(const TInt aSizeOfPtrArray,TInt aSizeOfPtrCArray)
/**
 * Two phase constructor.
 *
 * @param aSizeOfPtrArray Size of the array containing the pointers.
 */
	{
	CPacketContextPtrHolder* p = new (ELeave) CPacketContextPtrHolder();
	CleanupStack::PushL(p);
	p->ConstructL(aSizeOfPtrArray,aSizeOfPtrCArray);
	CleanupStack::Pop();
	return p;
	}

CPacketContextPtrHolder::CPacketContextPtrHolder()
	:CEtelPacketPtrHolder(),
	iStoreNetworkPtr(NULL,0,0)
	{}

CPacketContextPtrHolder::~CPacketContextPtrHolder()
	{
	delete iStoreNetworkBuf;
	}

CPacketQoSPtrHolder* CPacketQoSPtrHolder::NewL(const TInt aSizeOfPtrArray, TInt aSizeOfPtrCArray)
/**
 * Two phase constructor.
 *
 * @param aSizeOfPtrArray Size of the array containing the pointers.
 */
	{
	CPacketQoSPtrHolder* p = new (ELeave) CPacketQoSPtrHolder();
	CleanupStack::PushL(p);
	p->ConstructL(aSizeOfPtrArray,aSizeOfPtrCArray);
	CleanupStack::Pop();
	return p;
	}

CPacketQoSPtrHolder::CPacketQoSPtrHolder()
	:CEtelPacketPtrHolder()
	{}

CPacketQoSPtrHolder::~CPacketQoSPtrHolder()
	{}


