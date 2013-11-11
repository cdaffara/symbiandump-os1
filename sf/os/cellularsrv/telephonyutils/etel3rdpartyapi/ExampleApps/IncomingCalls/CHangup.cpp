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


#include "CHangup.h"

/**
Factory constructor.

@param  aController Pointer to MExecAsync object passed to constructor of 
                    CISVAPIBase
@return             Instance of CHangup class
*/
CHangup* CHangup::NewL(MExecAsync* aController)
	{
	CHangup* self = new(ELeave) CHangup(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor.
Cancels outstanding requests.
*/
CHangup::~CHangup()
	{
	Cancel();
	}

/**
Hangs up call specified in aCallId.

@param aCallId Call ID of call to hang up
*/
void CHangup::DoStartRequestL(CTelephony::TCallId aCallId)
	{
	// Hangs up a call
	iTelephony->Hangup(iStatus, aCallId);
    SetActive();
	}

/**
Default constructor.

@param aController Pointer to MExecAsync object passed to constructor of 
                   CISVAPIBase
*/
CHangup::CHangup(MExecAsync* aController)
	: CISVAPIAsync(aController, KHangup)
	{
	// Empty method
	}

/**
Second phase constructor.
*/
void CHangup::ConstructL()
	{
	// Empty method
	}

/**
Checks the status of the active object and if there is no error, tells the user
via the console that the call has been hung up.
*/
void CHangup::RunL()
	{
	if(iStatus != KErrNone)
		{
		iConsole->Printf(KError);
		
		// Print the error status code
		iConsole->Printf(_L("%d\n"), iStatus.Int());
		}
	else
		{
		// Print the consoel output message if there is no error
		iConsole->Printf(_L("CLICK\n"));
		}
	}
	
/**
Cancels asynchronous request to CTelephony::Hangup().
*/	
void CHangup::DoCancel()
	{
	// Cancels an outstanding asynchronous request.
	iTelephony->CancelAsync(CTelephony::EHangupCancel);
	}
