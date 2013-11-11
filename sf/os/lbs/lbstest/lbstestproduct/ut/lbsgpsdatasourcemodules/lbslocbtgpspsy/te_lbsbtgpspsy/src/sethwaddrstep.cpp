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
// This step sets the address that the test notifier will use to give to the BT GPS PSY. It's
// secondary purpose is to reset the number of requests made on the test notifier to zero and
// to reset the test notifiers mode to 0.
//



/**
 @file sethwaddrstep.cpp
 @internalTechnology
*/
#include "sethwaddrstep.h"
#include "Te_LbsBtGpsPsyDefs.h"
#include <lbs.h>
#include <bt_sock.h>
// CONSTANTS




CSetHwAddrStep::~CSetHwAddrStep()
/**
 * Destructor
 */
	{
	} 

CSetHwAddrStep::CSetHwAddrStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KSetHwAddrStep);
	}

TVerdict CSetHwAddrStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	if (TestStepResult()==EPass)
		{
		iBtGpsSim.ClearSelectionList();
		iBtGpsSim.SetParameter(EBtGpsSimLoopSelections, ETrue);
		
		TPtrC hwAddrPtr;
		if(!GetStringFromConfig(ConfigSection(),KTe_LbsBtGpsPsySuiteString,hwAddrPtr))
			{ 
			// Leave if there's any error.
			User::Leave(KErrNotFound);
			}
		TInt64 hwAddrInt(0);
		TLex lex(hwAddrPtr);
		User::LeaveIfError(lex.Val(hwAddrInt, EHex));
		
		if (hwAddrInt != 0)
			{
			// Set the device address for the test notifier
			TBTDevAddr hwDevAddr = hwAddrInt;
			TBTSockAddr hwSockAddr;
			hwSockAddr.SetBTAddr(hwDevAddr);
			TBuf<KMaxSockAddrSize> addressBuffer;
			addressBuffer.Copy(hwSockAddr);
	
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

// End of File


