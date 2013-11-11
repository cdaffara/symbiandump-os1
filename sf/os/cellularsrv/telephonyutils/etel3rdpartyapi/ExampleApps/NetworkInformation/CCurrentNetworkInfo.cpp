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


#include "CCurrentNetworkInfo.h"

/**
Factory constructor.

@param  aController Pointer to MExecAsync object passed to constructor of 
                    CISVAPIBase
@return             Instance of CCurrentNetworkInfo class
*/
CCurrentNetworkInfo* CCurrentNetworkInfo::NewL(MExecAsync* aController)
	{
	CCurrentNetworkInfo* self = new(ELeave) CCurrentNetworkInfo(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor.
Cancels outstanding requests.
*/
CCurrentNetworkInfo::~CCurrentNetworkInfo()
	{
	Cancel();
	}

/**
Gets the current network information and stores it in the
iCurrentNetworkInfoV1Pckg package.
*/
void CCurrentNetworkInfo::DoStartRequestL()
	{
	_LIT( KNotifyPanic, "CCurrentNetworkInfo Notify Method" );
    __ASSERT_ALWAYS( !IsActive(), User::Panic( KNotifyPanic, 1 ));
    iRequestNotify = EFalse;
   
    // Retrieves over-the-air network information about the currently registered mobile network.
	iTelephony->GetCurrentNetworkInfo(iStatus, iCurrentNetworkInfoV1Pckg);
	SetActive();
	}

/**
Constructor.

@param aController Pointer to an MExecAsync object passed to constructor of
                   CISVAPIBase
*/
CCurrentNetworkInfo::CCurrentNetworkInfo(MExecAsync* aController)
	: CISVAPIAsync(aController, KNetworkInfo),
	  iCurrentNetworkInfoV1Pckg(iCurrentNetworkInfoV1)
	{
	// Empty method
	}

/**
Second phase constructor.
*/
void CCurrentNetworkInfo::ConstructL()
	{
	// Empty method
	}
	
/**
Checks the status of the active object and prints the network information to
the console if there is no error.
*/
void CCurrentNetworkInfo::RunL()
	{
	if(iStatus != KErrNone)
		{
		iConsole->Printf(KError);
		
		// Print the error status code
		iConsole->Printf(_L("%d\n"), iStatus.Int());
		}
	else
		{
		TBuf<30> theStatus;
		if (iRequestNotify)
	    	{
	    	iConsole->ClearScreen();
	    	iConsole->Printf(_L("~*THIS IS A NOTIFICATION*~\n"));
	    	}
		iConsole->Printf(KNetworkInfoMsg);
		iConsole->Printf(iCurrentNetworkInfoV1.iDisplayTag);
		iConsole->Printf(KNewLine);
		switch (iCurrentNetworkInfoV1.iStatus)
			{
		case CTelephony::ENetworkStatusUnknown:
			theStatus.Append(_L("ENetworkStatusUnknown\n"));
			break;
		case CTelephony::ENetworkStatusAvailable:
			theStatus.Append(_L("ENetworkStatusUnavailable\n"));
			break;
		case CTelephony::ENetworkStatusCurrent:
			theStatus.Append(_L("ENetworkStatusCurrent\n"));
			break;
		case CTelephony::ENetworkStatusForbidden:
			theStatus.Append(_L("ENetworkStatusForbidden\n"));
			break;
			}
		iConsole->Printf(theStatus);
		if(iCurrentNetworkInfoV1.iAccess)
			{
			// Make any calls to change Phone profile here using iCellId parameter
			iConsole->Printf((_L("The Cell Id is : %d\n")), iCurrentNetworkInfoV1.iCellId);
			iConsole->Printf((_L("The Area Code is : %d\n")), iCurrentNetworkInfoV1.iLocationAreaCode);
			}
		if (iRequestNotify)
			{
			RequestNotificationL();
			}
		else
			{
			ExampleComplete();
			}
		}
	}

/**
Requests to receive notifications of change in the current network information.
*/
void CCurrentNetworkInfo::DoRequestNotificationL()
   {
	// Panic if this object is already performing an asynchronous operation. 
	// Application will crash if you SetActive() an already active object.
	_LIT( KNotifyPanic, "CCurrentNetworkInfo Notify Method" );
	__ASSERT_ALWAYS( !IsActive(), User::Panic( KNotifyPanic, 1 ));
	iRequestNotify = ETrue;
	
	// Registers interest in receiving change notifications for events.
	iTelephony->NotifyChange(	iStatus,
								CTelephony::ECurrentNetworkInfoChange,
								iCurrentNetworkInfoV1Pckg );
	SetActive();
	}
	
/**
Cancels asynchronous request to CTelephony::GetCurrentNetworkInfo().
*/
void CCurrentNetworkInfo::DoCancel()
	{
	iRequestNotify = EFalse;
	
	// Cancels an outstanding asynchronous request.
	iTelephony->CancelAsync(CTelephony::ECurrentNetworkInfoChangeCancel);
	}

