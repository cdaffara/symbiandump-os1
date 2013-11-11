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

#include "CDialCall.h"

/**
Factory constructor.

@param  aController Pointer to MExecAsync object passed to constructor of
                    CISVAPIBase
@return             Instance of CDialCall class
*/
CDialCall* CDialCall::NewL(MExecAsync* aController)
	{
	CDialCall* self = new(ELeave) CDialCall(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor.  Cancels any outstanding requests.
*/
CDialCall::~CDialCall()
	{
	Cancel();
	}

/**
Sets the required parameters and starts dialling the call.

@param aNumber Phone number to dial.
*/
void CDialCall::DoStartRequestL(const TDesC& aNumber)
	{
	CTelephony::TCallParamsV1 callParams;
	callParams.iIdRestrict = CTelephony::ESendMyId;
	CTelephony::TCallParamsV1Pckg callParamsPckg(callParams);
	
	iConsole->Printf(_L("Dialling "));
	
	// Print the number to dial
	iConsole->Printf(aNumber);
	iConsole->Printf(KNewLine);
	
	// Dial a new call to specified phone number
	iTelephony->DialNewCall(iStatus, callParamsPckg, aNumber, iCallId);
	SetActive();
	}

/**
Constructor.

@param aController Pointer to MExecAsync object passed to constructor of CISVAPIBase
*/
CDialCall::CDialCall(MExecAsync* aController)
	: CISVAPIAsync(aController, KDialCall)
	{
	// Empty method
	}

/**
Second phase constructor.
*/
void CDialCall::ConstructL()
	{
	// Empty method
	}

/**
Checks status of the active object and displays output to console if
there is no error.
*/
void CDialCall::RunL()
	{
	if(iStatus != KErrNone)
		{
		// Return error
		iConsole->Printf(KError);
		
		// Print error status code
		iConsole->Printf(_L("%d\n"), iStatus.Int());
		}
	else
		{
		// Print 'Dialling' if there is no error
		iConsole->Printf(_L("Dialling...\n"));
		ExampleComplete();
		}
	}

/**
Cancels asynchronous request to CTelephony::DialNewCall().
*/
void CDialCall::DoCancel()
	{
	// Cancels an outstanding asynchronous request.
	iTelephony->CancelAsync(CTelephony::EDialNewCallCancel);
	}
