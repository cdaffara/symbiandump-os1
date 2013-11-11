// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "simplepairingresult.h"

#include <bluetooth/logger.h>
#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_PAIRING_SERVER);
#endif



CSimplePairingResultList* CSimplePairingResultList::NewL()
	{
	LOG_STATIC_FUNC
	CSimplePairingResultList* self = new(ELeave) CSimplePairingResultList();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CSimplePairingResultList::CSimplePairingResultList()
	: CBluetoothSecurityResultList(KSimplePairingResultListMaxSize)
	{
	LOG_FUNC
	}

CAuthenticationResultList* CAuthenticationResultList::NewL()
	{
	LOG_STATIC_FUNC
	CAuthenticationResultList* self = new(ELeave) CAuthenticationResultList();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CAuthenticationResultList::CAuthenticationResultList()
	: CBluetoothSecurityResultList(KAuthenticationResultListMaxSize)
	{
	LOG_FUNC
	}

CBluetoothSecurityResultList::CBluetoothSecurityResultList(TInt aMaxListCount)
	: iResultList(_FOFF(TBluetoothSecurityResult, iLink))
	, iListCount(0)
	, iMaxListCount(aMaxListCount)
	{
	LOG_FUNC
	}

void CBluetoothSecurityResultList::ConstructL()
	{
	LOG_FUNC
	}

CBluetoothSecurityResultList::~CBluetoothSecurityResultList()
	{
	LOG_FUNC
	while(!iResultList.IsEmpty())
		{
		TBluetoothSecurityResult* entry = iResultList.First();
		entry->iLink.Deque();
		delete entry;
		}
	}

void CBluetoothSecurityResultList::NewResult(const TBTDevAddr& aDevAddr, TInt aResult)
	{
	LOG_FUNC
	if(iObserver)
		{
		// Add the new entry.
		TBluetoothSecurityResult* entry = new TBluetoothSecurityResult(aDevAddr, aResult);
		if(!entry)
			{
			// OOM so we've lost a result - oh well we did our best.
			return;
			}
		iResultList.AddLast(*entry);
		++iListCount;

		// Trim the list if needed
		while(iListCount > iMaxListCount)
			{
			entry = iResultList.First();
			entry->iLink.Deque();
			--iListCount;
			delete entry;
			}

		// Complete any outstanding notifications.
		if(iOutstandingNotification)
			{
			iOutstandingNotification = EFalse;
			ReturnResult();
			}
		}

	}

void CBluetoothSecurityResultList::RegisterObserverL(MBluetoothSecurityResultObserver& aObserver)
	{
	LOG_FUNC
	if(iObserver)
		{
		LEAVEL(KErrAlreadyExists);
		}
	iObserver = &aObserver;
	}

void CBluetoothSecurityResultList::ReleaseObserver(MBluetoothSecurityResultObserver& aObserver)
	{
	LOG_FUNC
	if(iObserver == &aObserver)
		{
		iObserver = NULL;
		//Clear the list so that any queued results are not returned to the observer during the next session
		//(not doing this can cause results for a previous pairing request to be returned to a client-side app during the next pairing request).
		while (!iResultList.IsEmpty())
			{
			TBluetoothSecurityResult* entry = iResultList.First();
			entry->iLink.Deque();
			delete entry;
			}

		//Set iListCount to zero.
		iListCount=0;
		}
	}

void CBluetoothSecurityResultList::ReturnResult()
	{
	LOG_FUNC
	if(iListCount > 0)
		{
		__ASSERT_DEBUG(!iResultList.IsEmpty(), PANIC(KBluetoothSecListFaultCat, EResultListCountMismatch));
		__ASSERT_DEBUG(iObserver, PANIC(KBluetoothSecListFaultCat, ENoBoundObserver));
		TBluetoothSecurityResult* entry = iResultList.First();
		__ASSERT_DEBUG(entry, PANIC(KBluetoothSecListFaultCat, ENullResultListEntry));
		TInt err = iObserver->MbsroResult(entry->DeviceAddress(), entry->Result());
		if(err == KErrNone)
			{
			entry->iLink.Deque();
			--iListCount;
			delete entry;
			}
		}
	else
		{
		__ASSERT_DEBUG(!iOutstandingNotification, PANIC(KBluetoothSecListFaultCat, ENotificationAlreadyOutstanding));
		iOutstandingNotification = ETrue;
		}
	}

void CBluetoothSecurityResultList::CancelReturn()
	{
	LOG_FUNC
	iOutstandingNotification = EFalse;
	}


//
// TBluetoothSecurityResult
//


TBluetoothSecurityResult::TBluetoothSecurityResult(const TBTDevAddr& aDevAddr, TInt aResult)
	: iDeviceAddress(aDevAddr)
	, iResult(aResult)
	{
	LOG_FUNC
	}

const TBTDevAddr& TBluetoothSecurityResult::DeviceAddress() const
	{
	LOG_FUNC
	return iDeviceAddress;
	}

TInt TBluetoothSecurityResult::Result() const
	{
	LOG_FUNC
	return iResult;
	}

