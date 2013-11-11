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


#include "CHold.h"

/**
Factory constructor.

@param  aController Pointer to MExecAsync object passed to constructor of
                    CISVAPIBase
@return             Instance of CHold class
*/
CHold* CHold::NewL(MExecAsync* aController)
	{
	CHold* self = new(ELeave) CHold(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor.
Cancels outstanding requests.
*/
CHold::~CHold()
	{
	Cancel();
	}

/**
Holds a call if the dynamic capabilities allow it.

@param aCallId The identifier of the call to put on hold
*/
void CHold::DoStartRequestL(CTelephony::TCallId aCallId)
	{
	_LIT(KDummyAnswerPanic, "CHold Get Method");
	__ASSERT_ALWAYS(!IsActive(), User::Panic(KDummyAnswerPanic, 1));
	iRequestNotify = EFalse;
	CTelephony::TCallCapsV1 callCapsV1;
	CTelephony::TCallCapsV1Pckg callCapsV1Pckg(callCapsV1);
	
	// Retrieves the dynamic call capabilities for calls you dialled 
	iTelephony->GetCallDynamicCaps(aCallId, callCapsV1Pckg);

	if( callCapsV1.iControlCaps & CTelephony::KCapsHold )
		{
		// The call represented by aCallId can be put on hold.
		iTelephony->Hold(iStatus, aCallId);
		SetActive();
		}
	else if (callCapsV1.iControlCaps & CTelephony::KCapsSwap)
		{
		// Can't hold the active call.
		}
	}

/**
Default constructor.

@param aController Pointer to an MExecAsync object passed to constructor of
                   CISVAPIBase
*/
CHold::CHold(MExecAsync* aController)
	: CISVAPIAsync(aController, KHold)
	{
	// Empty method
	}

/**
Second phase constructor.
*/
void CHold::ConstructL()
	{
	// Empty method
	}

/**
Checks the status of the active object and calls ExampleComplete() to notify 
the menu object that the example has finished.
*/
void CHold::RunL()
	{
	if(iStatus != KErrNone)
		{
		iConsole->Printf(KError);
		
		// Print the error status code
		iConsole->Printf(_L("%d\n"), iStatus.Int());
		}
	else
		{
		ExampleComplete();
		}
	}

/**
Cancels asynchronous request to CTelephony::Hold()
*/
void CHold::DoCancel()
	{
	// Cancels an outstanding asynchronous request.
	iTelephony->CancelAsync(CTelephony::EHoldCancel);
	}

