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
Gets the current call barring status and stores result in 
iCallBarringStatusV1Pckg.
*/
void CCallBarringStatus::DoStartRequestL()
	{
	_LIT( KNotifyPanic, "CCallBarringStatus Get Method" );
	__ASSERT_ALWAYS( !IsActive(), User::Panic( KNotifyPanic, 1 ));
	iRequestNotify = EFalse;
	CTelephony::TCallBarringCondition condition = CTelephony::EBarAllIncoming;
	
	// Interrogate the current status of the call barring services.
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
	: CISVAPIAsync(aController, KCallBarringStatus),
	  iCallBarringStatusV1Pckg(iCallBarringStatusV1)
	{
	// Empty method
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
		// Print the output to the console if there is no error.
		iConsole->Printf(KCallBarringStatusMsg);
		switch(iCallBarringStatusV1.iCallBarring)
			{
		case CTelephony::EStatusActive:
			iConsole->Printf(_L("Call barring status is active.\n"));
			break;
		case CTelephony::ENotActive:
			iConsole->Printf(_L("Call barring status is not active.\n"));
			break;
		case CTelephony::ENotProvisioned:
			iConsole->Printf(_L("Call barring status is not provisioned.\n"));
			break;
		case CTelephony::ENotAvailable:
			iConsole->Printf(_L("Call barring status is not available.\n"));
			break;
		case CTelephony::EUnknown:
		default:
			iConsole->Printf(_L("Call barring status is unknown.\n"));
			break;
			}
		}
	ExampleComplete();
	}

/**
Cancels the asynchronous request to CTelephony::GetCallBarringStatus().
*/
void CCallBarringStatus::DoCancel()
	{
	// Cancels an outstanding asynchronous request.
	iTelephony->CancelAsync(CTelephony::EGetCallBarringStatusCancel);
	}

