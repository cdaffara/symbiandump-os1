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

#include "CAnswerIncomingCall.h"

/**
Factory constructor.

@param  aController Pointer to MExecAsync object passed to constructor of 
                    CISVAPIBase
@return             Instance of CAnswerIncomingCall class
*/
CAnswerIncomingCall* CAnswerIncomingCall::NewL(MExecAsync* aController)
	{
	CAnswerIncomingCall* self = new(ELeave) CAnswerIncomingCall(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor.
Cancels outstanding requests.
*/
CAnswerIncomingCall::~CAnswerIncomingCall()
	{
	Cancel();
	}

/**
Answers the call specified in iCallId.
*/
void CAnswerIncomingCall::DoStartRequestL()
	{
	_LIT( KNotifyPanic, "CAnswerIncomingCall Get Method" );
	__ASSERT_ALWAYS( !IsActive(), User::Panic( KNotifyPanic, 1 ));
	iRequestNotify = EFalse;
	
	// Answers an incoming new voice call.
	iTelephony->AnswerIncomingCall(iStatus, iCallId);
	SetActive();
	}

/**
Constructor.

@param aController Pointer to an MExecAsync object passed to constructor of 
                   CISVAPIBase
*/
CAnswerIncomingCall::CAnswerIncomingCall(MExecAsync* aController)
	: CISVAPIAsync(aController, KAnswerIncomingCall),
	  iCallStatusV1Pckg( iCallStatusV1 )
	{
	// Empty method
	}

/**
Second phase constructor.
*/
void CAnswerIncomingCall::ConstructL()
	{
	iRequestNotify = EFalse;
	}

/**
Checks the status of the active object and if there is no error, tells the
user via the console that the call has been answered.
*/
void CAnswerIncomingCall::RunL()
	{
	if(iStatus != KErrNone)
		{
		iConsole->Printf(KError);
		
		// Print error status code
		iConsole->Printf(_L("%d\n"), iStatus.Int());
		}
	else
		{
		// Print the output to console if no error
		iConsole->Printf(_L("Call Answered ....\n"));
		ExampleComplete();
		}
	}

/**
Cancels asynchronous request to CTelephony::AnswerIncomingCall().
*/
void CAnswerIncomingCall::DoCancel()
	{
	// Cancels an outstanding asynchronous request.
	iTelephony->CancelAsync(CTelephony::EAnswerIncomingCallCancel);
	}

