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

#include "CNetworkRegInfo.h"

/**
Factory constructor.

@param  aController Pointer to MExecAsync object passed to constructor of
                    CISVAPIBase
@return             Instance of CNetworkRegInfo class
*/
CNetworkRegInfo* CNetworkRegInfo::NewL(MExecAsync* aController)
	{
	CNetworkRegInfo* self = new(ELeave) CNetworkRegInfo(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor.  Calls Cancel() to cancel outstanding requests.
*/
CNetworkRegInfo::~CNetworkRegInfo()
	{
	Cancel();
	}

/**
Kicks off request.
*/
void CNetworkRegInfo::DoStartRequestL()
	{
    iRequestNotify = EFalse;
    
    // Retrieves the current network registration status.
	iTelephony->GetNetworkRegistrationStatus(iStatus, iNetworkRegV1Pckg);
	SetActive();
	}

/**
Constructor called by CNetworkRegInfo::NewL().

@param aController Pointer to MExecAsync object passed to constructor of
                   CISVAPIBase
*/
CNetworkRegInfo::CNetworkRegInfo(MExecAsync* aController)
	: CISVAPIAsync(aController, KNetworkRegInfo),
	  iNetworkRegV1Pckg(iNetworkRegV1)
	{
	// Empty method
	}

/**
Second phase constructor. Sets iRequestNotify to EFalse.
*/
void CNetworkRegInfo::ConstructL()
	{
	iRequestNotify = EFalse;
	}

/**
Prints network registration status to the console.
*/
void CNetworkRegInfo::RunL()
	{
	if(iStatus != KErrNone)
		{
		iConsole->Printf(KError);
		
		// Print the error status code
		iConsole->Printf(_L("%d\n"), iStatus.Int());
		}
	else
		{
		// There is no error, so display the current registration status to the
		// user.
		TBuf<30> theStatus;
		if(iRequestNotify)
			{
			iConsole->ClearScreen();
			iConsole->Printf(_L("*~This is a notifcation\n~*"));
			}
		iConsole->Printf(KNetworkRegMsg);
		switch(iNetworkRegV1.iRegStatus)
			{
		case CTelephony::ERegistrationUnknown:
			theStatus.Append(_L("ERegistrationUnknown\n"));
			break;
		case CTelephony::ENotRegisteredNoService:
			theStatus.Append(_L("ENotRegisteredNoService\n"));
			break;
		case CTelephony::ENotRegisteredEmergencyOnly:
			theStatus.Append(_L("ENotRegisteredEmergencyOnly\n"));
			break;
		case CTelephony::ENotRegisteredSearching:
			theStatus.Append(_L("ENotRegisteredSearching\n"));
			break;
		case CTelephony::ERegisteredBusy:
			theStatus.Append(_L("ERegisteredBusy\n"));
			break;
		case CTelephony::ERegisteredOnHomeNetwork:
			theStatus.Append(_L("ERegisteredOnHomeNetwork\n"));
			break;
		case CTelephony::ERegistrationDenied:
			theStatus.Append(_L("ERegistrationDenied\n"));
			break;
		case CTelephony::ERegisteredRoaming:
			theStatus.Append(_L("ERegisteredRoaming\n"));
			break;
		default:
			break;
	 		}
		iConsole->Printf(theStatus);
		}
	if (iNetworkRegV1.iRegStatus == CTelephony::ERegisteredOnHomeNetwork)
		{
		ExampleNotify();
		}
	else
		{
		RequestNotificationL();
		}
	}

/**
Requests to receive notifications of change in the
network registration information.
*/
void CNetworkRegInfo::DoRequestNotificationL()
	{
	// Panic if this object is already performing an asynchronous operation.
	// Application will panic if you call SetActive() on an already active object.
	_LIT( KNotifyPanic, "CNetworkReg Notify Method" );
	__ASSERT_ALWAYS( !IsActive(), User::Panic( KNotifyPanic, 1 ));
	iRequestNotify = ETrue;
	
	// Notify if there is any change
	iTelephony->NotifyChange(iStatus,
	                         CTelephony::ENetworkRegistrationStatusChange,
	                         iNetworkRegV1Pckg );
	SetActive();
	}

/**
Cancels asynchronous request to CTelephony::GetNetworkRegistrationStatus().
*/
void CNetworkRegInfo::DoCancel()
	{
	iRequestNotify = EFalse;
	
	// Cancels an outstanding asynchronous request.
	iTelephony->CancelAsync(CTelephony::ENetworkRegistrationStatusChangeCancel);
	}
