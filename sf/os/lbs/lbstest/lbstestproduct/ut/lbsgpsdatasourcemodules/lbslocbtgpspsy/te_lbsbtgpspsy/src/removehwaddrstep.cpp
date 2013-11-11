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
// This step removes a hardware address from the central repository for the test notifier
// as well as unpairing the device (if it was paired).
//



/**
 @file removehwaddrstep.cpp
 @internalTechnology
*/
#include "removehwaddrstep.h"
#include "Te_LbsBtGpsPsyDefs.h"
#include <lbs.h>
#include <bt_sock.h>
#include <btmanclient.h>
// CONSTANTS




CRemoveHwAddrStep::~CRemoveHwAddrStep()
/**
 * Destructor
 */
	{
	} 

CRemoveHwAddrStep::CRemoveHwAddrStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KRemoveHwAddrStep);
	}

TVerdict CRemoveHwAddrStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	if (TestStepResult()==EPass)
		{
		// Connect to the bt registry
		RBTRegistry btReg;
		RBTRegServ btRegServ;
		CleanupClosePushL(btRegServ);
		CleanupClosePushL(btReg);
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
		btReg.UnpairDevice(hwDevAddr,status);
		User::WaitForRequest(status);	
		if(status.Int()==KErrNone)
			{			
			_LIT(KDeviceUnpaired, "A device has been unpaired" );
			INFO_PRINTF1(KDeviceUnpaired);
			}
		else
		if(status.Int()==KErrNotFound)
			{
			_LIT(KDeviceWasntPaired, "The device wasnt paired" );
			INFO_PRINTF1(KDeviceWasntPaired);
			}
		else 
			{
			SetTestStepResult(EFail);
			_LIT(KDeviceUnpaired, "A device could not be unpaired, err: %d" );
			INFO_PRINTF2(KDeviceUnpaired,status.Int());
			}
		CleanupStack::PopAndDestroy(&btReg);
		CleanupStack::PopAndDestroy(&btRegServ);
		}
	return TestStepResult();
	} 
// End of File


