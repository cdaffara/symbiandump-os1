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
// Example CTestStep derived implementation
// 
//

/**
 @file lbsnetlocmgrCancelTest.cpp
*/
#include "lbsnetlocmgrCancelTest.h"
#include "Te_LbsNetLocMgrTestSuiteDefs.h"
#include "LbsInternalInterface.h"


#include "lbsprocesssupervisor.h"


ClbsnetlocmgrCancelTest::~ClbsnetlocmgrCancelTest()
/**
 * Destructor
 */
	{
	}

ClbsnetlocmgrCancelTest::ClbsnetlocmgrCancelTest()
/**
 * Constructor
 */
	{
	SetTestStepName(KLbsnetlocmgrCancelTest);
	}

TVerdict ClbsnetlocmgrCancelTest::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_LbsNetLocMgrTestSuiteStepBase::doTestStepPreambleL();
	if (TestStepResult()!=EPass)
	    return TestStepResult();
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict ClbsnetlocmgrCancelTest::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	INFO_PRINTF1(_L("Simulate location server sending location update request"));
	INFO_PRINTF1(_L("Check that location published by manager is correct"));	  
	
	if (TestStepResult()==EPass)
		{
		//Initialize the process supervisor
		RLbsProcessSupervisor lbsSupervisor;
		lbsSupervisor.InitializeL();
		lbsSupervisor.OpenL(KLbsNetLocManagerUid);
		
		// simulate what is done by Location Server when it wants to cancel
		TLbsPositionUpdateRequestCancel cancelReq;
		TInt err= KErrNone;
		TRAP(err, iPositionReq.SetPositionUpdateRequest(cancelReq);)
		if (err!=KErrNone)
			{
			INFO_PRINTF1(_L("\t Send request failed.")); 
			SetTestStepResult(EFail);	
			return TestStepResult();			
			}			
		INFO_PRINTF1(_L("Send location request to netlocmanager."));
			
		// wait for response
		User::WaitForRequest(iStatusUpd);
		
		if(iStatusUpd.Int()!=KErrNone)
			{
			INFO_PRINTF1(_L("\t response to Cancel failed.")); 
			SetTestStepResult(EFail);	
			return TestStepResult();			
			}			
		
		lbsSupervisor.CloseDownProcess();	
		
		lbsSupervisor.Close();
		
		SetTestStepResult(EPass);
		}
	  return TestStepResult();
	}



TVerdict ClbsnetlocmgrCancelTest::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_LbsNetLocMgrTestSuiteStepBase::doTestStepPostambleL();

	SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}
