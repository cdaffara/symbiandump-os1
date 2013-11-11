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
Destructor.
Cancels outstanding requests.
*/
CDialCall::~CDialCall()
	{
	Cancel();
	}

/**
Makes a call to aNumber.

@param aNumber The number to dial
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
	
	// Initiates a new call.
	iTelephony->DialNewCall(iStatus, callParamsPckg, aNumber, iCallId);
	SetActive();
	}

/**
Constructor.

@param aController Pointer to an MExecAsync object passed to constructor of 
                   CISVAPIBase
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
Checks the status of the active object and calls ExampleComplete() to notify
the menu object that the example has finished.
*/
void CDialCall::RunL()
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
Cancels asynchronous request to CTelephony::DialNewCall()
*/
void CDialCall::DoCancel()
	{
	// Cancels an outstanding asynchronous request
	iTelephony->CancelAsync(CTelephony::EDialNewCallCancel);
	}
