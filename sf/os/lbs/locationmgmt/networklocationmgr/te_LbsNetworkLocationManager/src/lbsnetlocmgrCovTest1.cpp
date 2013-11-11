
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
 @file LbsnetlocmgrCovTest1.cpp
*/
#include "lbsnetlocmgrCovTest1.h"
#include "Te_LbsNetLocMgrTestSuiteDefs.h"
#include "cprocesslaunch.h"
#include "tprocessstartparams.h"

#include "lbsprocesssupervisor.h"

ClbsnetlocmgrCovTest1::~ClbsnetlocmgrCovTest1()
/**
 * Destructor
 */
	{
	}

ClbsnetlocmgrCovTest1::ClbsnetlocmgrCovTest1()
/**
 * Constructor
 */
	{
	SetTestStepName(KLbsnetlocmgrCovTest1);
	}

TVerdict ClbsnetlocmgrCovTest1::doTestStepPreambleL()
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


TVerdict ClbsnetlocmgrCovTest1::doTestStepL()
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
		

		//normal request that responds with wrong session id
		TLbsPositionUpdateRequest request;
		TTime targetTime = 0;
		targetTime.UniversalTime(); 
		targetTime += (TTimeIntervalSeconds)1;
		TLbsLocRequestQualityInt requestQuality;				
		requestQuality.SetMaxFixTime(0); //used to identify request on gateway side
		requestQuality.SetMinHorizontalAccuracy(0.1);
		requestQuality.SetMinVerticalAccuracy(0.1);
		request.TargetTime() = targetTime;
		request.RequestQuality() = requestQuality;
		
		TInt err= KErrNone;
		TRAP(err, iPositionReq.SetPositionUpdateRequest(request);)
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
			INFO_PRINTF1(_L("\t response failed.")); 
			SetTestStepResult(EFail);	
			return TestStepResult();			
			}	
		

		//normal request that responds with unknown response type
		iPositionUpd.NotifyPositionUpdate(iStatusUpd);	
		targetTime = 0;
		targetTime.UniversalTime();
		targetTime += (TTimeIntervalSeconds)1; 
		requestQuality.SetMaxFixTime(1); //used to identify request on gateway side
		requestQuality.SetMinHorizontalAccuracy(0.1);
		requestQuality.SetMinVerticalAccuracy(0.1);
		request.TargetTime() = targetTime;
		request.RequestQuality() = requestQuality;
		
		err= KErrNone;
		TRAP(err, iPositionReq.SetPositionUpdateRequest(request);)
		if (err!=KErrNone)
			{
			INFO_PRINTF1(_L("\t Send request failed.")); 
			SetTestStepResult(EFail);	
			return TestStepResult();			
			}			
		INFO_PRINTF1(_L("Send location request to netlocmanager."));
			
		// wait for response
		User::WaitForRequest(iStatusUpd);
		
		if(iStat.Int()!=KErrNone)
			{
			INFO_PRINTF1(_L("\t response failed.")); 
			SetTestStepResult(EFail);	
			return TestStepResult();			
			}	


		//close down
		lbsSupervisor.CloseDownProcess();
		
		lbsSupervisor.Close();
		

		SetTestStepResult(EPass);
		}
	  return TestStepResult();
	}



TVerdict ClbsnetlocmgrCovTest1::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_LbsNetLocMgrTestSuiteStepBase::doTestStepPostambleL();
	SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}
