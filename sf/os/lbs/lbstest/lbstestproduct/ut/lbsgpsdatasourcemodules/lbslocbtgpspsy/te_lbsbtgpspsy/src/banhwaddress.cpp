// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This step connects to the bluetooth registry and 'bans' a bluetooth address
// so that the BT GPS PSY can no longer connect to it. This is useful in tests
// where the connection has to become interrupted and then the BT GPS PSY needs
// to be stopped from reconnecting to that same device.
//



/**
 @file banhwaddress.cpp
 @internalTechnology
*/
#include "banhwaddress.h"
#include "Te_LbsBtGpsPsyDefs.h"
#include <lbs.h>
#include <bt_sock.h>
#include <btmanclient.h>
#include <BtGpsPsyPrivateCRKeys.h>
// CONSTANTS




CBanHwAddrStep::~CBanHwAddrStep()
/**
 * Destructor
 */
	{
	}

CBanHwAddrStep::CBanHwAddrStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KBanHwAddrStep);
	}

TVerdict CBanHwAddrStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	if (TestStepResult()==EPass)
		{
		SetTestStepResult(EPass);
		// Connect to the bt registry
		RBTRegistry btReg;
		RBTRegServ btRegServ;
		User::LeaveIfError(btRegServ.Connect());
		User::LeaveIfError(btReg.Open(btRegServ));

		TPtrC hwAddrPtr;
		// Try to get the address from the registry
		if(!GetStringFromConfig(ConfigSection(),KTe_LbsBtGpsPsySuiteString,hwAddrPtr))
			{
			// Leave if there's any error.
			User::Leave(KErrNotFound);
			}
		TInt64 hwAddrInt(0);
		TLex lex(hwAddrPtr);
		User::LeaveIfError(lex.Val(hwAddrInt, EHex));
		
		TBTDevAddr hwDevAddr = hwAddrInt;
		TRequestStatus status;
		TBTDeviceSecurity devSecurity;
		devSecurity.SetBanned(ETrue);
		CBTDevice* btDevice = CBTDevice::NewL(hwDevAddr);
		CleanupStack::PushL(btDevice);
		btDevice->SetGlobalSecurity(devSecurity);

		// Delete all devices - necessary so that we're not trying to add
		// a new device which already exists...
		TBTRegistrySearch view;
		view.FindAll();
		btReg.CreateView(view,status);
		User::WaitForRequest(status);	
		btReg.DeleteAllInView(status);
		User::WaitForRequest(status);	
		
		// Add the banned device
		btReg.AddDeviceL(*btDevice,status);
		User::WaitForRequest(status);	
		
		if(status.Int()==KErrNone)
			{			
			_LIT(KDeviceBanned, "A device has been banned" );
			INFO_PRINTF1(KDeviceBanned);
			}
		else 
			{
			SetTestStepResult(EFail);
			_LIT(KDeviceNotBanned, "A device could not be banned, err: %d" );
			INFO_PRINTF2(KDeviceNotBanned,status.Int());
			}
		btReg.CloseView();
		btReg.Close();
		btRegServ.Close();
		CleanupStack::Pop(btDevice);
		}

	return TestStepResult();
	} 


// End of File


