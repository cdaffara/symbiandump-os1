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


#include "CBatteryInfo.h"

/**
Factory constructor.

@param  aController Pointer to MExecAsync object passed to constructor of 
                    CISVAPIBase
@return             Instance of CBatteryInfo class
*/
CBatteryInfo* CBatteryInfo::NewL(MExecAsync* aController)
	{
	CBatteryInfo* self = new(ELeave) CBatteryInfo(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor.
Cancels outstanding requests.
*/
CBatteryInfo::~CBatteryInfo()
	{
	Cancel();
	}

/**
Gets battery information and stores it in the iBatteryInfoV1Pckg.
*/
void CBatteryInfo::DoStartRequestL()
	{
	_LIT( KNotifyPanic, "CBatteryInfo Get Method" );
	__ASSERT_ALWAYS( !IsActive(), User::Panic( KNotifyPanic, 1 ));
	iRequestNotify = EFalse;
	
	// Retrieves the status and charge level of the phone battery.
	iTelephony->GetBatteryInfo(iStatus, iBatteryInfoV1Pckg);
	SetActive();
	}

/**
Constructor.

@param aController Pointer to an MExecAsync object passed to constructor of 
                   CISVAPIBase
*/
CBatteryInfo::CBatteryInfo(MExecAsync* aController)
	: CISVAPIAsync(aController, KBatteryInfo),
	  iBatteryInfoV1Pckg(iBatteryInfoV1)
	{
	// Empty method
	}

/**
Second phase constructor.
*/
void CBatteryInfo::ConstructL()
	{
	// Empty method
	}

/**
Checks the status of the active object and prints the battery information to
the console if there is no error.
*/
void CBatteryInfo::RunL()
	{
	CTelephony::TBatteryInfoV1 BatteryInfo = iBatteryInfoV1;

	if(iStatus != KErrNone)
		{
		iConsole->Printf(KError);
		
		// Print the error status code
		iConsole->Printf(_L("%d\n"), iStatus.Int());
		}
	else
		{
		// Print Battery Info
		CTelephony::TBatteryStatus batteryStatus = BatteryInfo.iStatus;
		TUint chargeLevel = BatteryInfo.iChargeLevel;
		if (iRequestNotify)
			{
			iConsole->ClearScreen();
			iConsole->Printf(_L("~*THIS IS A NOTIFICATION*~\n"));
			}
		iConsole->Printf(KBatteryInfoMsg);
		iConsole->Printf(_L("Charge is %d\n"), chargeLevel);
		switch (batteryStatus)
			{
		case CTelephony::EPowerStatusUnknown:
			iConsole->Printf(_L("EPowerStatusUnknown %d\n"), batteryStatus);
			break;
		case CTelephony::EPoweredByBattery:
			iConsole->Printf(_L("EPoweredByBattery %d\n"), batteryStatus);
			break;
		case CTelephony::EBatteryConnectedButExternallyPowered:
			iConsole->Printf(_L("EBatteryConnectedButExternallyPowered %d\n"), batteryStatus);
			break;
		case CTelephony::ENoBatteryConnected:
			iConsole->Printf(_L("ENoBatteryConnected %d\n"), batteryStatus);
			break;
		case CTelephony::EPowerFault:
			iConsole->Printf(_L("EPowerFault %d\n"), batteryStatus);
			break;
		default:
			iConsole->Printf(KError);
			}
		}
		if (iRequestNotify)
			{
			DoRequestNotificationL();
			}
		else
			{
			ExampleComplete();
			}
	}

/**
Requests to receive notifications of change in the battery information.
*/
void CBatteryInfo::DoRequestNotificationL()
	{
	// Panics if this object is already performing an asynchronous operation.
	// Application will panic if you call SetActive() on an already active object.
	_LIT( KNotifyPanic, "CBatteryInfo Notify Method" );
	__ASSERT_ALWAYS( !IsActive(), User::Panic( KNotifyPanic, 1 ));
	iRequestNotify = ETrue;
	
	// Registers interest in receiving change notifications for events.
	iTelephony->NotifyChange(	iStatus,
								CTelephony::EBatteryInfoChange,
								iBatteryInfoV1Pckg );
	SetActive();
	}

/**
Cancels asynchronous request to CTelephony::GetBatteryInfo().
*/
void CBatteryInfo::DoCancel()
	{
	// Cancels an outstanding asynchronous request.
	TInt error = iTelephony->CancelAsync(CTelephony::EBatteryInfoChangeCancel);
	}

