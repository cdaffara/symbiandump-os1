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
Destructor.
*/
CNetworkRegInfo::~CNetworkRegInfo()
	{
	Cancel();
	}

/**
Gets the network registration status and stores it in the iNetworkRegV1Pckg 
package.
*/
void CNetworkRegInfo::DoStartRequestL()
	{
    iRequestNotify = EFalse;
   
    // Retrieves the current network registration status
	iTelephony->GetNetworkRegistrationStatus(iStatus, iNetworkRegV1Pckg);
	SetActive();
	}

/**
Constructor.

@param aController Pointer to an MExecAsync object passed to constructor of
                   CISVAPIBase
*/
CNetworkRegInfo::CNetworkRegInfo(MExecAsync* aController)
	: CISVAPIAsync(aController, KNetworkRegInfo),
	  iNetworkRegV1Pckg(iNetworkRegV1)
	{
	// Empty method
	}

/**
Second phase constructor.
*/
void CNetworkRegInfo::ConstructL()
	{
	iRequestNotify = EFalse;
	}

/**
Checks status of the active object and prints the network registration status
to the console.
*/
void CNetworkRegInfo::RunL()
	{
	if(iStatus != KErrNone)
		{
		iConsole->Printf(KError);
		
		// Print the output to console if there is no error
		iConsole->Printf(_L("%d\n"), iStatus.Int());
		}
	else
		{
		TBuf<30> theStatus;
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
Requests to receive notifications of changes in the network registration
status.
*/
void CNetworkRegInfo::DoRequestNotificationL()
	{
	// Panic if this object is already performing an asynchronous operation.
	// Application will panic if you call SetActive() on an already active object.
	_LIT( KNotifyPanic, "CNetworkReg Notify Method" );
	__ASSERT_ALWAYS( !IsActive(), User::Panic( KNotifyPanic, 1 ));
	iRequestNotify = ETrue;
	
	// Registers interest in receiving change notifications for events.
	iTelephony->NotifyChange(	iStatus,
								CTelephony::ENetworkRegistrationStatusChange,
								iNetworkRegV1Pckg );
	SetActive();
	}
	
/**
Cancels asynchronous request to CTelephony::GetNetworkRegistrationStatus()
*/
void CNetworkRegInfo::DoCancel()
	{
	iRequestNotify = EFalse;
	
	// Cancels an outstanding asynchronous request
	iTelephony->CancelAsync(CTelephony::ENetworkRegistrationStatusChangeCancel);
	}
