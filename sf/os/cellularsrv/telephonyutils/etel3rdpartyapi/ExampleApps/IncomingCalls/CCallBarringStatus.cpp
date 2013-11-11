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

#include "CCallBarringStatus.h"

/**
Factory constructor.

@param  aController Pointer to MExecAsync object passed to constructor of 
                    CISVAPIBase
@return             Instance of CCallBarringStatus class
*/
CCallBarringStatus* CCallBarringStatus::NewL(MExecAsync* aController)
	{
	CCallBarringStatus* self = new(ELeave) CCallBarringStatus(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor.
Calls Cancel() to cancel outstanding requests.
*/
CCallBarringStatus::~CCallBarringStatus()
	{
	Cancel();
	}

/**
Gets the current call barring status and stores the result in
iCallBarringStatusV1Pckg.
*/
void CCallBarringStatus::DoStartRequestL()
	{
	_LIT( KNotifyPanic, "CCallBarringStatus Get Method" );
	__ASSERT_ALWAYS( !IsActive(), User::Panic( KNotifyPanic, 1 ));
	iRequestNotify = EFalse;
	CTelephony::TCallBarringCondition condition = CTelephony::EBarAllIncoming;
	
	// Interrogate the current status of the call barring services
	iTelephony->GetCallBarringStatus(	iStatus,
										condition,
										iCallBarringStatusV1Pckg);
	SetActive();
	}

/**
Constructor.

@param aController Pointer to an MExecAsync object passed to constructor of 
                   CISVAPIBase.
*/
CCallBarringStatus::CCallBarringStatus(MExecAsync* aController)
	: CISVAPIAsync(aController, KCallBarringStatus)
	, iCallBarringStatusV1Pckg(iCallBarringStatusV1)
	{
	//
	}

/**
Second phase constructor.
*/
void CCallBarringStatus::ConstructL()
	{
	// Empty method
	}

/**
Checks the status of the active object and displays the retrieved call barring
status if there is no error.
*/
void CCallBarringStatus::RunL()
	{
	if(iStatus != KErrNone)
		{
		iConsole->Printf(KError);
		
		// Print the error status code
		iConsole->Printf(_L("%d\n"), iStatus.Int());
		}
	else
		{
		// Print the console output message if there is no error
		iConsole->Printf(KCallBarringStatusMsg);
		switch(iCallBarringStatusV1.iCallBarring)
			{
		case CTelephony::EStatusActive:
			// Call barring is active and restricts all incoming calls!
			// Could end application here by calling Terminate().
			// Wait for 5 seconds then continue by calling ExampleComplete()
			iConsole->Printf(_L("Call Barring Restricts incoming calls!\n"));
			iConsole->Printf(_L("Recify this to remove this delay\n"));
			User::After(5000000);
			ExampleComplete();
			break;
		case CTelephony::ENotActive:
		case CTelephony::ENotProvisioned:
		case CTelephony::ENotAvailable:
		case CTelephony::EUnknown:
			ExampleComplete();
			break;
			}
		}
	}

/**
Cancels asynchronous request to CTelephony::GetCallBarringStatus().
*/
void CCallBarringStatus::DoCancel()
	{
	// Cancels an outstanding asynchronous request
	iTelephony->CancelAsync(CTelephony::EGetCallBarringStatusCancel);
	}
