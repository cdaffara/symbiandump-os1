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


#include "CCallForwardingStatus.h"

/**
Factory constructor.

@param  aController Pointer to MExecAsync object passed to constructor of 
                    CISVAPIBase
@return             Instance of CCallForwardingStatus class
*/
CCallForwardingStatus* CCallForwardingStatus::NewL(MExecAsync* aController)
	{
	CCallForwardingStatus* self = new(ELeave) CCallForwardingStatus(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Cancels outstanding requests.
*/
CCallForwardingStatus::~CCallForwardingStatus()
	{
	Cancel();
	}

/**
Retrieves the current forwarding status.
*/
void CCallForwardingStatus::DoStartRequestL()
	{
	_LIT( KNotifyPanic, "CCallForwardingStatus Get Method" );
	__ASSERT_ALWAYS( !IsActive(), User::Panic( KNotifyPanic, 1 ));
	iRequestNotify = EFalse;
	if (iSecondCondition)
		{
		iSecondCondition = EFalse;
		CTelephony::TCallForwardingCondition condition2;
		condition2 = CTelephony::ECallForwardingUnconditional;
		
		// Interrogates the current status of the call forwarding services for first condition.
		iTelephony->GetCallForwardingStatus(iStatus,
											condition2,
											iCallForwardingStatus2V1Pckg);
		}
	else
		{
		iSecondCondition = ETrue;
		CTelephony::TCallForwardingCondition condition1;
		condition1 = CTelephony::ECallForwardingNoReply;
		
		// Interrogates the current status of the call forwarding services for second condition
		iTelephony->GetCallForwardingStatus(iStatus,
											condition1,
											iCallForwardingStatus1V1Pckg);
		}
	SetActive();
	}

/**
Constructor.

@param aController Pointer to an MExecAsync object passed to constructor of 
                   CISVAPIBase
*/
CCallForwardingStatus::CCallForwardingStatus(MExecAsync* aController)
	: CISVAPIAsync(aController, KCallForwardingStatus),
	  iCallForwardingStatus1V1Pckg(iCallForwardingStatus1V1),
	  iCallForwardingStatus2V1Pckg(iCallForwardingStatus2V1)
	{
	// Empty method
	}

/**
Second phase constructor.
*/
void CCallForwardingStatus::ConstructL()
	{
	iSecondCondition = EFalse;
	}

/**
Checks the status of the active object and displays the current call forwarding
status if there is no error.
*/
void CCallForwardingStatus::RunL()
	{
	if(iStatus != KErrNone)
		{
		iConsole->Printf(KError);
		
		// Print the error status code
		iConsole->Printf(_L("%d\n"), iStatus.Int());
		}
	else
		{
		if (!iSecondCondition)
			{
			iConsole->Printf(KCallForwardingStatusMsg);
			switch(iCallForwardingStatus1V1.iCallForwarding)
				{
			case CTelephony::EStatusActive:
				iConsole->Printf(_L("Call forwarding status 1 is active.\n"));
				break;
			case CTelephony::ENotActive:
				iConsole->Printf(_L("Call forwarding status 2 is not active.\n"));
				break;
			case CTelephony::ENotProvisioned:
				iConsole->Printf(_L("Call forwarding status 3 is not provisioned.\n"));
				break;
			case CTelephony::ENotAvailable:
				iConsole->Printf(_L("Call forwarding status 4 is not available.\n"));
				break;
			case CTelephony::EUnknown:
			default:
				iConsole->Printf(_L("Call forwarding status 5 is unknown.\n"));
				break;
				}
			switch(iCallForwardingStatus2V1.iCallForwarding)
				{
			case CTelephony::EStatusActive:
				iConsole->Printf(_L("Call forwarding status 2 is active.\n"));
				break;
			case CTelephony::ENotActive:
				iConsole->Printf(_L("Call forwarding status 2 is not active.\n"));
				break;
			case CTelephony::ENotProvisioned:
				iConsole->Printf(_L("Call forwarding status 2 is not provisioned.\n"));
				break;
			case CTelephony::ENotAvailable:
				iConsole->Printf(_L("Call forwarding status 2 is not available.\n"));
				break;
			case CTelephony::EUnknown:
			default:
				iConsole->Printf(_L("Call forwarding status 2 is unknown.\n"));
				break;
				}
			ExampleComplete();
			}
		else
			{
			ExampleNotify();
			}
		}
	}

/**
Cancels the asynchronous call to CTelephony::GetCallForwardingStatus().
*/
void CCallForwardingStatus::DoCancel()
	{
	// Cancels an outstanding asynchronous request.
	iTelephony->CancelAsync(CTelephony::EGetCallForwardingStatusCancel);
	}

