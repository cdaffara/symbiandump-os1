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


#include "COperatorName.h"

/**
Factory constructor.

@param  aController Pointer to MExecAsync object passed to constructor of 
                    CISVAPIBase
@return             Instance of COperatorName class
*/
COperatorName* COperatorName::NewL(MExecAsync* aController)
	{
	COperatorName* self = new(ELeave) COperatorName(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor.
Cancels outstanding requests.
*/
COperatorName::~COperatorName()
	{
	Cancel();
	}

/**
Gets the network operator name and stores it in the iOperatorNameV1Pckg
package.
*/
void COperatorName::DoStartRequestL()
	{
	_LIT(KDummyAnswerPanic, "COperatorName Get Method");
	__ASSERT_ALWAYS(!IsActive(), User::Panic(KDummyAnswerPanic, 1));
	iRequestNotify = EFalse;
	
	// Retrieve ICC stored information about the currently registered mobile network
	iTelephony->GetOperatorName(iStatus, iOperatorNameV1Pckg);
	SetActive();
	}

/**
Constructor.

@param aController Pointer to an MExecAsync object passed to constructor of 
                   CISVAPIBase
*/
COperatorName::COperatorName(MExecAsync* aController)
	: CISVAPIAsync(aController, KOperatorName),
	  iOperatorNameV1Pckg(iOperatorNameV1)
	{
	// Empty method
	}

/**
Second phase constructor.
*/
void COperatorName::ConstructL()
	{
	// Empty method
	}

/**
Checks the status of the active object and prints the operator name to the
console if there is no error.
*/
void COperatorName::RunL()
	{
	if(iStatus != KErrNone)
		{
		iConsole->Printf(KError);
		
		// Print the error status code
		iConsole->Printf(_L("%d\n"), iStatus.Int());
		}
	else
		{
		// Print the output to console if there is no error
		iConsole->Printf(KOperatorNameMsg);
		iConsole->Printf(iOperatorNameV1.iOperatorName);
		iConsole->Printf(KNewLine);
		ExampleComplete();
		}
	}

/**
Cancels asynchronous request to CTelephony::GetOperatorName().
*/
void COperatorName::DoCancel()
	{
	// Cancels an outstanding asynchronous request
	iTelephony->CancelAsync(CTelephony::EGetOperatorNameCancel);
	}

