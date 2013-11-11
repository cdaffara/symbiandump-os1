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
// This step sets the secondary device which will be connected to if the BT GPS PSY
// fails on the first device that the test notifier provides. It also sets the test
// notifiers 'mode':
// 0 - Keep trying the address at KDevNotifCenrepHwKey infinetly
// 1 - Try the address at KDevNotifCenrepHwKey once, then try KDevNotifCenrepSecondaryHwKey
// 2 - Try the address at KDevNotifCenrepHwKey, then stop trying.
//



/**
 @file setsecondaryhwaddrstep.cpp
 @internalTechnology
*/

#include "setsecondaryhwaddrstep.h"
#include "Te_LbsBtGpsPsyDefs.h"
#include <lbs.h>
#include <bt_sock.h>
// CONSTANTS


CSetSecondaryHwAddrStep::~CSetSecondaryHwAddrStep()
/**
 * Destructor
 */
	{
	}

CSetSecondaryHwAddrStep::CSetSecondaryHwAddrStep()
/**
 * Constructor
 */ 
	{
	SetTestStepName(KSetSecondaryHwAddrStep); 
	}

TVerdict CSetSecondaryHwAddrStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	
	CTe_LbsBtGpsPsyStepBase::doTestStepPreambleL();
	if (TestStepResult()!=EPass)
	    return   TestStepResult();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CSetSecondaryHwAddrStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	if (TestStepResult()==EPass)
		{
    	_LIT(KSetSecondary, "**** Setting secondary address ****");
    	INFO_PRINTF1(KSetSecondary);
		// Get the maximum allowed attempts to this primary device
		TInt notifierType = 1;
		if(!GetIntFromConfig(ConfigSection(),KTe_LbsBtGpsPsySuiteNotifierType,notifierType))
			{
			// Print out here that no notiferType was found
	    	_LIT(KNoNotifierType, "No notifier type found, setting to default of type 1");
	    	INFO_PRINTF1(KNoNotifierType);
			}
		
		// Set the notifier type, the notifier types are as follows:
		// 0 - Keep trying the address at KDevNotifCenrepHwKey infinetly
		// 1 - Try the address at KDevNotifCenrepHwKey once, then try KDevNotifCenrepSecondaryHwKey
		// 2 - Try the address at KDevNotifCenrepHwKey, then stop trying.
		iBtGpsSim.SetParameter(EBtGpsSimLoopSelections, (notifierType == 0));

		
		TPtrC hwAddrPtr;
		if(!GetStringFromConfig(ConfigSection(),KTe_LbsBtGpsPsySuiteSecondaryString,hwAddrPtr)
			)
			{
			// Leave if there's any error.
			User::Leave(KErrNotFound);
			}
		TInt64 hwAddrInt(0);
		TLex lex(hwAddrPtr);
		User::LeaveIfError(lex.Val(hwAddrInt, EHex));
		
		if (hwAddrInt != 0)
			{				
			TBTDevAddr hwDevAddr = hwAddrInt;
			TBTSockAddr hwSockAddr;
			hwSockAddr.SetBTAddr(hwDevAddr);
			TBuf<KMaxSockAddrSize> addressBuffer;
			addressBuffer.Copy(hwSockAddr);
	
			// Set the secondary hardware address for the test notifier.
	
			RBtGpsSimDevice simDevice;
			TInt err = simDevice.Open(iBtGpsSim, hwSockAddr);
			if (err == KErrNotFound)
				{
				err = simDevice.Create(iBtGpsSim, EBtGpsSimReal, hwSockAddr);
				simDevice.SetParameter(EBtGpsSimAuthorised, ETrue);
				simDevice.SetParameter(EBtGpsSimPin, _L8("0000"));
				}
	
			if (err == KErrNone)
				{
				simDevice.AddToSelectionList();
				SetTestStepResult(EPass);
				}
			else
				{
				SetTestStepResult(EFail);
				}
	
			simDevice.Close();
			}
		}
	return TestStepResult();
	}



TVerdict CSetSecondaryHwAddrStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}




// End of File


