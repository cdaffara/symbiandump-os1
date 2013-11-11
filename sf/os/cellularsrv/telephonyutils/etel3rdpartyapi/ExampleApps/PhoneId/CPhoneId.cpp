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


#include "CPhoneId.h"

/**
Factory constructor.

@param  aController Pointer to MExecAsync object passed to constructor of 
                    CISVAPIBase
@return             Instance of CPhoneId class
*/
CPhoneId* CPhoneId::NewL(MExecSync* aController)
	{
	CPhoneId* self = new(ELeave) CPhoneId(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor.
Cancels outstanding requests
*/
CPhoneId::~CPhoneId()
	{
	Cancel();
	}

/**
Gets the phone ID by calling 3rd party API function CTelephony::GetPhoneId().
*/
void CPhoneId::DoStartRequestL()
	{
	// Retrieves the model information and unique identification of the mobile device.
	iTelephony->GetPhoneId(iStatus, iPhoneIdV1Pckg);
	SetActive();
	}

/**
Default constructor.

@param aController Pointer to MExecAsync object passed to constructor of 
                   CISVAPIBase
*/
CPhoneId::CPhoneId(MExecSync* aController)
	: CISVAPISync(aController, KPhoneId),
	  iPhoneIdV1Pckg(iPhoneIdV1)
	{
	// Empty method
	}

/**
Second phase constructor.
*/
void CPhoneId::ConstructL()
	{
	// Empty method
	}

/**
Prints phone information to the console.
*/
void CPhoneId::RunL()
	{
	if(iStatus != KErrNone)
		{
		iConsole->Printf(KError);
		
		// Print the status error code
		iConsole->Printf(_L("%d\n"), iStatus.Int());
		}
	else
		{
		// Active object completed with no error, therefore print phone
		// information to the console.
		TBuf<CTelephony::KPhoneManufacturerIdSize> manufacturer = iPhoneIdV1.iManufacturer;
		TBuf<CTelephony::KPhoneModelIdSize> model = iPhoneIdV1.iModel;
		TBuf<CTelephony::KPhoneSerialNumberSize> serialNumber = iPhoneIdV1.iSerialNumber;
		iConsole->Printf(KPhoneIdMsg);
		
		// Print phone manufacturer
		iConsole->Printf(manufacturer);
		iConsole->Printf(KNewLine);
		
		// Print phone model
		iConsole->Printf(model);
		iConsole->Printf(KNewLine);
		
		// Print the IMEI or ESN serial number
		iConsole->Printf(serialNumber);
		iConsole->Printf(KNewLine);
		ExampleComplete();
		}
	}

/**
Cancels asynchronous request to CTelephony::GetPhoneId().
*/
void CPhoneId::DoCancel()
	{
	// Cancels an outstanding asynchronous request.
	iTelephony->CancelAsync(CTelephony::EGetPhoneIdCancel);
	}
