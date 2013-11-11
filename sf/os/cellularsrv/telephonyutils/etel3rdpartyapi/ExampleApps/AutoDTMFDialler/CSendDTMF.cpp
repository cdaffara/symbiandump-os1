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


#include "CSendDTMF.h"

/**
Factory constructor.

@param  aController Pointer to MExecAsync object passed to constructor of 
                    CISVAPIBase
@return             Instance of CSendDTMF class
*/
CSendDTMF* CSendDTMF::NewL(MExecAsync* aController)
	{
	CSendDTMF* self = new(ELeave) CSendDTMF(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor.  Cancels outstanding requests.
*/
CSendDTMF::~CSendDTMF()
	{
	Cancel();
	}

/**
Sends DTMF tones as specified in aNumber.

@param aNumber Descriptor containing the DTMF tones to send
*/
void CSendDTMF::DoStartRequestL(const TDesC& aNumber)
	{
	if (aNumber.Length() == 1)
		{
		iSingleTone = ETrue;
		}
	else
		{
		iSingleTone = EFalse;
		}
	
	// Transmits DTMF tones across all the currently active voice calls.
	iTelephony->SendDTMFTones(iStatus, aNumber);
	SetActive();
	}

/**
Constructor.

@param aController Pointer to MExecAsync object passed to constructor of 
                   CISVAPIBase
*/
CSendDTMF::CSendDTMF(MExecAsync* aController)
	: CISVAPIAsync(aController, KSendDTMF)
	{
	// Empty method
	}

/**
Second phase constructor.
*/
void CSendDTMF::ConstructL()
	{
	iSingleTone = EFalse;
	}

/**
Checks iStatus and prints details to console.
*/
void CSendDTMF::RunL()
	{
	if(iStatus != KErrNone)
		{
		iConsole->Printf(KError);
		
		// Print error status code
		iConsole->Printf(_L("%d\n"), iStatus.Int());
		}
	else
		{
		if (iSingleTone)
			{
			iConsole->Printf(_L("Beep\n"));
			ExampleNotify();
			}
		else
			{
			iConsole->Printf(_L("Beeps\n"));
			ExampleComplete();
			}
		}
	}

/**
Cancels asynchronous request to CTelephony::SendDTMFTones().
*/
void CSendDTMF::DoCancel()
	{
	// Cancels an outstanding asynchronous request.
	iTelephony->CancelAsync(CTelephony::ESendDTMFTonesCancel);
	}
