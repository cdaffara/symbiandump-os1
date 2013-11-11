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


#include "CIdentityServiceStatus.h"

/**
Factory constructor.

@param  aController Pointer to MExecAsync object passed to constructor of 
                    CISVAPIBase
@return             Instance of CIdentityServiceStatus class
*/
CIdentityServiceStatus* CIdentityServiceStatus::NewL(MExecAsync* aController)
	{
	CIdentityServiceStatus* self = new(ELeave) CIdentityServiceStatus(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor.
Cancels outstanding requests.
*/
CIdentityServiceStatus::~CIdentityServiceStatus()
	{
	Cancel();
	}

/**
Gets the identity service status and stores it in the
iIdentityServiceStatusV1Pckg package.
*/
void CIdentityServiceStatus::DoStartRequestL()
	{
	_LIT(KDummyAnswerPanic, "CIdentityServiceStatus Get Method");
	__ASSERT_ALWAYS(!IsActive(), User::Panic(KDummyAnswerPanic, 1));
	iRequestNotify = EFalse;
 	CTelephony::TIdentityService condition = CTelephony::EIdServiceCallerPresentation;
 	
 	// Interrogate the current status of the identity services.
	iTelephony->GetIdentityServiceStatus(iStatus,
													 condition,
													 iIdentityServiceStatusV1Pckg);
	SetActive();
	}

/**
Constructor.

@param aController Pointer to an MExecAsync object passed to constructor of 
                   CISVAPIBase
*/
CIdentityServiceStatus::CIdentityServiceStatus(MExecAsync* aController)
	: CISVAPIAsync(aController, KIdentityServiceStatus),
	  iIdentityServiceStatusV1Pckg(iIdentityServiceStatusV1)
	{
	// Empty method
	}

/**
Second phase constructor.
*/
void CIdentityServiceStatus::ConstructL()
	{
	// Empty method
	}

/**
Checks the status of the active object and prints the identity service status 
to the console if there is no error.
*/
void CIdentityServiceStatus::RunL()
	{
	if(iStatus != KErrNone)
		{
		iConsole->Printf(KError);
		
		// Print the error status code
		iConsole->Printf(_L("%d\n"), iStatus.Int());
		}
	else
		{
		// Print call identity service status.
		iConsole->Printf(KIdentityServiceStatusMsg);
		switch(iIdentityServiceStatusV1.iIdentityStatus)
			{
		case CTelephony::EIdServiceActivePermanent:
			iConsole->Printf(_L("Provisioned and permanently active.\n"));
			break;
		case CTelephony::EIdServiceActiveDefaultRestricted:
			iConsole->Printf(_L("Provisioned and active. By default, the number is restricted unless overridden by the user.\n"));
			break;
		case CTelephony::EIdServiceActiveDefaultAllowed :
			iConsole->Printf(_L("Provisioned and active. By default, the number is displayed unless specifically restricted by the user.\n"));
			break;
		case CTelephony::EIdServiceNotProvisioned:
			iConsole->Printf(_L("Not provisioned.\n"));
			break;
		case CTelephony::EIdServiceUnknown:
			iConsole->Printf(_L("Unknown.\n"));
			break;
			}
		}
	ExampleComplete();
	}

/**
Cancels the asynchronous request to CTelephony::GetIdentityServiceStatus().
*/
void CIdentityServiceStatus::DoCancel()
	{
	// Cancels an outstanding asynchronous request.
	iTelephony->CancelAsync(CTelephony::EGetIdentityServiceStatusCancel);
	}

