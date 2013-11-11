// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "CGetLockInfo.h"

/**
Factory constructor.

@param  aController Pointer to MExecAsync object passed to constructor of 
                    CISVAPIBase
@return             Instance of CGetLockInfo class
*/
CGetLockInfo* CGetLockInfo::NewL(MExecAsync* aController)
	{
	CGetLockInfo* self = new(ELeave) CGetLockInfo(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor.
Cancels outstanding requests.
*/
CGetLockInfo::~CGetLockInfo()
	{
	Cancel();
	}

/**
Gets ICC Lock information and stores it in the iIccLockInfoV1Pckg package.
*/
void CGetLockInfo::DoStartRequestL()
	{
	_LIT(KDummyAnswerPanic, "CGetLockInfo Get Method");
	__ASSERT_ALWAYS(!IsActive(), User::Panic(KDummyAnswerPanic, 1));
	iRequestNotify = EFalse;
	CTelephony::TIccLock lockSelect = CTelephony::ELockPin2;
	
	// Retrieves the state and status of ICC locks 
	iTelephony->GetLockInfo(iStatus, lockSelect, iIccLockInfoV1Pckg);
	SetActive();
	}

/**
Default constructor.

@param aController Pointer to an MExecAsync object passed to constructor of 
                   CISVAPIBase
*/
CGetLockInfo::CGetLockInfo(MExecAsync* aController)
	: CISVAPIAsync(aController, KLockInfo),
	  iIccLockInfoV1Pckg(iIccLockInfoV1)
	{
	// Empty method
	}

/**
Second phase constructor.
*/
void CGetLockInfo::ConstructL()
	{
	// Empty method
	}

/**
Checks status of the active object and displays the ICC lock information if
there is no error.
*/
void CGetLockInfo::RunL()
	{
	if(iStatus != KErrNone)
		{
		iConsole->Printf(KError);
		
		// Print the error status code
		iConsole->Printf(_L("%d\n"), iStatus.Int());
		}
	else
		{
		if( iIccLockInfoV1.iSetting == CTelephony::ELockSetEnabled)
       	{
			iConsole->Printf(_L("Lock 2 is available for the phone to use\n"));
			switch (iIccLockInfoV1.iStatus)
				{
			case CTelephony::EStatusUnlocked:
				iConsole->Printf(_L("Its current status is Unlocked\n"));
				break;
			case CTelephony::EStatusBlocked:
				iConsole->Printf(_L("Its current status is blocked\n"));
				break;
			case CTelephony::EStatusLocked:
				iConsole->Printf(_L("Its current status is locked\n"));
				break;
			case CTelephony::EStatusUnknown:
				iConsole->Printf(_L("Its current status is unknown\n"));
				break;
				}
			if (iIccLockInfoV1.iStatus == CTelephony::EStatusUnlocked)
				{
				// Lock is enabled but unlocked so can continue.
				ExampleComplete();
				}
			else
				{
				DoRequestNotificationL();
				// Alert user that they may not be able to call number if it's 
				// not in their FDN phone book.
				}
			}
		else
			{
			// Lock isn't enabled so can continue
			ExampleComplete();
			}
		}
	}

/**
Requests to receive notifications of change in the ICC lock information.
*/
void CGetLockInfo::DoRequestNotificationL()
	{
	// Panic if this object is already performing an asynchronous operation.
	// Application will panic if you call SetActive() on an already active object.
	_LIT( KNotifyPanic, "CGetLockInfo Notify Method" );
	__ASSERT_ALWAYS( !IsActive(), User::Panic( KNotifyPanic, 1 ));
	iRequestNotify = ETrue;
	
	// Registers interest in receiving change notifications for events.
	iTelephony->NotifyChange(	iStatus,
								CTelephony::EPin2LockInfoChange,
								iIccLockInfoV1Pckg );
	SetActive();
	}

/**
Cancels asynchronous request to CTelephony::GetLockInfo().
*/
void CGetLockInfo::DoCancel()
	{
	// Cancels an outstanding asynchronous request.
	iTelephony->CancelAsync(CTelephony::EPin2LockInfoChangeCancel);
	}

