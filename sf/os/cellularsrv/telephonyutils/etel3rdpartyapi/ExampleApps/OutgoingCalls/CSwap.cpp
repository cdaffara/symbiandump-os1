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


#include "CSwap.h"

/**
Factory constructor.

@param  aController Pointer to MExecAsync object passed to constructor of 
                    CISVAPIBase
@return             Instance of CSwap class
*/
CSwap* CSwap::NewL(MExecAsync* aController)
	{
	CSwap* self = new(ELeave) CSwap(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor.
Cancels outstanding requests.
*/
CSwap::~CSwap()
	{
	Cancel();
	}

/**
Swaps the two specified calls if the dynamic call capabilities allow it.

@param aCallId1 ID of call to be swapped with aCallId2
@param aCallId2 ID of call to be swapped with aCallId1
*/
void CSwap::DoStartRequestL(CTelephony::TCallId aCallId1,
							CTelephony::TCallId aCallId2)
	{
	_LIT(KDummyAnswerPanic, "CSignalInfo Get Method");
	__ASSERT_ALWAYS(!IsActive(), User::Panic(KDummyAnswerPanic, 1));
	iRequestNotify = EFalse;
	CTelephony::TCallCapsV1 callCapsV1;
	CTelephony::TCallCapsV1Pckg callCapsV1Pckg(callCapsV1);
	
	// Retrieves the dynamic call capabilities for calls you dialled or answered with CTelephony.
	iTelephony->GetCallDynamicCaps(aCallId1, callCapsV1Pckg);

	if( callCapsV1.iControlCaps & CTelephony::KCapsSwap)
		{
		// The call represented by aCallId1 can be swapped with iCallId2.
		iTelephony->Swap(iStatus, aCallId1, aCallId2);
		SetActive();
		}
	else
		{
		// The calls cannot be swapped.
		}
	}

/**
Constructor.

@param aController Pointer to an MExecAsync object passed to constructor of 
                   CISVAPIBase
*/
CSwap::CSwap(MExecAsync* aController)
	: CISVAPIAsync(aController, KSwap)
	{
	// Empty method
	}

/**
Second phase constructor.
*/
void CSwap::ConstructL()
	{
	// Empty method
	}

/**
Checks the status of the active object and calls ExampleComplete() to notify
the menu object that the example has finished.
*/
void CSwap::RunL()
	{
	if(iStatus != KErrNone)
		{
		iConsole->Printf(KError);
		
		// Print the error status code
		iConsole->Printf(_L("Swap %d\n"), iStatus.Int());
		}
	else
		{
		ExampleComplete();
		}
	}

/**
Cancels asynchronous request to CTelephony::Swap().
*/
void CSwap::DoCancel()
	{
	// Cancels an outstanding asynchronous request.
	iTelephony->CancelAsync(CTelephony::ESwapCancel);
	}

