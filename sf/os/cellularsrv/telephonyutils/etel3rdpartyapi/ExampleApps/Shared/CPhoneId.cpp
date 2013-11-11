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
// CPhoneApp.cpp
// 
//

#include "CPhoneId.h"

/**
TODO These Literals would be read from some file which the application would
check the result of GetPhoneId() against each time the App was run to make sure
it was registered.
*/
_LIT(KRegisteredManufacturer, "Generic");
_LIT(KRegisteredModel, "123");
_LIT(KRegisteredSerialNumber, "1234567890");

/**
Factory constructor.

@param  aController Pointer to MExecAsync object passed to constructor of 
                    CISVAPIBase
@return             Instance of CPhoneId class
*/
CPhoneId* CPhoneId::NewL(MExecAsync* aController)
	{
	CPhoneId* self = new(ELeave) CPhoneId(aController);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Destructor.
Cancels outstanding requests.
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
Constructor.

@param aController Pointer to MExecAsync object passed to constructor of 
CISVAPIBase
*/
CPhoneId::CPhoneId(MExecAsync* aController)
	: CISVAPIAsync(aController, KPhoneId),
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
Checks the status of the active object and whether the application is registered to
the mobile phone and displays output to the console.
*/
void CPhoneId::RunL()
	{
  	// Print Phone Info
	if(iStatus != KErrNone)
		{
		iConsole->Printf(KError);
		
		// Print the error status code
		iConsole->Printf(_L("%d\n"), iStatus.Int());
		}
	else
		{
		if (AppRegistered())
			{
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
		else
			{
			// Application isn't registered!

			// Sim Tsy Config doesn't match Lits defined in CPhoneId
			// Could end Application here by calling Terminate().
			// Wait for 5 seconds then continue by calling ExampleComplete().
			iConsole->Printf(_L("Application not registered to phone!\n"));
			iConsole->Printf(_L("Recify this to remove this delay\n"));
			User::After(5000000);
			ExampleComplete();
			}
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

/**
Compares data retrieved from phone with registered values to check the application is 
registered.

@return Returns ETrue if phone data matches registered values otherwise EFalse.
*/
TBool CPhoneId::AppRegistered()
	{
	// Values on phone
	TBuf<CTelephony::KPhoneManufacturerIdSize> manufacturer = iPhoneIdV1.iManufacturer;
	TBuf<CTelephony::KPhoneModelIdSize> model = iPhoneIdV1.iModel;
	TBuf<CTelephony::KPhoneSerialNumberSize> serialNumber = iPhoneIdV1.iSerialNumber;

	// Registered values
	TBuf<CTelephony::KPhoneModelIdSize> registeredModel(KRegisteredModel);
	TBuf<CTelephony::KPhoneSerialNumberSize> registeredSerialNumber(KRegisteredSerialNumber);
	TBuf<CTelephony::KPhoneManufacturerIdSize> registeredManufacturer(KRegisteredManufacturer);

	return	registeredModel == model &&
			registeredManufacturer == manufacturer &&
			registeredSerialNumber == serialNumber ;
	}
