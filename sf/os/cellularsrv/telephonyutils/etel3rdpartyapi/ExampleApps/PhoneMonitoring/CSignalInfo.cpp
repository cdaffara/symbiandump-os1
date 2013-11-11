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

#include "CSignalInfo.h"

/**
Factory constructor.

@param  aController Pointer to MExecAsync object passed to constructor of 
                    CISVAPIBase
@return             Instance of CSignalInfo class
*/
CSignalInfo* CSignalInfo::NewL(MExecAsync* aController)
	{
	CSignalInfo* self = new(ELeave) CSignalInfo(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor.
Cancels outstanding requests.
*/
CSignalInfo::~CSignalInfo()
	{
	Cancel();
	}

/**
Gets the signal strength and stores it in the iSignalStrengthV1Pckg package.
*/
void CSignalInfo::DoStartRequestL()
	{
	_LIT(KDummyAnswerPanic, "CSignalInfo Get Method");
	__ASSERT_ALWAYS(!IsActive(), User::Panic(KDummyAnswerPanic, 1));
	iRequestNotify = EFalse;
	
	// Retrieves the phone's current signal strength via the aDes argument.
	iTelephony->GetSignalStrength(iStatus, iSignalStrengthV1Pckg);
	SetActive();
	}

/**
Constructor.

@param aController Pointer to an MExecAsync object passed to constructor of 
                   CISVAPIBase
*/
CSignalInfo::CSignalInfo(MExecAsync* aController)
	: CISVAPIAsync(aController, KSignalInfo),
	  iSignalStrengthV1Pckg(iSignalStrengthV1)
	{
	// Empty method
	}

/**
Second phase constructor.
*/
void CSignalInfo::ConstructL()
	{
	// Empty method
	}

/**
Checks the status of the active object and prints the signal strength (in bars)
to the console if there is no error.
*/
void CSignalInfo::RunL()
	{
	if(iStatus != KErrNone)
		{
		iConsole->Printf(KError);
		
		// Print the error status code
		iConsole->Printf(_L("%d\n"), iStatus.Int());
		}
	else
		{
		TInt32 strength = iSignalStrengthV1.iSignalStrength;
		TInt8 bars = iSignalStrengthV1.iBar;
		if (iRequestNotify)
			{
			iConsole->ClearScreen();
			iConsole->Printf(_L("~*THIS IS A NOTIFICATION*~\n"));
			}
		iConsole->Printf(KSignalStrengthMsg);
		iConsole->Printf(_L("There are %d bars!\n"), bars);
		iConsole->Printf(_L("Signal Strength is %d!\n"), strength);
		if (iRequestNotify)
			{
			DoRequestNotificationL();
			}
		else
			{
			ExampleComplete();
			}
		}
	}

/**
Requests to receive notifications of change in the signal strength.
*/
void CSignalInfo::DoRequestNotificationL()
   {
	// Panic if this object is already performing an asynchronous operation. 
	// Application will panic if you call SetActive() on an already active object.
	_LIT( KNotifyPanic, "CSignalInfo Notify Method" );
	__ASSERT_ALWAYS( !IsActive(), User::Panic( KNotifyPanic, 1 ));
	iRequestNotify = ETrue;
	
	// Registers interest in receiving change notifications for events.
	iTelephony->NotifyChange( 	iStatus,
								CTelephony::ESignalStrengthChange,
								iSignalStrengthV1Pckg );
	SetActive();
	}

/**
Cancels asynchronous request to CTelephony::GetSignalStrength().
*/
void CSignalInfo::DoCancel()
	{
	// Cancels an outstanding asynchronous request.
	iTelephony->CancelAsync(CTelephony::ESignalStrengthChangeCancel);
	}
