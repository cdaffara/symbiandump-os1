// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file ctlbsprivfwstepperftimings.cpp
// This is the class implementation for the LBS privacy handler performance timing test
// 
//

#include <e32base.h>

#include "ctlbsprivfwstepperftimings.h"
#include <lbs/lbsloccommon.h>

#include "ctlbsprivfwnotifierdataobserver.h"
#include "ctlbsprivfwapidataobserver.h"


//_LIT( KTxtRequestorName,	"Requester" );
//_LIT( KTxtLcsClientName,	"LcsClient" );

// TODO may take these consts from .ini file
const TInt KOneRequest = 1;
const TInt KTimeLimitForOneRequest = 100000; // 100mS
const TInt K100Requests = 100;
const TInt KTimeLimitFor100Requests = 5 * 1000000; // 5 seconds
const TInt K5seconds = 5 * 1000000; // 5 seconds

_LIT(KNrhName, "lbsnetworkrequesthandler.exe");
_LIT(KNgName, "lbsnetgateway.exe");


/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
TVerdict CT_LbsPrivFwStepPerfTimings::doTestStepPreambleL()
	{
	// Process some common pre setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsPrivFwStepPerfTimings::doTestStepPreambleL()"));
	CT_LbsPrivFwStepBase::doTestStepPreambleL();
	
	switch(iTestCaseId)
		{
		case 1:	
			{					
			// Ensure all of the LBS privacy processes are NOT running for this test case
			TInt i = 0;
			while (IsProcessRunning(KNrhName) || IsProcessRunning(KNgName))
				{
				User::After(K5seconds);
				if (i++ > 6)
					{
					INFO_PRINTF1(_L("FAIL: Test not implemented or test case ID is unknown."));
					SetTestStepResult(EFail);
					User::Leave(KErrGeneral);
					}
				}
				
			iRequestCount = KOneRequest;
			iFailTimeLimit = KTimeLimitForOneRequest;
			break;
			}
			
		case 2:	
			{					
			iRequestCount = K100Requests;
			iFailTimeLimit = KTimeLimitFor100Requests;
			break;
			}
	
		default:
			{
			INFO_PRINTF1(_L("FAIL: Test not implemented or test case ID is unknown."));
			SetTestStepResult(EFail);
			break;
			}
		}


	return TestStepResult();
	}


/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
TVerdict CT_LbsPrivFwStepPerfTimings::doTestStepPostambleL()
	{
	// Process some common post setting to test steps.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsPrivFwStepPerfTimings::doTestStepPostabmleL()"));

	return TestStepResult();
	}


CT_LbsPrivFwStepPerfTimings::~CT_LbsPrivFwStepPerfTimings()
	{
	delete iRequestSubmitter;
	}


CT_LbsPrivFwStepPerfTimings::CT_LbsPrivFwStepPerfTimings(CT_LbsPrivFwServer& aParent) : CT_LbsPrivFwStepBase(aParent)
	{
	SetTestStepName(KLbsPrivFwStep_PerfTimings);
	}


CT_LbsPrivFwStepPerfTimings* CT_LbsPrivFwStepPerfTimings::New(CT_LbsPrivFwServer& aParent)
{

CT_LbsPrivFwStepPerfTimings* testStep = new CT_LbsPrivFwStepPerfTimings(aParent);
// Note the lack of ELeave.
// This means that having insufficient memory will return NULL;

if (testStep)
	{
	TInt err = KErrNone;

	TRAP(err, testStep->ConstructL());
	if (err)
		{
		delete testStep;
		testStep = NULL;
		}
	}
	
return testStep;
}


void CT_LbsPrivFwStepPerfTimings::ConstructL()
	{
	// Create the base class objects.
	CT_LbsPrivFwStepBase::ConstructL();
	}


TVerdict CT_LbsPrivFwStepPerfTimings::doTestStepL()
	{
	//Connect to Network Privacy API
	CPrivFwApiWrapper* netPrivacy = CPrivFwApiWrapper::NewL(iApiUsed);
	CleanupStack::PushL(netPrivacy);

	// make the request submitter active object
	iRequestSubmitter = CT_LbsPrivFwReqSubmitter::NewL(iProxy, netPrivacy, iApiObserver,
			iPrivacyHandlerType, this, iArray);
	
	//Start all the active objects
	iNotifierObserver->Start(); 
	iApiObserver->Start();
	iRequestSubmitter->StartL(iRequestCount);
	
	CActiveScheduler::Start();
	
	TTime endTime;
	endTime.UniversalTime(); // stop the clock
	TTime startTime = iRequestSubmitter->StartTime();
	TTimeIntervalMicroSeconds interval;
	
	interval = endTime.MicroSecondsFrom(startTime);
	
#ifndef __WINS__
#ifndef	_DEBUG // it doesn't make sense to test these performance timings on a udeb platform 	
	// Write results into a log file to publish the results in the SMG dashboard (TODO)
	if (interval.Int64() > iFailTimeLimit.Int64())
		{
		ERR_PRINTF1(_L("CT_LbsPrivFwStepPerfTimings::doTestStepL() - Exceeded performance timing"));
		SetTestStepResult(EFail);
		}
#endif
#endif	
	INFO_PRINTF3(_L("Time taken to perform %d request(s) = %d microS"), iRequestCount, interval.Int64());
	INFO_PRINTF3(_L("Target time for       %d request(s) = %d microS"), iRequestCount, iFailTimeLimit.Int64());

	CleanupStack::PopAndDestroy(1);
	
	return TestStepResult();
	}



	

