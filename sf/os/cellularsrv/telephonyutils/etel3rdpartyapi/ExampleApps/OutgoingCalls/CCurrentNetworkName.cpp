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

#include "CCurrentNetworkName.h"

/**
Factory constructor.

@param  aController Pointer to MExecAsync object passed to constructor of 
                    CISVAPIBase
@return             Instance of CCurrentNetworkName class
*/
CCurrentNetworkName* CCurrentNetworkName::NewL(MExecAsync* aController)
	{
	CCurrentNetworkName* self = new(ELeave) CCurrentNetworkName(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor.
Cancels outstanding requests.
*/
CCurrentNetworkName::~CCurrentNetworkName()
	{
	Cancel();
	}

/**
Gets the current network name and stores it in the iCurrentNetworkNameV1Pckg
package.
*/
void CCurrentNetworkName::DoStartRequestL()
	{
	_LIT( KNotifyPanic, "CCurrentNetworkName Get Method" );
	__ASSERT_ALWAYS( !IsActive(), User::Panic( KNotifyPanic, 1 ));
	iRequestNotify = EFalse;
	
	// Retrieves ICC-stored information about the (preferred) name of the currently registered mobile network.
	iTelephony->GetCurrentNetworkName(iStatus, iCurrentNetworkNameV1Pckg);
	SetActive();
	}

/**
Default constructor.

@param aController Pointer to an MExecAsync object passed to constructor of 
                   CISVAPIBase
*/
CCurrentNetworkName::CCurrentNetworkName(MExecAsync* aController)
	: CISVAPIAsync(aController, KNetworkName),
	  iCurrentNetworkNameV1Pckg(iCurrentNetworkNameV1)
	{
	// Empty method
	}

/**
Second phase constructor.
*/
void CCurrentNetworkName::ConstructL()
	{
	// Empty method
	}

/**
Checks the status of the active object and prints the network name to the
console if there is no error.
*/
void CCurrentNetworkName::RunL()
	{
	if(iStatus != KErrNone)
		{
		iConsole->Printf(KError);
		
		// Print the error status code
		iConsole->Printf(_L("%d\n"), iStatus.Int());
		}
	else
		{
		// Print the console message if there is no error
		iConsole->Printf(KNetworkNameMsg);
		iConsole->Printf(iCurrentNetworkNameV1.iNetworkName);
		iConsole->Printf(KNewLine);
		ExampleComplete();
		}
	}

/**
Cancels asynchronous request to CTelephony::GetCurrentNetworkName().
*/
void CCurrentNetworkName::DoCancel()
	{
	// Cancels an outstanding asynchronous request.
	iTelephony->CancelAsync(CTelephony::EGetCurrentNetworkNameCancel);
	}

