// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file ctlbsprivfwstepprivtimeout.cpp
// This is the class implementation for the LBS Client Test Step Base
// 
//

#include "ctlbsprivfwstepprivtimeout.h"
#include <lbs/lbsloccommon.h>

#include "ctlbsprivfwnotifierdataobserver.h"
#include "ctlbsprivfwapidataobserver.h"

const TInt K1Sec = 1000000; 
const TInt K2Sec = 2000000;
const TInt K5seconds = 5 * K1Sec; 
const TInt KLbsShutdownTimeout = 2000000; // LBS shutdown timeout value of 2 secs

const TInt repeatcount = 2;
const TInt timeoutValues[] = {10, 15}; // values in Seconds to use as example timeouts
const TPosNetworkPrivacyRequestInfoType gsmInfoType = ERequestInfoTypeGsm;

// All LBS Process names
_LIT(KNrhName, "lbsnetworkrequesthandler*");
_LIT(KNgName, "lbsnetgateway*");
_LIT(KLocManName, "lbsgpslocmanager*");
_LIT(KLocSvrName, "lbslocserver*");
_LIT(KNetLocManName, "lbsnetlocmanager*");
_LIT(KRootName, "lbsroot*");
_LIT(KCtlrName, "lbstestprivacycontroller*");

_LIT( KTxtRequestorName,	"Requester" );
_LIT( KTxtLcsClientName,	"LcsClient" );


/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
TVerdict CT_LbsPrivFwStepPrivTimeout::doTestStepPreambleL()
	{
	// Process some common pre setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsPrivFwStepPrivTimeout::doTestStepPreambleL()"));

	// Ensure all of the LBS privacy processes are NOT running for this test case
	TInt i = 0;
	while (IsProcessRunning(KNrhName) || IsProcessRunning(KNgName))
		{
		User::After(K5seconds);
		if (i++ > 6)
			{
			ERR_PRINTF1(_L("FAIL: Privacy processes still running so cannot execute this test."));
			SetTestStepResult(EFail);
			User::Leave(KErrGeneral);
			}
		}
		
	SetTestStepResult(EPass);

	return TestStepResult();
	}


/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
TVerdict CT_LbsPrivFwStepPrivTimeout::doTestStepPostambleL()
	{
	// Process some common post setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsPrivFwStepPrivTimeout::doTestStepPostabmleL()"));

	//SetTestStepResult(EPass);  // or EFail
	return TestStepResult();
	}


CT_LbsPrivFwStepPrivTimeout::~CT_LbsPrivFwStepPrivTimeout()
	{
    if (iTimer)
        {
        iTimer->Cancel();
        delete iTimer;
        }
 	}


CT_LbsPrivFwStepPrivTimeout::CT_LbsPrivFwStepPrivTimeout(CT_LbsPrivFwServer& aParent) : CT_LbsPrivFwStepBase(aParent)
	{
	SetTestStepName(KLbsPrivFwStep_PrivTimeout);
	}


