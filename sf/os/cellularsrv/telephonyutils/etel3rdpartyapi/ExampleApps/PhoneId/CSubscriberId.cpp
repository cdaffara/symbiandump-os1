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


#include "CSubscriberId.h"

/**
Factory constructor.

@param  aController Pointer to MExecAsync object passed to constructor of 
                    CISVAPIBase
@return             Instance of CSubscriberId class
*/
CSubscriberId* CSubscriberId::NewL(MExecSync* aController)
	{
	CSubscriberId* self = new(ELeave) CSubscriberId(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor.
Cancels outstanding requests.
*/
CSubscriberId::~CSubscriberId()
	{
	Cancel();
	}

/**
Gets the subscriber Id by calling 3rd party API function CTelephony::GetSubscriberId()
and stores it in the iSubscripberIdV1Pckg package.
*/
void CSubscriberId::DoStartRequestL()
	{
	// Retrieves information about the mobile device's current subscriber
	iTelephony->GetSubscriberId(iStatus, iSubscriberIdV1Pckg);
	SetActive();
	}

/**
Constructor.

@param aController Pointer to an MExecAsync object passed to constructor of 
                   CISVAPIBase
*/
CSubscriberId::CSubscriberId(MExecSync* aController)
	: CISVAPISync(aController, KSubscriberId),
	  iSubscriberIdV1Pckg(iSubscriberIdV1)
	{
	// Empty method
	}

/**
Second phase constructor.
*/
void CSubscriberId::ConstructL()
	{
	// Empty method
	}

/**
Checks the status of the active object and prints the subscriber information to
the console if there is no error.
*/
void CSubscriberId::RunL()
	{
	if(iStatus != KErrNone)
		{
		iConsole->Printf(KError);
		
		// Print the error status code
		iConsole->Printf(_L("%d\n"), iStatus.Int());
		}
	else
		{
		// Print Subscriber Info.
		TBuf<CTelephony::KIMSISize> subscriberId = iSubscriberIdV1.iSubscriberId;
		iConsole->Printf(KSubscriberIdMsg);
		
		// Print the subscriber information about the phone
		iConsole->Printf(subscriberId);
		iConsole->Printf(KNewLine);

		ExampleComplete();
		}
	}

/**
Cancels the asynchronous request to CTelephony::GetSubscriberId().
*/
void CSubscriberId::DoCancel()
	{
	// Cancels an outstanding asynchronous request.
	iTelephony->CancelAsync(CTelephony::EGetSubscriberIdCancel);
	}

