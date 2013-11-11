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

/**
 @file
*/

#include "CCallStatus.h"

/**
Factory constructor.

@param  aController Pointer to MExecAsync object passed to constructor of 
                    CISVAPIBase
@param  aCallId	Caller ID to retrieve the status                     
@return             Instance of CCallStatus class
*/
CCallStatus* CCallStatus::NewL(MExecAsync* aController, CTelephony::TCallId aCallId)
	{
	CCallStatus* self = new(ELeave) CCallStatus(aController, aCallId);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Default destructor.
Cancels outstanding requests.
*/
CCallStatus::~CCallStatus()
	{
	Cancel();
	}

/**
Gets the current call status, stores it in the iCallStatusV1Pckg package and
prints it to the console.
*/
void CCallStatus::DoStartRequestL()
	{
	// Retrieves the status of the selected call specified by the argument
	iTelephony->GetCallStatus(iCallId, iCallStatusV1Pckg);
	switch(iCallStatusV1.iStatus)
		{
	case CTelephony::EStatusRinging:
		iConsole->Printf(_L("RING RING RING\n"));
		break;
	case CTelephony::EStatusConnected:
		iConsole->Printf(_L("Call Status: Connected\n"));
		break;
	case CTelephony::EStatusConnecting:
		iConsole->Printf(_L("Call Status: Connecting\n"));
		break;
	case CTelephony::EStatusAnswering:
		iConsole->Printf(_L("Call Status: Answering\n"));
		break;
	case CTelephony::EStatusIdle:
		iConsole->Printf(_L("Call Status: Idle\n"));
		break;
	case CTelephony::EStatusDisconnecting:
		iConsole->Printf(_L("Call Status: Disconnecting\n"));
		break;
	default:
		iConsole->Printf(_L("Call status changed\n"));
		break;
		}
	ExampleComplete();
	}

/**
Default constructor.

@param aController Pointer to an MExecAsync object passed to constructor of
                   CISVAPIBase
@param  aCallId	Caller ID to retrieve the status                     
*/
CCallStatus::CCallStatus(MExecAsync* aController,
						 CTelephony::TCallId aCallId)
	: CISVAPIAsync(aController, KCallStatus),
	  iCallStatusV1Pckg(iCallStatusV1),
	  iCallId(aCallId)
	{
	// Empty method
	}

/**
Second phase constructor.
*/
void CCallStatus::ConstructL()
	{
	// Empty method
	}

/**
Checks the status of the active object and prints the call status to the
console if there is no error.
*/
void CCallStatus::RunL()
	{
	CTelephony::TCallStatus CallStatus = iCallStatusV1.iStatus;
	// Print Call Info
	if(iStatus != KErrNone)
		{
		iConsole->Printf(KError);
		
		// Print the error status code
		iConsole->Printf(_L("%d\n"), iStatus.Int());
		}
	else
		{
		switch(CallStatus)
			{
		case CTelephony::EStatusRinging:
			iConsole->Printf(_L("RING RING RING\n"));
			break;
		case CTelephony::EStatusConnected:
			iConsole->Printf(_L("Call Status: Connected\n"));
			break;
		case CTelephony::EStatusConnecting:
			iConsole->Printf(_L("Call Status: Connecting\n"));
			break;
		case CTelephony::EStatusAnswering:
			iConsole->Printf(_L("Call Status: Answering\n"));
			break;
		case CTelephony::EStatusIdle:
			iConsole->Printf(_L("Call Status: Idle\n"));
			break;
		case CTelephony::EStatusDisconnecting:
			iConsole->Printf(_L("Call Status: Disconnecting\n"));
			break;
		default:
			iConsole->Printf(_L("Call status changed\n"));
			break;
			}
		if (CallStatus == CTelephony::EStatusIdle)
			{
			ExampleNotify();
			}
		else
			{
			RequestNotificationL();
			}
		}
	}

/**
Requests to receive notifications of change in the call status
*/
void CCallStatus::DoRequestNotificationL()
	{
	// Panic if this object is already performing an asynchronous operation. 
	// Application will panic if you SetActive() an already active object.
	_LIT( KNotifyPanic, "CCallStatus Notify Method" );
	__ASSERT_ALWAYS( !IsActive(), User::Panic( KNotifyPanic, 1 ));
	iRequestNotify = ETrue;
	CTelephony::TNotificationEvent iEvent;
	switch(iCallId)
		{
	case CTelephony::EISVCall1:
		iEvent = CTelephony::EOwnedCall1StatusChange;
		break;
	case CTelephony::EISVCall2:
		iEvent = CTelephony::EOwnedCall2StatusChange;
		break;
	default:
		// We have not been given a valid call ID
		return;
		}
	
	// Registers interest in receiving change notifications for events.
	iTelephony->NotifyChange(iStatus, iEvent, iCallStatusV1Pckg);
	SetActive();
	}

/**
Cancel asynchronous request to CTelephony::GetCallStatus()
*/
void CCallStatus::DoCancel()
	{
	switch(iCallId)
		{
	case CTelephony::EISVCall1:
		iTelephony->CancelAsync(CTelephony::EOwnedCall1StatusChangeCancel);
		break;
	case CTelephony::EISVCall2:
		iTelephony->CancelAsync(CTelephony::EOwnedCall2StatusChangeCancel);
		break;
	default:
		// We have not been given a valid call ID
		break;
		}
	}