CT_LbsPrivFwStepPrivTimeout* CT_LbsPrivFwStepPrivTimeout::New(CT_LbsPrivFwServer& aParent)
{

CT_LbsPrivFwStepPrivTimeout* testStep = new CT_LbsPrivFwStepPrivTimeout(aParent);
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


void CT_LbsPrivFwStepPrivTimeout::ConstructL()
	{
	// Create the base class objects.
	CT_LbsPrivFwStepBase::ConstructL();
	
    iTimer = CPeriodic::NewL(CActive::EPriorityStandard);
	}


TVerdict CT_LbsPrivFwStepPrivTimeout::doTestStepL()
	{
	//Connect to Network Privacy API
	CPrivFwApiWrapper* netPrivacy = CPrivFwApiWrapper::NewL(iApiUsed);
	CleanupStack::PushL(netPrivacy);
	
	//Make sure that all known processes are stopped - nothing starts by just connecting
	User::After(K5seconds);
	if (!(IsProcessStopped(KNrhName) &&
		 IsProcessStopped(KNgName) &&
		 IsProcessStopped(KLocManName) &&
		 IsProcessStopped(KLocSvrName) &&
		 IsProcessStopped(KNetLocManName) &&
		 IsProcessStopped(KRootName) &&
		 IsProcessStopped(KCtlrName) ))
		{
		ERR_PRINTF1(_L("FAIL: Illegal initial state - at least 1 LBS process is still running."));
		SetTestStepResult(EFail);
		User::Leave(KErrGeneral);
		}
	
	// Save original default timeout value
	TTimeIntervalMicroSeconds32 originalPrivacyTimeout(0);
	originalPrivacyTimeout = netPrivacy->ConnectionTimeout();

	// Start the observer active objects
	iNotifierObserver->Start(); 
	iApiObserver->Start();

	const TInt maxNameLen = 20;
	TBuf<maxNameLen> reqInfoName;
	RPointerArray<CPosGSMPrivacyRequestInfo>* reqInfoArray;
  	reqInfoArray = new (ELeave) RPointerArray<CPosGSMPrivacyRequestInfo>();
	CleanupStack::PushL(reqInfoArray);
	
	for (TInt i = 0; i<repeatcount; i++)
		{
		// Set up the Privacy API timeout and make sure it is set
		iTimeout = timeoutValues[i] * K1Sec;
		netPrivacy->SetConnectionTimeout(iTimeout);
		if (iTimeout != netPrivacy->ConnectionTimeout() )
			{
			ERR_PRINTF2(_L("FAIL: Cannot set a new timeout value of %d."), iTimeout.Int());
			SetTestStepResult(EFail);
			User::Leave(KErrGeneral);
			}
		
		// initialise the shutdown state 
		iState = EWaitingForRequests;
		
		//Build and submit a verification request
		reqInfoArray->AppendL(CPosGSMPrivacyRequestInfo::NewL());
		reqInfoName = KTxtRequestorName;
		reqInfoName.AppendNumFixedWidth(i, EDecimal, 2);
		(*reqInfoArray)[i]->SetRequestorL(reqInfoName, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		reqInfoName = KTxtLcsClientName;
		reqInfoName.AppendNumFixedWidth(i, EDecimal, 2);
		(*reqInfoArray)[i]->SetLCSClientL(reqInfoName, CPosGSMPrivacyRequestInfo::EIdTypeLogicalName);
		(*reqInfoArray)[i]->SetRequestType(CPosNetworkPrivacyRequestInfo::ERequestSingleShot);
		SubmitVerifyLocationRequest((*reqInfoArray)[i], gsmInfoType, iApiObserver, netPrivacy, 
				CPosNetworkPrivacy::EDecisionAccepted, // If a timeout happens, do this...
				EResponseTypeAccepted); // I want this to happen...


		CActiveScheduler::Start();
		
		// arrive here (after Start) when the request has completed and all timeout checks exercised
		// so repeat for all example timeout values

		}

	// reset the timeout to the original value.
	netPrivacy->SetConnectionTimeout(originalPrivacyTimeout);

	CleanupStack::Pop();
	reqInfoArray->Reset();
	delete reqInfoArray;

	CleanupStack::PopAndDestroy(netPrivacy);
	
	return TestStepResult();
	}


void CT_LbsPrivFwStepPrivTimeout::SubmitVerifyLocationRequest(CPosGSMPrivacyRequestInfo* aReqInfo, 
		TPosNetworkPrivacyRequestInfoType aPrivacyInfoType,
		CT_LbsPrivFwApiDataObserver* aPrivObserver, 
		CPrivFwApiWrapper* aNetPrivacy, 
		CPosNetworkPrivacy::TRequestDecision aDecision,
		TResponseType aIntendedDecision, TUint aResponseDelay)
	{
	(void)aResponseDelay;
	
	CT_LbsPrivFwVerifyRequest* req = CT_LbsPrivFwVerifyRequest::NewL(iProxy, aNetPrivacy, aPrivObserver, iPrivacyHandlerType, this);		

	req->SetRequestInfo(aReqInfo, aPrivacyInfoType);
	
	// Set what the outcome should be
	req->SetDesiredResponse(aIntendedDecision);
	// Set what should happen if a timeout occurs
	req->SetDecision(aDecision);
	//Issue the verification request
	req->Start();
	
	// Add the request to the array (which takes ownership)
	iArray.AppendL(req);
	}

void CT_LbsPrivFwStepPrivTimeout::CheckShutdown()
	{
	if (iState == EWaitingForRequests)
		{
		TInt arraySize = iArray.Count();
		TInt terminals = 0;
		for(int i = 0; i < arraySize; i++)
			{
			if(iArray[i]->IsRequestComplete())
				{
				terminals++;
				}
			else
				{
				break;
				}
			}
		if (terminals == arraySize)
			{
			iState = EAllRequestsComplete;
		    TTimeIntervalMicroSeconds32 delay(0);
		    TTimeIntervalMicroSeconds32 interval(K2Sec);
		    // delay is set to 1 sec before we expect LBS to be closed, the interval for 
		    // the next periodic event is set to 2 sec i.e 1 sec after we expect LBS to close
			delay = iTimeout.Int() + KLbsShutdownTimeout - K1Sec; 
		    iTimer->Start(delay, interval, TCallBack(TimerCallBack, this));
			}
		}
	else if (iState == EAllRequestsComplete)
		{
		// move on to next state 
		iState = EPrivacyAPInotTimedOut;
		
		//and check all relevant processes
		if (IsProcessStopped(KNrhName))
			{
			ERR_PRINTF1(_L("FAIL: Network Request Handler has stopped"));
			SetTestStepResult(EFail);
			}
		if (IsProcessStopped(KNgName))
			{
			ERR_PRINTF1(_L("FAIL: Network Gateway has stopped"));
			SetTestStepResult(EFail);
			}
		if ((iPrivacyHandlerType == EPrivacyHandleByController) && IsProcessStopped(KCtlrName))
			{
			ERR_PRINTF1(_L("FAIL: Privacy Controller has stopped"));
			SetTestStepResult(EFail);
			}
		if (IsProcessRunning(KLocManName))
			{
			ERR_PRINTF1(_L("FAIL: Loc Manager is running"));
			SetTestStepResult(EFail);
			}
		if (IsProcessRunning(KLocSvrName))
			{
			ERR_PRINTF1(_L("FAIL: Loc Server is running"));
			SetTestStepResult(EFail);
			}
		if (IsProcessRunning(KNetLocManName))
			{
			ERR_PRINTF1(_L("FAIL: Network Loc Manager is running"));
			SetTestStepResult(EFail);
			}
		if (IsProcessRunning(KRootName))
			{
			ERR_PRINTF1(_L("FAIL: LBS Root is running"));
			SetTestStepResult(EFail);
			}
		if (TestStepResult() == EPass)
			{
			INFO_PRINTF1(_L("at pre-timeout, NRH and the Gateway are both running all other processes stopped "));
			}
		}
	else
		{
		User::After(K2Sec);
		// move on to next state 
		iState = EPrivacyAPITimedOut;
		
		// kill the interval timer
	    if (iTimer)
	        {
	        iTimer->Cancel();
	        }
		//Check all relevant processes - all should be stopped
		if (IsProcessRunning(KNrhName))
			{
			ERR_PRINTF1(_L("FAIL: Network Request Handler is running"));
			SetTestStepResult(EFail);
			}
		if (IsProcessRunning(KNgName))
			{
			ERR_PRINTF1(_L("FAIL: Network Gateway his running"));
			SetTestStepResult(EFail);
			}
		if (IsProcessRunning(KLocManName))
			{
			ERR_PRINTF1(_L("FAIL: Loc Manager is running"));
			SetTestStepResult(EFail);
			}
		if (IsProcessRunning(KLocSvrName))
			{
			ERR_PRINTF1(_L("FAIL: Loc Server is running"));
			SetTestStepResult(EFail);
			}
		if (IsProcessRunning(KNetLocManName))
			{
			ERR_PRINTF1(_L("FAIL: Network Loc Manager is running"));
			SetTestStepResult(EFail);
			}
		if (IsProcessRunning(KRootName))
			{
			ERR_PRINTF1(_L("FAIL: LBS Root is running"));
			SetTestStepResult(EFail);
			}
		if (IsProcessRunning(KCtlrName))
			{
			ERR_PRINTF1(_L("FAIL: Privacy Controller is running"));
			SetTestStepResult(EFail);
			}

		if (TestStepResult() == EPass)
			{
			INFO_PRINTF1(_L("at post-timeout, ALL processes stopped "));
			}
		
		// And return to main body of the test
		CActiveScheduler::Stop();
		}
	}

void CT_LbsPrivFwStepPrivTimeout::ProcessNotiferCtrlCloseDown()
	{
	//Do nothing
	}


TInt CT_LbsPrivFwStepPrivTimeout::TimerCallBack(TAny* aPtr)
    {
    ((CT_LbsPrivFwStepPrivTimeout*)aPtr)->CheckShutdown();
    return 1; 
    }


