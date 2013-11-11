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
Default destructor.  Cancels outstanding requests.
*/
CCallStatus::~CCallStatus()
	{
	Cancel();
	}

/**
Gets the call status of call specified in iCallId and displays output
to console.
*/
void CCallStatus::DoStartRequestL()
	{
	iTelephony->GetCallStatus(iCallId, iCallStatusV1Pckg);
	iConsole->Printf((_L("%d ")), iCallId);
	CTelephony::TCallStatus callStatus = iCallStatusV1.iStatus;
	switch(callStatus)
			{
	case CTelephony::EStatusRinging:
		iConsole->Printf(_L("RING RING RING\n"));
		break;
	case CTelephony::EStatusConnected:
		iConsole->Printf(_L("Call Status connected\n"));
		break;
	case CTelephony::EStatusConnecting:
		iConsole->Printf(_L("Call Status connecting\n"));
		break;
	case CTelephony::EStatusAnswering:
		iConsole->Printf(_L("Call Status Answering\n"));
		break;
	case CTelephony::EStatusIdle:
		iConsole->Printf(_L("Call Status Idle\n"));
		break;
	case CTelephony::EStatusDisconnecting:
		iConsole->Printf(_L("Call Status Disconnecting\n"));
		break;
	case CTelephony::EStatusHold:
		iConsole->Printf(_L("Call Status on Hold\n"));
		break;
	default:
		iConsole->Printf((_L("Call status changed is %d \n")), callStatus);
		break;
			}

	if(callStatus == CTelephony::EStatusConnected)
		{
		ExampleComplete();
		}
	else
		{
		RequestNotificationL();
		}
	}

/**
Default constructor.

@param aController Pointer to MExecAsync object passed to constructor of 
                   CISVAPIBase.
@param aCallId     Call ID of the call whose status will be retrieved.
*/
CCallStatus::CCallStatus(MExecAsync* aController, CTelephony::TCallId aCallId)
	: CISVAPIAsync(aController, KCallStatus),
	iCallId(aCallId),
	iCallStatusV1Pckg(iCallStatusV1)
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
Checks status and prints output to the console.
*/
void CCallStatus::RunL()
	{
	// Print Call Info
	if(iStatus != KErrNone)
		{
		// Return error
		iConsole->Printf(KError);
		
		// Print the error status code
		iConsole->Printf(_L("%d\n"), iStatus.Int());
		}
	else
		{
		// Print the console output if there is no error
		iConsole->Printf((_L("%d ")), iCallId);
		CTelephony::TCallStatus CallStatus = iCallStatusV1.iStatus;
		switch(CallStatus)
			{
		case CTelephony::EStatusRinging:
			iConsole->Printf(_L("RING RING RING\n"));
			break;
		case CTelephony::EStatusConnected:
			iConsole->Printf(_L("Call Status connected\n"));
			break;
		case CTelephony::EStatusConnecting:
			iConsole->Printf(_L("Call Status connecting\n"));
			break;
		case CTelephony::EStatusAnswering:
			iConsole->Printf(_L("Call Status Answering\n"));
			break;
		case CTelephony::EStatusIdle:
			iConsole->Printf(_L("Call Status Idle\n"));
			break;
		case CTelephony::EStatusDisconnecting:
			iConsole->Printf(_L("Call Status Disconnecting\n"));
			break;
		case CTelephony::EStatusHold:
			iConsole->Printf(_L("Call Status on Hold\n"));
			break;
		default:
			iConsole->Printf((_L("Call status is %d \n")), CallStatus);
			break;
			}
		if (CallStatus == CTelephony::EStatusDisconnecting)
			{
			ExampleNotify();
			}
		else if(CallStatus == CTelephony::EStatusConnected)
			{
			ExampleComplete();
			}
		else
			{
			RequestNotificationL();
			}
		}
	}

/**
Registers interest in receiving change notifications of the call status
by calling CTelephony::NotifyChange.
*/
void CCallStatus::DoRequestNotificationL()
	{
	// Panic if this object is already performing an asynchronous
	// operation. Application will crash if you SetActive() an already
	// active object.
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
		break;
		}

	// Notify if there is any change in call status
	iTelephony->NotifyChange(iStatus, iEvent, iCallStatusV1Pckg);
	SetActive();
	}

/**
Cancels a pending asychronous request depending on iCallId.
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
