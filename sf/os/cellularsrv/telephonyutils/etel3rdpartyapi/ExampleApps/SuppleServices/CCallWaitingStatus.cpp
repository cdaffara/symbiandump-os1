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
	
	// Interrogate the current status of the call waiting services.
	iTelephony->GetCallWaitingStatus(iStatus, iCallWaitingStatusV1Pckg);
	SetActive();
	}

/**
Default constructor.

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
Prints information about the current call waiting status if the request
completed without error.
*/
void CCallWaitingStatus::RunL()
	{
	// Print Battery Info
	if(iStatus != KErrNone)
		{
		iConsole->Printf(KError);
		
		// Print the error status code
		iConsole->Printf(_L("%d\n"), iStatus.Int());
		}
	else
		{
		// Print the console output message if there is no error
		iConsole->Printf(KCallWaitingStatusMsg);
		switch(iCallWaitingStatusV1.iCallWaiting)
			{
		case CTelephony::EStatusActive:
			iConsole->Printf(_L("Call waiting status is active.\n"));
			break;
		case CTelephony::ENotActive:
			iConsole->Printf(_L("Call waiting status is not active.\n"));
			break;
		case CTelephony::ENotProvisioned:
			iConsole->Printf(_L("Call waiting status is not provisioned.\n"));
			break;
		case CTelephony::ENotAvailable:
			iConsole->Printf(_L("Call waiting status is not available.\n"));
			break;
		case CTelephony::EUnknown:
		default:
			iConsole->Printf(_L("Call waiting status is unknown.\n"));
			break;
			}
		}
	ExampleComplete();
	}

/**
Cancels asynchronous request to CTelephony::GetCallWaitingStatus().
*/
void CCallWaitingStatus::DoCancel()
	{
	// Cancels an outstanding asynchronous request
	iTelephony->CancelAsync(CTelephony::EGetCallWaitingStatusCancel);
	}

