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

#include "CGetIndicator.h"

/**
Factory constructor.

@param  aController Pointer to MExecAsync object passed to constructor of 
                    CISVAPIBase
@return             Instance of CGetIndicator class
*/
CGetIndicator* CGetIndicator::NewL(MExecAsync* aController)
	{
	CGetIndicator* self = new(ELeave) CGetIndicator(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor.
Cancels outstanding requests.
*/
CGetIndicator::~CGetIndicator()
	{
	Cancel();
	}

/**
Gets the indicator information (information regarding whether the battery
charger is connected and stores it in the iIndicatorV1Pckg package.
*/
void CGetIndicator::DoStartRequestL()
	{
	_LIT(KDummyAnswerPanic, "CGetIndicator Get Method");
	__ASSERT_ALWAYS(!IsActive(), User::Panic(KDummyAnswerPanic, 1));
	iRequestNotify = EFalse;
	
	// Retrieves the battery charging indicator, the network availability indicator and call-in-progress indicator.
	iTelephony->GetIndicator(iStatus, iIndicatorV1Pckg);
	SetActive();
	}

/**
Default constructor.

@param aController Pointer to an MExecAsync object passed to constructor of
                   CISVAPIBase
*/
CGetIndicator::CGetIndicator(MExecAsync* aController)
	: CISVAPIAsync(aController, KGetIndicator),
	  iIndicatorV1Pckg(iIndicatorV1)
	{
	// Empty method
	}

/**
Second phase constructor.
*/
void CGetIndicator::ConstructL()
	{
	// Empty method
	}

/**
Checks the status of the active object and prints the inidcator information to
the console if there is no error.
*/
void CGetIndicator::RunL()
	{
	// Print Indicator Info
	if(iStatus != KErrNone)
		{
		iConsole->Printf(KError);
		
		// Print the error status code
		iConsole->Printf(_L("%d\n"), iStatus.Int());
		}
	else
		{
      if(iIndicatorV1.iCapabilities & CTelephony::KIndChargerConnected)
			{
			// We can detect when a charger is connected
			if(iIndicatorV1.iIndicator & CTelephony::KIndChargerConnected)
				{
				// Charger is connected
				iConsole->Printf(_L("Charger connected\n"));
				}
				else
				{
				// Charger is not connected
				iConsole->Printf(_L("Charger not connected\n"));
				}
			}
		else
			{
			// We do not know whether a charger is connected.
			iConsole->Printf(_L("Status of charger unknown\n"));
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
	}

/**
Requests to receive notifications of change in the indicator information.
*/
void CGetIndicator::DoRequestNotificationL()
   {
	// Panic if this object is already performing an asynchronous operation.
	// Application will panic if you call SetActive() on an already active object.
	_LIT( KNotifyPanic, "CGetIndicator Notify Method" );
	__ASSERT_ALWAYS( !IsActive(), User::Panic( KNotifyPanic, 1 ));
	iRequestNotify = ETrue;
	
	// Registers interest in receiving change notifications for events.
	iTelephony->NotifyChange(	iStatus,
								CTelephony::EIndicatorChange,
								iIndicatorV1Pckg );
	SetActive();
	}

/**
Cancels asynchronous request to CTelephony::GetIndicator().
*/
void CGetIndicator::DoCancel()
	{
	// Cancels an outstanding asynchronous request.
	iTelephony->CancelAsync(CTelephony::EIndicatorChangeCancel);
	}
