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


#include "CCallWaitingStatus.h"

/**
Factory constructor.

@param  aController Pointer to MExecAsync object passed to constructor of 
                    CISVAPIBase
@return             Instance of CCallWaitingStatus class
*/
CCallWaitingStatus* CCallWaitingStatus::NewL(MExecAsync* aController)
	{
	CCallWaitingStatus* self = new(ELeave) CCallWaitingStatus(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor.
Cancels outstanding requests.
*/
CCallWaitingStatus::~CCallWaitingStatus()
	{
	Cancel();
	}

/**
Gets the call waiting status and stores it in the iCallWaitingStatusV1Pckg 
package.
*/
void CCallWaitingStatus::DoStartRequestL()
	{
	_LIT( KNotifyPanic, "CCallWaitingStatus Get Method" );
	__ASSERT_ALWAYS( !IsActive(), User::Panic( KNotifyPanic, 1 ));
	iRequestNotify = EFalse;
	
	// Interrogate the current status of the call waiting services
	iTelephony->GetCallWaitingStatus(iStatus, iCallWaitingStatusV1Pckg);
	SetActive();
	}

/**
Constructor.

@param aController Pointer to an MExecAsync object passed to constructor of
                   CISVAPIBase
*/
CCallWaitingStatus::CCallWaitingStatus(MExecAsync* aController)
	: CISVAPIAsync(aController, KCallWaitingStatus),
	  iCallWaitingStatusV1Pckg(iCallWaitingStatusV1)
	{
	// Empty method
	}

/**
Second phase constructor.
*/
void CCallWaitingStatus::ConstructL()
	{
	// Empty method
	}

/**
If the active object completed, notify the menu object of the result.
*/
void CCallWaitingStatus::RunL()
	{
	if(iStatus != KErrNone)
		{
		iConsole->Printf(KError);
		
		// Print the error status code
		iConsole->Printf(_L("%d\n"), iStatus.Int());
		}
	else
		{
		// Print the console message if no error
		iConsole->Printf(KCallWaitingStatusMsg);
		switch(iCallWaitingStatusV1.iCallWaiting)
			{
		case CTelephony::EStatusActive:
			ExampleComplete();
			break;
		case CTelephony::ENotActive:
		case CTelephony::ENotProvisioned:
		case CTelephony::ENotAvailable:
		case CTelephony::EUnknown:
			ExampleNotify();
			break;
			}
		}
	}

/**
Cancels asynchronous request to CTelephony::GetCallWaitingStatus()
*/
void CCallWaitingStatus::DoCancel()
	{
	// Cancels an outstanding asynchronous request.
	iTelephony->CancelAsync(CTelephony::EGetCallWaitingStatusCancel);
	}

