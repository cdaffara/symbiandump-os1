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


#include "CLineStatus.h"
#include <simtsy.h>

/**
Factory constructor.

@param  aController Pointer to MExecAsync object passed to constructor of 
                    CISVAPIBase
@return             Instance of CLineStatus class
*/
CLineStatus* CLineStatus::NewL(MExecAsync* aController)
	{
	CLineStatus* self = new(ELeave) CLineStatus(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor.
Cancels outstanding requests.
*/
CLineStatus::~CLineStatus()
	{
	Cancel();
	}

/**
Gets the line status of the voice line, stores the result in
iLineStatusV1Pckg and displays the result to the console.
*/
void CLineStatus::DoStartRequestL()
	{
    iRequestNotify = EFalse;
    CTelephony::TPhoneLine line = CTelephony::EVoiceLine;
   
   // Retrieves the status of the line selected by the specified argument
	iTelephony->GetLineStatus(line, iLineStatusV1Pckg);
	CTelephony::TCallStatus voiceLineStatus = iLineStatusV1.iStatus;
	switch (voiceLineStatus)
		{
	case CTelephony::EStatusRinging:
		iConsole->Printf(_L("RING RING RING\n"));
		break;
	case CTelephony::EStatusConnected:
		iConsole->Printf(_L("Line Status: Connected\n"));
		break;
	case CTelephony::EStatusConnecting:
		iConsole->Printf(_L("Line Status: Connecting\n"));
		break;
	case CTelephony::EStatusAnswering:
		iConsole->Printf(_L("Line Status: Answering\n"));
		break;
	case CTelephony::EStatusIdle:
		iConsole->Printf(_L("Line Status: Idle\n"));
		break;
	case CTelephony::EStatusDisconnecting:
		iConsole->Printf(_L("Line Status: Disconnecting\n"));
		break;
	case CTelephony::EStatusHold:
		iConsole->Printf(_L("Line Status: On Hold\n"));
		break;
	default:
		iConsole->Printf(_L("Line status changed.\n"));
		break;
		}
	if (voiceLineStatus == CTelephony::EStatusIdle)
		{
		ExampleNotify();
		}
	else
		{
		if (!IsActive())
			{
			RequestNotificationL();
			}
		}
	}
	
/**
Constructor.

@param aController Pointer to an MExecAsync object passed to constructor of 
                   CISVAPIBase
*/
CLineStatus::CLineStatus(MExecAsync* aController)
	: CISVAPIAsync(aController, KLineStatus),
	  iLineStatusV1Pckg(iLineStatusV1)
	{
	// Empty method
	}

/**
Second phase constructor.
*/
void CLineStatus::ConstructL()
	{
	// Empty method
	}

/**
Checks the status of the active object and prints the voice line status to the 
console if there is no error.
*/
void CLineStatus::RunL()
	{
	CTelephony::TCallStatus voiceLineStatus = iLineStatusV1.iStatus;
	// Print Line Information.
	if(iStatus != KErrNone)
		{
		iConsole->Printf(KError);
		
		// Print the error status code
		iConsole->Printf(_L("%d\n"), iStatus.Int());
		}
	else
		{
		switch (voiceLineStatus)
			{
		case CTelephony::EStatusRinging:
			iConsole->Printf(_L("RING RING RING\n"));
			break;
		case CTelephony::EStatusConnected:
			iConsole->Printf(_L("Line Status: Connected\n"));
			break;
		case CTelephony::EStatusConnecting:
			iConsole->Printf(_L("Line Status: Connecting\n"));
			break;
		case CTelephony::EStatusAnswering:
			iConsole->Printf(_L("Line Status: Answering\n"));
			break;
		case CTelephony::EStatusIdle:
			iConsole->Printf(_L("Line Status: Idle\n"));
			break;
		case CTelephony::EStatusDisconnecting:
			iConsole->Printf(_L("Line Status: Disconnecting\n"));
			break;
		case CTelephony::EStatusHold:
			iConsole->Printf(_L("Line Status: On Hold\n"));
			break;
		default:
			iConsole->Printf(_L("Line status changed.\n"));
			break;
			}
		if (voiceLineStatus == CTelephony::EStatusIdle
				||  voiceLineStatus == CTelephony::EStatusHold)
			{
			ExampleNotify();
			}
		else if (voiceLineStatus == CTelephony::EStatusRinging)
			{
			ExampleComplete();
			}
		else
			{
			if (!IsActive())
				{
				RequestNotificationL();
				}
			}
		}
	}

/**
Requests to receive notifications of change in the voice line
status.
*/
void CLineStatus::DoRequestNotificationL()
	{
	// Panic if this object is already performing an asynchronous operation. 
	// Application will panic if you call SetActive() on an already active object.
	_LIT( KNotifyPanic, "CLineStatus Notify Method" );
	__ASSERT_ALWAYS( !IsActive(), User::Panic( KNotifyPanic, 1 ));
	iRequestNotify = ETrue;
	
	// Registers interest in receiving change notifications for events.
	iTelephony->NotifyChange(iStatus,
								CTelephony::EVoiceLineStatusChange,
								iLineStatusV1Pckg );
	SetActive();
	}

/**
Cancels asynchronous request to CTelephony::GetLineStatus()
*/
void CLineStatus::DoCancel()
	{
	// Cancels an outstanding asynchronous request.
	iTelephony->CancelAsync(CTelephony::EVoiceLineStatusChangeCancel);
	}

